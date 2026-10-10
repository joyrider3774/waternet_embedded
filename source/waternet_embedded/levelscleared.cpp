#include <stdint.h>

#include "helperfuncs.h"
#include "commonvars.h"
#include "printfuncs.h"
#include "sound.h"
#include "level.h"
//the screen is painted a strip at a time where there is no buffer
#include "bandrender.h"

//only draws, so the band renderer can call it once per strip
static void drawLevelsClearedOnce(void)
{
    //the strip already starts as this colour, see BandRender_Begin
    if (!BandRender_Drawing())
        GFX.fillRect(0,0,WINDOW_WIDTH, WINDOW_HEIGHT,ColorBlack);
    set_bkg_tiles(0, 0, congratsScreenWidth, congratsScreenHeight, imgCongratsScreen);
}

void initLevelsCleared(void)
{
    //this sheet is only the letters the screen prints, tiles 64 to 90, and only as eight
//row tiles: it has no twelve row half and starts at tile 64
    set_bkg_data(congratsScreenTiles, 64, 0);
#if SCREENBUFFER == 0
    //a strip at a time, so the screen is not seen being painted. See bandrender.h
    if (BandRender_Begin(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT, ColorBlack))
    {
        while (BandRender_Next())
            drawLevelsClearedOnce();
    }
    else
#endif
        drawLevelsClearedOnce();
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
        printCongratsScreen(0, 2, "VERY EASY LEVELS");
    }
    if (difficulty == diffEasy)
    {
        printCongratsScreen(3, 2, "EASY LEVELS");
    }
    if (difficulty == diffNormal)
    {
        printCongratsScreen(2, 2, "NORMAL LEVELS");
    }
    if (difficulty == diffHard)
    {
        printCongratsScreen(3, 2, "HARD LEVELS");
    }
    if (difficulty == diffVeryHard)
    {
        printCongratsScreen(0, 2, "VERY HARD LEVELS");
    }


     if (((currButtons & BUTTON_A) && (!(prevButtons & BUTTON_A))) ||
         ((currButtons & BUTTON_B) && (!(prevButtons & BUTTON_B))))
     {
         playMenuAcknowlege();
         titleStep = tsMainMenu;
         gameState = gsInitTitle;
     }
}