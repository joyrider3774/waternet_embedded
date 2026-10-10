#ifndef BANDRENDER_H
#define BANDRENDER_H

#include <stdint.h>
#include "defines.h"

//Paints part of the screen in one pass, for a build that draws straight to the panel.
//
//This game draws into a 1 bpp buffer instead where it can (SCREENBUFFER 1 in PlatformCHGame.h),
//not for speed but so that nothing half drawn is ever shown: the whole frame is put together in
//ram and sent in one piece. That buffer holds two colours, which suits the black & white skin it
//forces and is no use at all for art read from a card, where every skin is full RGB565.
//
//A strip does the same job in colour. The screen is painted a strip at a time, each one put
//together in ram and sent whole, so a repaint is never seen happening - and a strip is
//WINDOW_WIDTH by BANDHEIGHT of RGB565 rather than a whole frame, which is what makes it fit.
//
//A screen paints itself like this:
//
//    if (BandRender_Begin(x, y, w, h, backdrop))
//        while (BandRender_Next())
//            ...the ordinary drawing calls, in the order they should be painted...
//    else
//        ...the ordinary drawing calls, straight to the display...
//
//The drawing between one Next and the next goes into the strip rather than to the display, so the
//game's own drawing code is used unchanged: drawLevel is called once per strip and draws the
//whole board each time, and each strip keeps only the part of it that lands there. That works
//because drawLevel only draws - it reads the game's state and changes none of it.
//
//Only built where the drawing really reaches the display as it happens. With a screen buffer the
//frame is already put together away from the panel and the strips would only cost another pass,
//and so is a frame drawn into a page the display is not showing (PLATFORM_OFFSCREEN_DRAW), which
//is how the GBA, the DS, the PlayStation and the N64 work.
//
//That leaves the CHGame and the Gamebuino META, the two that write to the panel as they draw.
//It used to be a card build alone, because only the card drawing was routed into a strip and a
//build whose pictures come out of flash would have drawn them to the display and then had the
//strips go out over them. Those two paths go into the strip as well now, see BandRender_Image
//and BandRender_ImageOneBit, so the gate is about the device and no longer about where the art is.
//
//Everything here still stands where it is not built: Begin gives false and Drawing gives false,
//so a screen that paints itself the way above paints itself the plain way instead.
#if (SCREENBUFFER == 0) && !PLATFORM_OFFSCREEN_DRAW
#define BANDRENDER 1
#else
#define BANDRENDER 0
#endif

//The strip buffer, taken when the game starts and given back when it ends. Without it Begin
//gives false and the screen paints the plain way, which is correct and merely visible
void BandRender_Init(void);
void BandRender_Deinit(void);
bool BandRender_Ready(void);

//Starts painting the rectangle. backdrop is what each strip starts as: this game has no full
//screen background picture to read one from, its board is tiles drawn over a plain colour
bool BandRender_Begin(int16_t x, int16_t y, int16_t w, int16_t h, uint16_t backdrop);
//sends the strip that was drawn into and opens the next one, false when the rectangle is done
bool BandRender_Next(void);

//where the strip that is open sits, for drawing that can leave out what does not reach it
int16_t BandRender_StripY(void);
int16_t BandRender_StripH(void);

//true while a strip is open. helperfuncs.cpp asks this and sends its drawing here
bool BandRender_Drawing(void);
#if CARDIMAGES
//the w by h part at sx,sy of a picture on the card, at x,y, see cardimages.h. sy is the row of a
//sheet a tile starts at, which is how this game names one
void BandRender_ImageCard(int x, int y, int sx, int sy, int w, int h, const uint8_t* data,
                          bool transparent);
#endif
//A w by h picture out of flash at x,y, the pixels laid out as w of them a row. The same picture
//and the same arguments the display path takes, so that drawImage can hand its work straight over
void BandRender_Image(int x, int y, int w, int h, const uint16_t* data, bool transparent);
#if ONEBITIMAGES
//the same for a picture kept one bit a pixel, which carries its own size, see onebitimage.h
void BandRender_ImageOneBit(int x, int y, int sx, int sy, int w, int h, const uint8_t* data,
                            bool transparent);
#endif
//And for a run length encoded one, the format tools/png2rle565.py writes: the whole w by h
//picture at x,y, which is what pushImageRLE draws. The title screen and the pictures the intro
//scrolls are these
void BandRender_ImageRLE(int x, int y, int w, int h, const uint8_t* data);

#endif
