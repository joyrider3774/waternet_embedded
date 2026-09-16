#include "commonvars.h"
#include <stdint.h>


int startPos, menuPos, maxLevel, selectedLevel, boardX, boardY, difficulty,
    gameState, boardWidth, boardHeight, boardSize, levelDone, titleStep, gameMode, posAdd,
    mainMenu, option, needRedraw, paused, levelDoneBit, fpsStep;

unsigned int framecount;    
int selectionX, selectionY, i16;
int moves;
unsigned int randomSeedGame;
unsigned char level[maxBoardSize];
uint8_t currButtons, prevButtons;
const uint8_t * blockTiles, * selectorTiles, * congratsScreenTiles, *imgTitleScreen, *imgCongratsScreen;
uint16_t ColorWhite, ColorBlack;
