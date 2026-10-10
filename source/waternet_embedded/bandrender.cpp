#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include "defines.h"
#include "commonvars.h"
#include "helperfuncs.h"
#include "bandrender.h"
#include "cardimages.h"
//the one bit pictures a flash build draws, which go into a strip as well now
#include "onebitimage.h"

#if BANDRENDER

//A strip is as tall as a tile, so a row of the board is one strip. The board does not start on a
//multiple of this, so a tile falls in two strips and is drawn into both, clipped: that is what
//the clipping below is for. Half a tile would only make it three
#define BANDHEIGHT tileSize

//the strip being put together, at most the whole width of the screen
static uint16_t* bandBuf = NULL;

//the rectangle being painted and how far down it the strips have come
static int16_t rectX, rectY, rectW, rectH, rectNextTop;
static uint16_t rectBackdrop = 0;
//where the strip that is open sits on the screen, and whether one is open at all
static int16_t stripX, stripY, stripW, stripH;
static bool stripOpen = false;

void BandRender_Init(void)
{
	stripOpen = false;
	if (bandBuf)
		return;
	bandBuf = (uint16_t*)malloc((size_t)WINDOW_WIDTH * BANDHEIGHT * sizeof(uint16_t));
	if (!bandBuf)
		Platform_Log("no room for the strip buffer, the screen is painted the plain way\n");
}

void BandRender_Deinit(void)
{
	free(bandBuf);
	bandBuf = NULL;
	stripOpen = false;
}

bool BandRender_Ready(void)
{
	return bandBuf != NULL;
}

int16_t BandRender_StripY(void) { return stripY; }
int16_t BandRender_StripH(void) { return stripH; }
bool BandRender_Drawing(void) { return stripOpen; }

bool BandRender_Begin(int16_t x, int16_t y, int16_t w, int16_t h, uint16_t backdrop)
{
	if (!bandBuf)
		return false;
	//There is one strip buffer and one set of strip bounds, so a pass started inside another
	//would take them over and lose the one around it. The caller then paints the plain way,
	//which is correct and merely visible, rather than painting the wrong thing
	if (stripOpen)
		return false;
	if (x < 0) { w = (int16_t)(w + x); x = 0; }
	if (y < 0) { h = (int16_t)(h + y); y = 0; }
	if (x + w > WINDOW_WIDTH) w = (int16_t)(WINDOW_WIDTH - x);
	if (y + h > WINDOW_HEIGHT) h = (int16_t)(WINDOW_HEIGHT - y);
	if ((w <= 0) || (h <= 0))
		return false;
	rectX = x;
	rectY = y;
	rectW = w;
	rectH = h;
	rectNextTop = y;
	rectBackdrop = backdrop;
	stripOpen = false;
	return true;
}

//sends the strip that is open. One transaction and one window per strip: a window that outlives
//its transaction is not something every display class keeps
static void SendStrip(void)
{
	SCREEN.startWrite();
	SCREEN.setAddrWindow(stripX, stripY, stripW, stripH);
#if LOVYANGFX
	//true: the strip holds plain RGB565, the library puts it in display order
	SCREEN.writePixels(bandBuf, (int32_t)stripW * stripH, true);
#else
	SCREEN.pushPixels(bandBuf, stripW * stripH);
#endif
	SCREEN.endWrite();
}

bool BandRender_Next(void)
{
	//what was drawn into the strip before this one goes out now
	if (stripOpen)
	{
		stripOpen = false;
		SendStrip();
	}
	if (rectNextTop >= rectY + rectH)
		return false;
	stripX = rectX;
	stripW = rectW;
	stripY = rectNextTop;
	stripH = (int16_t)((rectNextTop + BANDHEIGHT <= rectY + rectH) ? BANDHEIGHT
	                                                              : (rectY + rectH - rectNextTop));
	rectNextTop = (int16_t)(rectNextTop + BANDHEIGHT);
	//the strip starts as the plain colour the board is drawn over
	const uint16_t count = (uint16_t)(stripW * stripH);
	for (uint16_t i = 0; i < count; i++)
		bandBuf[i] = rectBackdrop;
	stripOpen = true;
	return true;
}

#if CARDIMAGES
void BandRender_ImageCard(int x, int y, int sx, int sy, int w, int h, const uint8_t* data,
                          bool transparent)
{
	if (!data || (w <= 0) || (h <= 0))
		return;
	const int ox = x - sx, oy = y - sy;
	int c0 = sx, c1 = sx + w, r0 = sy, r1 = sy + h;
	if (c0 < stripX - ox) c0 = stripX - ox;
	if (r0 < stripY - oy) r0 = stripY - oy;
	if (c1 > stripX + stripW - ox) c1 = stripX + stripW - ox;
	if (r1 > stripY + stripH - oy) r1 = stripY + stripH - oy;
	if ((c0 >= c1) || (r0 >= r1))
		return;
	const int cols = c1 - c0;
	//A picture small enough to be kept whole sits in RAM, and its rows are copied out of it
	//rather than asked for one at a time: that is the same copy a flash build makes, and for
	//a sheet drawn hundreds of times a frame it is most of what a strip costs
	const uint8_t* px = CardImages_Cached(data);
	const int pitch = px ? (int)CardImages_Width(data) : 0;
	for (int r = r0; r < r1; r++)
	{
		uint16_t* dst = &bandBuf[(oy + r - stripY) * stripW + (ox + c0 - stripX)];
		uint16_t scratch[WINDOW_WIDTH];
		const uint16_t* src;
		if (px)
			src = (const uint16_t*)(px + ((size_t)r * pitch + c0) * sizeof(uint16_t));
		else if (CardImages_Row(data, c0, r, cols, scratch))
			src = scratch;
		else
		{
			//the row did not come, so nothing of it is drawn
			continue;
		}
		if (!transparent)
		{
			//nothing to leave out, so the row lands in the strip where it belongs
			memcpy(dst, src, (size_t)cols * sizeof(uint16_t));
			continue;
		}
		//the transparent pixels keep what the strip already holds
		for (int c = 0; c < cols; c++)
			if (src[c] != TRANSPARENT_COLOR)
				dst[c] = src[c];
	}
}
#endif

//A row of a picture in flash, the same copy the display path makes: where flash is plain memory
//an evenly placed row is used where it lies, otherwise it is copied into the scratch row first.
//A 16 bit read needs an even address, a core like the Cortex-M0+ faults on an odd one. The same
//as helperfuncs.cpp's ImageRow, which is private to that file
static inline const uint16_t* FlashRow(const void* src, uint16_t* scratch, int count)
{
#if PLATFORM_DIRECT_FLASH
	if (((uintptr_t)src & 1) == 0)
		return (const uint16_t*)src;
#endif
	PLATFORM_READ_BYTES((uint8_t*)scratch, src, (size_t)count * sizeof(uint16_t));
	return scratch;
}

//A picture out of flash into the strip. The picture is w pixels a row and the whole of it is
//drawn, which is what drawImage takes: this game names a tile by the row of its sheet and does
//the picking before it gets here
void BandRender_Image(int x, int y, int w, int h, const uint16_t* data, bool transparent)
{
	if (!data || (w <= 0) || (h <= 0))
		return;
	//the columns and rows of the picture that land in the strip that is open
	int c0 = 0, c1 = w, r0 = 0, r1 = h;
	if (c0 < stripX - x) c0 = stripX - x;
	if (r0 < stripY - y) r0 = stripY - y;
	if (c1 > stripX + stripW - x) c1 = stripX + stripW - x;
	if (r1 > stripY + stripH - y) r1 = stripY + stripH - y;
	if ((c0 >= c1) || (r0 >= r1))
		return;
	const int cols = c1 - c0;
	uint16_t scratch[WINDOW_WIDTH];
	for (int r = r0; r < r1; r++)
	{
		uint16_t* dst = &bandBuf[(y + r - stripY) * stripW + (x + c0 - stripX)];
		const uint16_t* src = FlashRow(data + c0 + (size_t)r * w, scratch, cols);
		if (!transparent)
		{
			memcpy(dst, src, (size_t)cols * sizeof(uint16_t));
			continue;
		}
		//the transparent pixels keep what the strip already holds
		for (int c = 0; c < cols; c++)
			if (src[c] != TRANSPARENT_COLOR)
				dst[c] = src[c];
	}
}

#if ONEBITIMAGES
//The same for a picture kept one bit a pixel. It carries its own size and is read a row at a
//time through the readers in onebitimage.h, which is the only way to reach row r of a plane
//that may be packed; the rows before the first one wanted are skipped rather than drawn
PLATFORM_HOT_CODE void BandRender_ImageOneBit(int x, int y, int sx, int sy, int w, int h,
                                              const uint8_t* data, bool transparent)
{
	if (!data || (w <= 0) || (h <= 0))
		return;
	const int dataWidth = OneBitWidth(data);
	const int dataHeight = OneBitHeight(data);
	const int maskAt = OneBitMaskAt(data);
	const bool useMask = transparent && (maskAt != 0);
	//how each plane is packed, which the readers work out from it, see tools/onebit.py
	const int flags = OneBitFlags(data);

	const int ox = x - sx, oy = y - sy;
	int c0 = sx, c1 = sx + w, r0 = sy, r1 = sy + h;
	if (c0 < 0) c0 = 0;
	if (r0 < 0) r0 = 0;
	if (c1 > dataWidth) c1 = dataWidth;
	if (r1 > dataHeight) r1 = dataHeight;
	//and then to the strip, which is the only part of the screen this writes to
	if (c0 < stripX - ox) c0 = stripX - ox;
	if (r0 < stripY - oy) r0 = stripY - oy;
	if (c1 > stripX + stripW - ox) c1 = stripX + stripW - ox;
	if (r1 > stripY + stripH - oy) r1 = stripY + stripH - oy;
	if ((c0 >= c1) || (r0 >= r1))
		return;

	const int stride = (dataWidth + 7) / 8;
	uint8_t rowPixels[ONEBIT_MAX_STRIDE];
	uint8_t rowMask[ONEBIT_MAX_STRIDE];
	OneBitReader pixels;
	OneBitReaderInit(&pixels, data + ONEBIT_HEADER, flags, false);
	OneBitReaderSkip(&pixels, r0, stride, rowPixels);
	OneBitReader mask;
	if (useMask)
	{
		OneBitReaderInit(&mask, data + maskAt, flags, true);
		OneBitReaderSkip(&mask, r0, stride, rowMask);
	}
	for (int r = r0; r < r1; r++)
	{
		OneBitReaderRow(&pixels, rowPixels, stride);
		if (useMask)
			OneBitReaderRow(&mask, rowMask, stride);
		uint16_t* dst = &bandBuf[(oy + r - stripY) * stripW + (ox + c0 - stripX)];
		for (int c = c0; c < c1; c++, dst++)
		{
			//what the mask clears keeps what the strip already holds
			if (useMask && !OneBitAt(rowMask, c))
				continue;
			*dst = OneBitAt(rowPixels, c) ? ONEBIT_SET : ONEBIT_CLEAR;
		}
	}
}
#endif

//A run length encoded picture into the strip, the same stream pushImageRLE decodes: a control
//byte with the top bit set is a run of (c & 0x7F) + 1 of the pixel after it, otherwise c + 1
//pixels follow. The stream can only be read from its start, so it is walked from there and the
//rows above the strip cost a step of the decoder rather than a write, and decoding stops at the
//row below it. The picture is drawn whole and opaque, which is all this game asks of it
void BandRender_ImageRLE(int x, int y, int w, int h, const uint8_t* data)
{
	if (!data || (w <= 0) || (h <= 0))
		return;
	//the columns and rows of the picture that land in the strip that is open
	int c0 = 0, c1 = w, r0 = 0, r1 = h;
	if (c0 < stripX - x) c0 = stripX - x;
	if (r0 < stripY - y) r0 = stripY - y;
	if (c1 > stripX + stripW - x) c1 = stripX + stripW - x;
	if (r1 > stripY + stripH - y) r1 = stripY + stripH - y;
	if ((c0 >= c1) || (r0 >= r1))
		return;
	//a control covers at most 128 pixels
	uint16_t pixels[128];
	uint32_t left = (uint32_t)w * h;
	int cx = 0, cy = 0;
	while ((left > 0) && (cy < r1))
	{
		uint8_t control = PLATFORM_READ_BYTE(data++);
		uint16_t count = (uint16_t)((control & 0x7F) + 1);
		if (count > left)
			count = (uint16_t)left;
		const bool run = (control & 0x80) != 0;
		uint16_t color = 0;
		if (run)
		{
			color = (uint16_t)(PLATFORM_READ_BYTE(data) | (PLATFORM_READ_BYTE(data + 1) << 8));
			data += 2;
		}
		else
		{
			//the pixels of a control that reaches the strip are read, one that lies above it
			//only moves the stream on
			if ((cy + (int)((cx + count - 1) / w)) >= r0)
				PLATFORM_READ_BYTES((uint8_t*)pixels, data, count * sizeof(uint16_t));
			data += count * sizeof(uint16_t);
		}
		left -= count;
		//the pixels of the control a row at a time, only the part in the strip is written
		for (uint16_t done = 0; done < count; )
		{
			int n = w - cx;
			if (n > (int)(count - done))
				n = (int)(count - done);
			if ((cy >= r0) && (cy < r1))
			{
				const int a = (cx > c0) ? cx : c0;
				const int e = (cx + n < c1) ? cx + n : c1;
				if (a < e)
				{
					uint16_t* dst = &bandBuf[(y + cy - stripY) * stripW + (x + a - stripX)];
					if (run)
					{
						for (int c = a; c < e; c++)
							*dst++ = color;
					}
					else
					{
						memcpy(dst, &pixels[done + (a - cx)], (size_t)(e - a) * sizeof(uint16_t));
					}
				}
			}
			done = (uint16_t)(done + n);
			cx += n;
			if (cx == w)
			{
				cx = 0;
				cy++;
			}
		}
	}
}

#else

//nothing is painted through a strip in this build, see bandrender.h
void BandRender_Init(void) {}
void BandRender_Deinit(void) {}
bool BandRender_Ready(void) { return false; }
bool BandRender_Begin(int16_t x, int16_t y, int16_t w, int16_t h, uint16_t backdrop)
{
	(void)x; (void)y; (void)w; (void)h; (void)backdrop;
	return false;
}
bool BandRender_Next(void) { return false; }
int16_t BandRender_StripY(void) { return 0; }
int16_t BandRender_StripH(void) { return 0; }
bool BandRender_Drawing(void) { return false; }
#if CARDIMAGES
void BandRender_ImageCard(int x, int y, int sx, int sy, int w, int h, const uint8_t* data,
                          bool transparent)
{
	(void)x; (void)y; (void)sx; (void)sy; (void)w; (void)h; (void)data; (void)transparent;
}
#endif
void BandRender_Image(int x, int y, int w, int h, const uint16_t* data, bool transparent)
{
	(void)x; (void)y; (void)w; (void)h; (void)data; (void)transparent;
}
#if ONEBITIMAGES
void BandRender_ImageOneBit(int x, int y, int sx, int sy, int w, int h, const uint8_t* data,
                            bool transparent)
{
	(void)x; (void)y; (void)sx; (void)sy; (void)w; (void)h; (void)data; (void)transparent;
}
#endif
void BandRender_ImageRLE(int x, int y, int w, int h, const uint8_t* data)
{
	(void)x; (void)y; (void)w; (void)h; (void)data;
}

#endif
