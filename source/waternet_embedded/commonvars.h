#ifndef commonvars_h
#define commonvars_h

#include <stdint.h>
#include "defines.h"
#include "Platform.h"

extern uint16_t ColorWhite, ColorBlack;
extern int startPos, menuPos, maxLevel, selectedLevel, boardX, boardY, difficulty,
	gameState, boardWidth, boardHeight, boardSize, levelDone, titleStep, gameMode, posAdd,
	mainMenu, option, needRedraw, paused, levelDoneBit, fpsStep;

extern unsigned int framecount;
extern int selectionX, selectionY;
extern int moves;
extern unsigned int randomSeedGame;
extern unsigned char level[maxBoardSize];
extern uint8_t currButtons, prevButtons;
extern const uint8_t* blockTiles, * selectorTiles, * congratsScreenTiles, *imgTitleScreen, *imgCongratsScreen;;


#endif