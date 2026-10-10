#include <stdint.h>
#include "commonvars.h"
#include "helperfuncs.h"
//the one bit pictures of the black & white skin
#include "onebitimage.h"
#include "savestate.h"
//the art read from a card, and the strips it is drawn into
#include "cardimages.h"
#include "bandrender.h"

//TRANSPARENT_COLOR is in defines.h, the band renderer leaves those pixels out as well

//The selector tiles are the same picture in every skin, so only one copy of them is included. The
//black & white skin keeps its own all the same: its copy is packed one bit a pixel and comes to
//10672 bytes, where the default skin's is 61440 of RGB565, and a build that has only the black &
//white skin in it has no other use for those
#if !CARDIMAGES && (FORCESKIN != skinBlackWhite)
#include "images/default/selectortiles_RGB565_LE.h"
#endif

//A row of an image on its way to the display. Where flash is plain memory an evenly placed
//row is handed over where it lies, otherwise it is copied into the scratch row first. A 16
//bit read needs an even address, a core like the Cortex-M0+ faults on an odd one
static inline const uint16_t* ImageRow(const void* src, uint16_t* scratch, int count)
{
#if PLATFORM_DIRECT_FLASH
    if (((uintptr_t)src & 1) == 0)
        return (const uint16_t*)src;
#endif
    PLATFORM_READ_BYTES((uint8_t*)scratch, src, count * sizeof(uint16_t));
    return scratch;
}

//only the skins FORCESKIN leaves in are part of the build (a 1 bpp buffer forces the black & white one)
#if !CARDIMAGES && SKINBUILT(skinBlackWhite)
#include "images/blackwhite/blocktiles_RGB565_LE.h"
#include "images/blackwhite/selectortiles_RGB565_LE.h"
#include "images/blackwhite/congratsscreen_RLE565.h"
#include "images/blackwhite/congratstiles_RGB565_LE.h"
#include "images/blackwhite/titlescreen_RLE565.h"
#endif

#if !CARDIMAGES && SKINBUILT(0)
#include "images/default/blocktiles_RGB565_LE.h"
#include "images/default/congratsscreen_RLE565.h"
#include "images/default/congratstiles_RGB565_LE.h"
#include "images/default/titlescreen_RLE565.h"
#endif

#if !CARDIMAGES && SKINBUILT(2)
#include "images/viaduct/blocktiles_RGB565_LE.h"
#include "images/viaduct/congratsscreen_RLE565.h"
#include "images/viaduct/congratstiles_RGB565_LE.h"
#include "images/viaduct/titlescreen_RLE565.h"
#endif

#if !CARDIMAGES && SKINBUILT(3)
#include "images/sonic/blocktiles_RGB565_LE.h"
#include "images/sonic/congratsscreen_RLE565.h"
#include "images/sonic/congratstiles_RGB565_LE.h"
#include "images/sonic/titlescreen_RLE565.h"


#endif


const uint8_t* currentTiles;
//Where the sheet in use keeps its eight row tiles, and the first tile it holds. The block tiles are
//128 twelve row tiles followed by 128 of eight rows, so they start at tile 0 and their small half
//begins after the big one. The congratulations screen's sheet holds only the letters it prints, as
//eight row tiles, so it starts at tile 64 and has no twelve row half at all
int currentFirstTile = 0;
int currentSmallRow = BLOCKTILES_SMALL_ROW;
//1 while the skin in use keeps its pictures one bit a pixel, which only the black & white
//one does. The selector tiles are the default skin's whatever skin is running, so they are
//always RGB565 and are drawn as such, see drawTile
bool skinImagesOneBit = false;

//the skin in use: the one FORCESKIN builds in (a 1 bpp buffer forces the black & white one), or
//the one picked in the options
uint8_t currentSkin(void)
{
#if CARDIMAGES
    //every skin is on the card, so the one the options chose is the one shown
    return skinSaveState();
#elif FORCESKIN >= 0
    return FORCESKIN;
#else
    return skinSaveState();
#endif
}

void preloadImages(void)
{
    skinImagesOneBit = ONEBITIMAGES && (currentSkin() == skinBlackWhite);
#if CARDIMAGES
    //The pictures of the skin the options chose are read off the card.
    //The card holds them in the order tools/mkcard.py found the folders - default first, then
    //the rest by name - which is NOT the order this game numbers them in: its skin 2 is viaduct
    //and its 3 is sonic, where the card has those two the other way round. Taking one for the
    //other paired each one's pictures with the other's colours. Named here rather than assumed,
    //so a skin added to either side cannot quietly do it again
    static const uint8_t cardSkinOf[maxSkins] =
        { CARD_SKIN_DEFAULT, CARD_SKIN_BLACKWHITE, CARD_SKIN_VIADUCT, CARD_SKIN_SONIC };
    CardImages_UseSkin(cardSkinOf[currentSkin() % maxSkins]);
#endif
    switch(currentSkin())
    {
#if SKINBUILT(0)
        //default
        case 0:
            ColorWhite = SCREEN.color565(123,186,255);
	        ColorBlack = SCREEN.color565(0,65,132);
#if CARDIMAGES
            //the same five, from the card, see CardImages_UseSkin above
            blockTiles = CardImages_Get(CARD_IMG_BLOCKTILES);
            selectorTiles = CardImages_Get(CARD_IMG_SELECTORTILES);
            congratsScreenTiles = CardImages_Get(CARD_IMG_CONGRATSTILES);
            imgTitleScreen = CardImages_Get(CARD_IMG_TITLESCREEN);
            imgCongratsScreen = CardImages_Get(CARD_IMG_CONGRATSSCREEN);
#else
            blockTiles = default_blocktiles_data;
            selectorTiles = default_selectortiles_data;
            congratsScreenTiles = default_congratstiles_data;
            imgTitleScreen = default_titlescreen_rle;
            imgCongratsScreen = default_congratsscreen_rle;
#endif
            setBlockTilesAsBackground();
            break;
#endif
#if SKINBUILT(skinBlackWhite)
        //blackwhite
        case 1:
            ColorWhite = SCREEN.color565(255,255,255);
	        ColorBlack = SCREEN.color565(0,0,0);
#if CARDIMAGES
            //the same five, from the card, see CardImages_UseSkin above
            blockTiles = CardImages_Get(CARD_IMG_BLOCKTILES);
            selectorTiles = CardImages_Get(CARD_IMG_SELECTORTILES);
            congratsScreenTiles = CardImages_Get(CARD_IMG_CONGRATSTILES);
            imgTitleScreen = CardImages_Get(CARD_IMG_TITLESCREEN);
            imgCongratsScreen = CardImages_Get(CARD_IMG_CONGRATSSCREEN);
#else
            blockTiles = black_white_blocktiles_data;
            //its own selector tiles rather than the default skin's. Sharing them saved flash while
            //every skin was RGB565, but this skin's are one bit a pixel and come to 10672 bytes
            //against the 61440 of the default skin's, so borrowing them now costs 50 KB
            selectorTiles = black_white_selectortiles_data;
            congratsScreenTiles = black_white_congratstiles_data;
            imgTitleScreen = black_white_titlescreen_rle;
            imgCongratsScreen = black_white_congratsscreen_rle;
#endif
            setBlockTilesAsBackground();
            break;
#endif
#if SKINBUILT(2)
        //viaduct
        case 2:
            ColorWhite = SCREEN.color565(210,210,210);
	        ColorBlack = SCREEN.color565(125,125,125);
#if CARDIMAGES
            //the same five, from the card, see CardImages_UseSkin above
            blockTiles = CardImages_Get(CARD_IMG_BLOCKTILES);
            selectorTiles = CardImages_Get(CARD_IMG_SELECTORTILES);
            congratsScreenTiles = CardImages_Get(CARD_IMG_CONGRATSTILES);
            imgTitleScreen = CardImages_Get(CARD_IMG_TITLESCREEN);
            imgCongratsScreen = CardImages_Get(CARD_IMG_CONGRATSSCREEN);
#else
            blockTiles = viaduct_blocktiles_data;
            selectorTiles = default_selectortiles_data;
            congratsScreenTiles = viaduct_congratstiles_data;
            imgTitleScreen = viaduct_titlescreen_rle;
            imgCongratsScreen = viaduct_congratsscreen_rle;
#endif
            setBlockTilesAsBackground();
            break;
#endif
#if SKINBUILT(3)
        //sonic
        case 3:
            ColorWhite = SCREEN.color565(204,96,0);
	        ColorBlack = SCREEN.color565(119,53,0);
#if CARDIMAGES
            //the same five, from the card, see CardImages_UseSkin above
            blockTiles = CardImages_Get(CARD_IMG_BLOCKTILES);
            selectorTiles = CardImages_Get(CARD_IMG_SELECTORTILES);
            congratsScreenTiles = CardImages_Get(CARD_IMG_CONGRATSTILES);
            imgTitleScreen = CardImages_Get(CARD_IMG_TITLESCREEN);
            imgCongratsScreen = CardImages_Get(CARD_IMG_CONGRATSSCREEN);
#else
            blockTiles = sonic_blocktiles_data;
            selectorTiles = default_selectortiles_data;
            congratsScreenTiles = sonic_congratstiles_data;
            imgTitleScreen = sonic_titlescreen_rle;
            imgCongratsScreen = sonic_congratsscreen_rle;
#endif
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
                d[c] = ToBuffer332(color, (int16_t)(dx + c), (int16_t)dy);
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
#if BANDRENDER
    //into the strip being put together, when there is one. See bandrender.h
    if (BandRender_Drawing())
    {
        BandRender_Image(x, y, w, h, data, transparent);
        return;
    }
#endif
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
            const uint16_t* prow = ImageRow(data + c0 + r * w, line, cols);
            //true: the values are plain RGB565, the library puts them in display order
            SCREEN.writePixels(prow, cols, true);
        }
    }
    else
    {
        //every run of opaque pixels on a row goes out as one
        for (int r = r0; r < r1; r++)
        {
            //the visible part of the row in one copy out of flash. The runs are gathered at
            //the front of the same line, a run never gets ahead of the pixel being read
            const uint16_t* prow = ImageRow(data + c0 + r * w, line, cols);
            int runX = 0, runLen = 0;
            for (int c = 0; c <= cols; c++)
            {
                uint16_t color = TRANSPARENT_COLOR;
                if (c < cols)
                    color = prow[c];
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

//Draws the w x h tile that starts at row `row` of a tile sheet. A sheet is one picture as wide as
//a tile, so a tile is the rows from `row` on, and this takes that row rather than a pointer into
//the middle of the data: a sheet packed one bit a pixel cannot be indexed by the byte the way an
//RGB565 one can.
//
//Every sheet in use belongs to the skin that is running, so which of the two kinds it is follows
//from the skin. Every skin can be in the build here and picked in the options, so that is a
//question for run time and not for the build, see skinImagesOneBit
#if CARDIMAGES
//A picture from the card: the w by h part at sx,sy of it, at x,y on the screen.
//This game names a tile by the row of the sheet it starts at, which is what sy is here - a flash
//build reaches it by adding to the pointer, and there is no pointer to add to on a card
static void drawImageCardPart(int x, int y, int sx, int sy, int w, int h, const uint8_t* data,
                              bool transparent)
{
    if (!data || (w <= 0) || (h <= 0))
        return;
    //into the strip being put together, when there is one. See bandrender.h
    if (BandRender_Drawing())
    {
        BandRender_ImageCard(x, y, sx, sy, w, h, data, transparent);
        return;
    }
    const int c0 = (x < 0) ? -x : 0;
    const int c1 = (x + w > WINDOW_WIDTH) ? WINDOW_WIDTH - x : w;
    const int r0 = (y < 0) ? -y : 0;
    const int r1 = (y + h > WINDOW_HEIGHT) ? WINDOW_HEIGHT - y : h;
    if ((c0 >= c1) || (r0 >= r1))
        return;
    const int cols = c1 - c0;
    const int dx = x + c0;
    uint16_t row[WINDOW_WIDTH];
    //THE BUS RULE: where the card shares the display's bus, reading it takes the bus over, so a
    //row is fetched with nothing of the display's open and only then sent. See Platform_CardRead
    for (int r = r0; r < r1; r++)
    {
        if (!CardImages_Row(data, sx + c0, sy + r, cols, row))
            continue;
        int c = 0;
        while (c < cols)
        {
            if (transparent)
                while ((c < cols) && (row[c] == TRANSPARENT_COLOR))
                    c++;
            const int runX = c;
            while ((c < cols) && (!transparent || (row[c] != TRANSPARENT_COLOR)))
                c++;
            if (c == runX)
                continue;
            SCREEN.startWrite();
  #if LOVYANGFX
            SCREEN.setAddrWindow(dx + runX, y + r, c - runX, 1);
            //true: the values are plain RGB565, the library puts them in display order
            SCREEN.writePixels(row + runX, c - runX, true);
  #else
            GFX.pushImage(dx + runX, y + r, c - runX, 1, row + runX);
  #endif
            SCREEN.endWrite();
        }
    }
}
#endif

void drawTile(int x, int y, int w, int h, const uint8_t* sheet, int row, bool transparent)
{
    if (!sheet)
        return;
#if CARDIMAGES
    //the sheet is on the card, so the tile is named by the row it starts at and not by a pointer
    drawImageCardPart(x, y, 0, row, w, h, sheet, transparent);
    return;
#endif
#if ONEBITIMAGES
    if (skinImagesOneBit)
    {
        drawImageOneBitPart(x, y, 0, row, w, h, sheet, transparent);
        return;
    }
#endif
    //the sheet is tileSize wide, so the tile's pixels lie together from its first row on
    drawImage(x, y, w, h, (const uint16_t*)(sheet + (size_t)row * tileSize * sizeof(uint16_t)), transparent);
}

void set_bkg_tile_xy(int ax, int ay, int tile)
{
    drawTile(BIG_X_OFFSET + ax * tileSize, BIG_Y_OFFSET + ay * tileSize, tileSize, tileSize,
             currentTiles, (tile - currentFirstTile) * tileSize, false);
}

void set_bkg_tile_xy8x8(int ax, int ay, int tile)
{
    //the eight row tiles of this sheet, wherever it keeps them
    drawTile(ax * 8 + SMALL_X_OFFSET, 8 + ay * SMALL_Y_OFFSET, tileSize, 8,
             currentTiles, currentSmallRow + (tile - currentFirstTile) * 8, true);
}

void set_bkg_data(const uint8_t* tiles, int firstTile, int smallRow)
{
    currentTiles = tiles;
    currentFirstTile = firstTile;
    currentSmallRow = smallRow;
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
#if CARDIMAGES
    //nothing on the card is run length encoded, see tools/mkcard.py: the full screen pictures are
    //plain ones here and this is the plain draw
    drawImageCardPart(x, y, 0, 0, w, h, data, false);
    return;
#endif
#if ONEBITIMAGES
    if (skinImagesOneBit)
    {
        //the picture carries its own size and is drawn whole
        drawImageOneBitPart(x, y, 0, 0, w, h, data, false);
        return;
    }
#endif
#if BANDRENDER
    //into the strip being put together, when there is one. The title screen and the pictures the
    //intro scrolls come through here, and without this they went to the panel while the strips
    //were being sent: the screen flickered through the intro and on every menu change
    if (BandRender_Drawing())
    {
        BandRender_ImageRLE(x, y, w, h, data);
        return;
    }
#endif
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