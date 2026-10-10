#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include "defines.h"
#include "commonvars.h"
#include "helperfuncs.h"
#include "bandrender.h"
#include "cardimages.h"

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

#endif
