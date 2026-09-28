#ifndef helperfuncs_h
#define helperfuncs_h

#include <stdint.h>
//for PLATFORM_FAST_CODE, which marks the calls that run once for every pixel
#include "Platform.h"

void setBlockTilesAsBackground(void);
void set_bkg_tile_xy(int ax, int ay, int tile);
void set_bkg_tile_xy8x8(int ax, int ay, int tile);
void set_bkg_data(const uint8_t* tiles);
void set_bkg_tiles(int ax, int ay, int w, int h, const uint8_t* tiles);
void setInverted(int inverted);
PLATFORM_FAST_CODE void pushImageTransparent(int x, int y, int w, int h, const uint16_t* data);
void preloadImages(void);
uint8_t currentSkin(void);
#endif