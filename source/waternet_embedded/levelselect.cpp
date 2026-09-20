
#include "helperfuncs.h"
#include "commonvars.h"
#include "printfuncs.h"
#include "savestate.h"
#include "sound.h"
#include "level.h"

void updateBackgroundLevelSelect(void) 
{
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
    
    drawLevel();
    needRedraw = 0;
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
