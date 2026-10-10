
#include "helperfuncs.h"
#include "commonvars.h"
#include "printfuncs.h"
#include "savestate.h"
#include "sound.h"
#include "level.h"
//the screen is painted a strip at a time where there is no buffer
#include "bandrender.h"

//Painted a strip at a time where there is no screen buffer, so the screen is never seen being
//put together. Only draws, so the band renderer can call it once per strip; see bandrender.h
static void updateBackgroundLevelSelectOnce(void);

void updateBackgroundLevelSelect(void)
{
    if (BandRender_Begin(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT, ColorBlack))
    {
        while (BandRender_Next())
            updateBackgroundLevelSelectOnce();
    }
    else
        updateBackgroundLevelSelectOnce();

    needRedraw = 0;
}

static void updateBackgroundLevelSelectOnce(void)
{
    //the strip already starts as this colour, see BandRender_Begin
    if (!BandRender_Drawing())
        GFX.fillRect(0,0,WINDOW_WIDTH, WINDOW_HEIGHT,ColorBlack);
    //LEVEL:
    printMessage(0, (maxBoardBgHeight*tileSize/8) +1, "LEVEL:");
    
    //[LEVEL NR] 2 chars
    printNumber(6, (maxBoardBgHeight*tileSize/8) +1, selectedLevel, 2);
    
    //B:BACK
    printMessage(10, (maxBoardBgHeight*tileSize/8) +1, "b:BACK");
    
    //A:PLAY
    printMessage(10, (maxBoardBgHeight*tileSize/8)+2, "a:PLAY");
    
    //Locked & Unlocked keywoard
    int tmpUnlocked = levelUnlocked(gameMode, difficulty, selectedLevel -1);
    if(!tmpUnlocked)
    {
        printMessage(0, (maxBoardBgHeight*tileSize/8)+2, "LOCKED");
    }
    else
    {
        if(tmpUnlocked)
        {
            printMessage(0, (maxBoardBgHeight*tileSize/8)+2, "OPEN");
        }
    }

    //The board, in the same pass as the text around it. Drawing it afterwards in a pass of its
    //own would mean this pass painting the plain backdrop over the board first and the board
    //going back on top of it after, which is the board being seen filling itself in. drawLevel
    //notices the pass that is open and draws into the strip, see level.cpp
    drawLevel();
}

void initLevelSelect(void)
{
    setBlockTilesAsBackground();
    SelectMusic(musNoMusic);
    updateBackgroundLevelSelect();
    needRedraw = 1;
}

void doLeft(void)
{
    if (difficulty == diffRandom)
    {
        playMenuSelectSound();
        randomSeedGame = Platform_RandomSeed() + framecount;
        initLevel(randomSeedGame);
        needRedraw = 1;
    }
    else
    {
        if (selectedLevel > 1)
        {
            playMenuSelectSound();
            selectedLevel--;
            initLevel(randomSeedGame);
            needRedraw = 1;
        }
    }
}

void doRight(void)
{
    if (difficulty == diffRandom)
    {
        playMenuSelectSound();
        //need new seed based on time
        randomSeedGame = Platform_RandomSeed() + framecount;
        initLevel(randomSeedGame);
        needRedraw = 1;
    }
    else
    {
        if (selectedLevel < maxLevel)
        {
            playMenuSelectSound();
            selectedLevel++;
            initLevel(randomSeedGame);
            needRedraw = 1;
        }
    }
}

void levelSelect(void)
{
    int tmpUnlocked;
    tmpUnlocked = levelUnlocked(gameMode, difficulty, selectedLevel -1);
    
    if (gameState == gsInitLevelSelect)
    {
        initLevelSelect();
        gameState -= gsInitDiff;
    }
       
    if ((currButtons & BUTTON_B) && (!(prevButtons & BUTTON_B)))
    {
        playMenuBackSound();
        gameState = gsInitTitle;
    }
    if ((currButtons & BUTTON_A) && (!(prevButtons & BUTTON_A)))
    {
        if(tmpUnlocked)
        {
            gameState = gsInitGame;
            playMenuAcknowlege();
        }
        else
        {
            playErrorSound();
        }
    }
    if ((currButtons & BUTTON_LEFT) && (!(prevButtons & BUTTON_LEFT)))
    {
        doLeft();
    }
    if ((currButtons & BUTTON_RIGHT) && (!(prevButtons & BUTTON_RIGHT)))
    {
        doRight();
    }
    
    if (needRedraw)
    {
        updateBackgroundLevelSelect();
    }
}
