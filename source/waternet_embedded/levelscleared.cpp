#include <stdint.h>

#include "helperfuncs.h"
#include "commonvars.h"
#include "printfuncs.h"
#include "sound.h"
#include "level.h"

void initLevelsCleared(void)
{
    set_bkg_data(congratsScreenTiles);
    GFX.fillRect(0,0,WINDOW_WIDTH, WINDOW_HEIGHT,ColorBlack);
    set_bkg_tiles(0, 0, congratsScreenWidth, congratsScreenHeight, imgCongratsScreen);
    SelectMusic(musAllLevelsClear);    
}

void levelsCleared(void) 
{
    if (gameState == gsInitLevelsCleared)
    {
        initLevelsCleared();
        gameState -= gsInitDiff;
    }
    
    if (difficulty == diffVeryEasy)
    {
        printCongratsScreen(0, 3, "VERY EASY LEVELS");
    }
    if (difficulty == diffEasy)
    {
        printCongratsScreen(3, 3, "EASY LEVELS");
    }
    if (difficulty == diffNormal)
    {
        printCongratsScreen(2, 3, "NORMAL LEVELS");
    }
    if (difficulty == diffHard)
    {
        printCongratsScreen(3, 3, "HARD LEVELS");
    }
    if (difficulty == diffVeryHard)
    {
        printCongratsScreen(0, 3, "VERY HARD LEVELS");
    }


     if (((currButtons & BUTTON_A) && (!(prevButtons & BUTTON_A))) ||
         ((currButtons & BUTTON_B) && (!(prevButtons & BUTTON_B))))
     {
         playMenuAcknowlege();
         titleStep = tsMainMenu;
         gameState = gsInitTitle;
     }
}