#ifndef helperfuncs_h
#define helperfuncs_h

#include <stdint.h>
//for PLATFORM_FAST_CODE, which marks the calls that run once for every pixel
#include "Platform.h"

void setBlockTilesAsBackground(void);
void set_bkg_tile_xy(int ax, int ay, int tile);
void set_bkg_tile_xy8x8(int ax, int ay, int tile);
//Puts a tile sheet in use. firstTile is the lowest tile number it holds and smallRow where
//its eight row tiles begin, both of which differ from sheet to sheet: the block tiles hold
//every tile from 0 with the eight row ones after the twelve row ones, the congratulations
//screen's sheet holds only the letters it prints and only in eight rows
//the twelve row tiles of the block sheet, which the eight row ones follow
#define BLOCKTILES_SMALL_ROW (128 * tileSize)
void set_bkg_data(const uint8_t* tiles, int firstTile = 0, int smallRow = BLOCKTILES_SMALL_ROW);
void set_bkg_tiles(int ax, int ay, int w, int h, const uint8_t* tiles);
void setInverted(int inverted);
//Draws the w x h tile that starts at row `row` of a tile sheet. A sheet is one picture as wide
//as a tile, so the tile is the rows from `row` on. It is named by that row rather than by a
//pointer into the data: a sheet packed one bit a pixel cannot be indexed by the byte
PLATFORM_FAST_CODE void drawTile(int x, int y, int w, int h, const uint8_t* sheet, int row,
                                 bool transparent);
//1 while the skin in use keeps its pictures one bit a pixel
extern bool skinImagesOneBit;
void preloadImages(void);
uint8_t currentSkin(void);
#endif