#include <stdint.h>
#include "commonvars.h"
#include "helperfuncs.h"
#include "savestate.h"

//magenta, the colour the images use for transparent pixels, in RGB565
#define TRANSPARENT_COLOR 0xF81F

//the selector tiles are identical in every skin, only the default copy is included to save flash
#include "images/default/selectortiles_RGB565_LE.h"

//only the skins FORCESKIN leaves in are part of the build (a 1 bpp buffer forces the black & white one)
#if SKINBUILT(skinBlackWhite)
#include "images/blackwhite/blocktiles_RGB565_LE.h"
#include "images/blackwhite/congratsscreen_RLE565.h"
#include "images/blackwhite/congratstiles_RGB565_LE.h"
#include "images/blackwhite/titlescreen_RLE565.h"
#endif

#if SKINBUILT(0)
#include "images/default/blocktiles_RGB565_LE.h"
#include "images/default/congratsscreen_RLE565.h"
#include "images/default/congratstiles_RGB565_LE.h"
#include "images/default/titlescreen_RLE565.h"
#endif

#if SKINBUILT(2)
#include "images/viaduct/blocktiles_RGB565_LE.h"
#include "images/viaduct/congratsscreen_RLE565.h"
#include "images/viaduct/congratstiles_RGB565_LE.h"
#include "images/viaduct/titlescreen_RLE565.h"
#endif

#if SKINBUILT(3)
#include "images/sonic/blocktiles_RGB565_LE.h"
#include "images/sonic/congratsscreen_RLE565.h"
#include "images/sonic/congratstiles_RGB565_LE.h"
#include "images/sonic/titlescreen_RLE565.h"
#endif


const uint8_t* currentTiles;

//the skin in use: the one FORCESKIN builds in (a 1 bpp buffer forces the black & white one), or
//the one picked in the options
uint8_t currentSkin(void)
{
#if FORCESKIN >= 0
    return FORCESKIN;
#else
    return skinSaveState();
#endif
}

void preloadImages(void)
{
    switch(currentSkin())
    {
#if SKINBUILT(0)
        //default
        case 0:
            ColorWhite = SCREEN.color565(123,186,255);
	        ColorBlack = SCREEN.color565(0,65,132);
            blockTiles = default_blocktiles_data;
            selectorTiles = default_selectortiles_data;
            congratsScreenTiles = default_congratstiles_data;
            imgTitleScreen = default_titlescreen_rle;
            imgCongratsScreen = default_congratsscreen_rle;
            setBlockTilesAsBackground();
            break;
#endif
#if SKINBUILT(skinBlackWhite)
        //blackwhite
        case 1:
            ColorWhite = SCREEN.color565(255,255,255);
	        ColorBlack = SCREEN.color565(0,0,0);
            blockTiles = black_white_blocktiles_data;
            selectorTiles = default_selectortiles_data;
            congratsScreenTiles = black_white_congratstiles_data;
            imgTitleScreen = black_white_titlescreen_rle;
            imgCongratsScreen = black_white_congratsscreen_rle;
            setBlockTilesAsBackground();
            break;
#endif
#if SKINBUILT(2)
        //viaduct
        case 2:
            ColorWhite = SCREEN.color565(210,210,210);
	        ColorBlack = SCREEN.color565(125,125,125);
            blockTiles = viaduct_blocktiles_data;
            selectorTiles = default_selectortiles_data;
            congratsScreenTiles = viaduct_congratstiles_data;
            imgTitleScreen = viaduct_titlescreen_rle;
            imgCongratsScreen = viaduct_congratsscreen_rle;
            setBlockTilesAsBackground();
            break;
#endif
#if SKINBUILT(3)
        //sonic
        case 3:
            ColorWhite = SCREEN.color565(204,96,0);
	        ColorBlack = SCREEN.color565(119,53,0);
            blockTiles = sonic_blocktiles_data;
            selectorTiles = default_selectortiles_data;
            congratsScreenTiles = sonic_congratstiles_data;
            imgTitleScreen = sonic_titlescreen_rle;
            imgCongratsScreen = sonic_congratsscreen_rle;
            setBlockTilesAsBackground();
            break;
#endif
    }
}

#if SCREENBUFFER
//The whole screen is drawn again from tiles with a buffer, tens of thousands of pixels, so
//this is kept lean: the image is clipped once, every visible row comes out of flash in
//one copy and is written through a row pointer, nothing is worked out per pixel
static void drawImageToBuffer(int x, int y, int w, int h, const uint16_t* data, bool transparent)
{
    void* buffer = SCREENBUFFER_PIXELS();
    if (!buffer)
        return;
    int c0 = (x < 0) ? -x : 0;
    int c1 = (x + w > WINDOW_WIDTH) ? WINDOW_WIDTH - x : w;
    int r0 = (y < 0) ? -y : 0;
    int r1 = (y + h > WINDOW_HEIGHT) ? WINDOW_HEIGHT - y : h;
    if ((c0 >= c1) || (r0 >= r1))
        return;
    const int cols = c1 - c0;
    const int dx = x + c0;
    //little endian RGB565 like both devices, so the bytes can be copied straight into it
    uint16_t row[WINDOW_WIDTH];
    for (int r = r0; r < r1; r++)
    {
        const int dy = y + r;
        PLATFORM_READ_BYTES((uint8_t*)row, data + c0 + r * w, cols * sizeof(uint16_t));
  #if SCREENBUFFER == 16
        uint16_t* d = &((uint16_t*)buffer)[dy * WINDOW_WIDTH + dx];
        for (int c = 0; c < cols; c++)
        {
            uint16_t color = row[c];
            //magenta is the transparent key, a 16 bpp sprite keeps its pixels byte swapped
            if (!transparent || (color != TRANSPARENT_COLOR))
                d[c] = (uint16_t)((color >> 8) | (color << 8));
        }
  #elif SCREENBUFFER == 8
        uint8_t* d = &((uint8_t*)buffer)[dy * WINDOW_WIDTH + dx];
        for (int c = 0; c < cols; c++)
        {
            uint16_t color = row[c];
            //RGB332, the same conversion SetBufferPixel does
            if (!transparent || (color != TRANSPARENT_COLOR))
                d[c] = (uint8_t)(((color & 0xE000) >> 8) | ((color & 0x0700) >> 6) | ((color & 0x0018) >> 3));
        }
  #else
        for (int c = 0; c < cols; c++)
        {
            uint16_t color = row[c];
            if (!transparent || (color != TRANSPARENT_COLOR))
                SetBufferBit((uint8_t*)buffer, dx + c, dy, color);
        }
  #endif
    }
}
#endif

//Draws an image, skipping its magenta pixels when transparent is set. The display can do
//the keying itself, but TFT_eSprite has no pushImage that skips a transparent colour and
//its 1 bpp pushImage expects 1 bpp image data, so in those cases the pixels are written
//into the buffer directly, in whatever form that buffer keeps them. LovyanGFX reads image
//data through plain pointers, but PROGMEM on the ESP8266 is flash that only takes 32 bit
//reads, so with that library every image is read here with PLATFORM_READ_BYTES, even the ones
//that go straight to the display
static void drawImage(int x, int y, int w, int h, const uint16_t* data, bool transparent)
{
#if (SCREENBUFFER == 0) && !LOVYANGFX
    if (transparent)
        GFX.pushImage(x, y, w, h, data, TRANSPARENT_COLOR);
    else
        GFX.pushImage(x, y, w, h, data);
#elif SCREENBUFFER == 0
    //straight to the display, only the part that is on screen
    int c0 = (x < 0) ? -x : 0;
    int c1 = (x + w > WINDOW_WIDTH) ? WINDOW_WIDTH - x : w;
    int r0 = (y < 0) ? -y : 0;
    int r1 = (y + h > WINDOW_HEIGHT) ? WINDOW_HEIGHT - y : h;
    if ((c0 >= c1) || (r0 >= r1))
        return;
    const int cols = c1 - c0;
    uint16_t line[WINDOW_WIDTH];
    //the chip select sits on the I/O expander, every write transaction costs I2C
    //traffic, so all the rows go out in one
    SCREEN.startWrite();
    if (!transparent)
    {
        //one window for the whole image, filled a row at a time
        SCREEN.setAddrWindow(x + c0, y + r0, cols, r1 - r0);
        for (int r = r0; r < r1; r++)
        {
            //the visible part of the row in one copy out of flash
            PLATFORM_READ_BYTES((uint8_t*)line, data + c0 + r * w, cols * sizeof(uint16_t));
            //true: the values are plain RGB565, the library puts them in display order
            SCREEN.writePixels(line, cols, true);
        }
    }
    else
    {
        //every run of opaque pixels on a row goes out as one
        for (int r = r0; r < r1; r++)
        {
            //the visible part of the row in one copy out of flash. The runs are gathered at
            //the front of the same line, a run never gets ahead of the pixel being read
            PLATFORM_READ_BYTES((uint8_t*)line, data + c0 + r * w, cols * sizeof(uint16_t));
            int runX = 0, runLen = 0;
            for (int c = 0; c <= cols; c++)
            {
                uint16_t color = TRANSPARENT_COLOR;
                if (c < cols)
                    color = line[c];
                //magenta is the transparent key, it (and the end of the row) closes a run
                if (color != TRANSPARENT_COLOR)
                {
                    if (runLen == 0)
                        runX = x + c0 + c;
                    line[runLen++] = color;
                }
                else if (runLen > 0)
                {
                    SCREEN.setAddrWindow(runX, y + r, runLen, 1);
                    SCREEN.writePixels(line, runLen, true);
                    runLen = 0;
                }
            }
        }
    }
    SCREEN.endWrite();
#else
  #if (SCREENBUFFER != 1) && !LOVYANGFX
    if (!transparent)
    {
        GFX.pushImage(x, y, w, h, data);
        return;
    }
  #endif
    drawImageToBuffer(x, y, w, h, data, transparent);
#endif
}

void set_bkg_tile_xy(int ax, int ay, int tile)
{
    drawImage(BIG_X_OFFSET + ax* tileSize, BIG_Y_OFFSET + ay* tileSize, tileSize, tileSize, (const uint16_t*)(currentTiles + tile * tileSize * tileSize * sizeof(uint16_t)), false);
}

void set_bkg_tile_xy8x8(int ax, int ay, int tile)
{
    pushImageTransparent(ax* 8 + SMALL_X_OFFSET, 8 + ay*SMALL_Y_OFFSET, tileSize, 8, (const uint16_t*)(currentTiles + (tile + 192) * tileSize * 8 * sizeof(uint16_t)));
}

void pushImageTransparent(int x, int y, int w, int h, const uint16_t* data)
{
    drawImage(x, y, w, h, data, true);
}

void set_bkg_data(const uint8_t* tiles)
{
    currentTiles = tiles;
}

//Draws a run length encoded RGB565 image made by tools/png2rle565.py, clipped to the screen:
//the intro scrolls the title screen in from below. A control byte with the top bit set is a
//run of (c & 0x7F) + 1 times the pixel after it, otherwise c + 1 literal pixels follow.
//The data is read with PLATFORM_READ_BYTE and PLATFORM_READ_BYTES: LovyanGFX reads image data
//through plain pointers, but PROGMEM on the ESP8266 is flash that only takes 32 bit reads
static void pushImageRLE(int x, int y, int w, int h, const uint8_t* data)
{
    if (!data || (w <= 0) || (h <= 0))
        return;
    //the columns and rows of the image that are on screen
    const int c0 = (x < 0) ? -x : 0;
    const int c1 = (x + w > WINDOW_WIDTH) ? WINDOW_WIDTH - x : w;
    const int r0 = (y < 0) ? -y : 0;
    const int r1 = (y + h > WINDOW_HEIGHT) ? WINDOW_HEIGHT - y : h;
    if ((c0 >= c1) || (r0 >= r1))
        return;
#if SCREENBUFFER
    void* dst = SCREENBUFFER_PIXELS();
    if (!dst)
        return;
#else
    //the chip select sits on the I/O expander, so the whole image goes out in one transaction
    //and one window. LovyanGFX's pushBlock and pushPixels open one of their own per call,
    //there the write variants are used inside this one
    SCREEN.startWrite();
    SCREEN.setAddrWindow(x + c0, y + r0, c1 - c0, r1 - r0);
#endif
    //a control covers at most 128 pixels
    uint16_t pixels[128];
    uint32_t left = (uint32_t)w * h;
    int cx = 0, cy = 0;
    //decoding stops after the last row on screen
    while ((left > 0) && (cy < r1))
    {
        uint8_t control = PLATFORM_READ_BYTE(data++);
        uint16_t count = (control & 0x7F) + 1;
        if (count > left)
            count = (uint16_t)left;
        const bool run = (control & 0x80) != 0;
        uint16_t color = 0;
        if (run)
        {
            color = PLATFORM_READ_BYTE(data) | (PLATFORM_READ_BYTE(data + 1) << 8);
            data += 2;
        }
        else
        {
            //little endian RGB565 like both devices, so the bytes can be copied straight in
            PLATFORM_READ_BYTES((uint8_t*)pixels, data, count * sizeof(uint16_t));
            data += count * sizeof(uint16_t);
        }
        left -= count;
        //the pixels of the control a row at a time, only the part on screen is drawn
        for (uint16_t done = 0; done < count; )
        {
            int n = w - cx;
            if (n > count - done)
                n = count - done;
            if ((cy >= r0) && (cy < r1))
            {
                const int a = (cx > c0) ? cx : c0;
                const int e = (cx + n < c1) ? cx + n : c1;
                if (a < e)
                {
                    const uint16_t* src = &pixels[done + (a - cx)];
#if SCREENBUFFER
                    const int sy = y + cy;
                    for (int c = a; c < e; c++)
                        SetBufferPixel(dst, x + c, sy, run ? color : src[c - a]);
#elif LOVYANGFX
                    //a uint16_t colour is taken as plain RGB565, true: so are the pixels
                    if (run)
                        SCREEN.writeColor(color, e - a);
                    else
                        SCREEN.writePixels(src, e - a, true);
#else
                    if (run)
                        SCREEN.pushBlock(color, e - a);
                    else
                        SCREEN.pushPixels((uint16_t*)src, e - a);
#endif
                }
            }
            done += n;
            cx += n;
            if (cx == w)
            {
                cx = 0;
                cy++;
            }
        }
    }
#if !SCREENBUFFER
    SCREEN.endWrite();
#endif
}

//the title and congratulations screens, run length encoded
void set_bkg_tiles(int ax, int ay, int w, int h, const uint8_t* bitmap)
{
    pushImageRLE(ax, ay, w, h, bitmap);
}

void setBlockTilesAsBackground(void)
{
    set_bkg_data(blockTiles);
}