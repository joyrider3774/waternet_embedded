#include <stdint.h>

#include "commonvars.h"
#include "helperfuncs.h"
#include "sound.h"
#include "printfuncs.h"
#include "savestate.h"
#include "level.h"


void updateBackgroundTitleScreen(void)
{
    GFX.fillRect(0,0,WINDOW_WIDTH, WINDOW_HEIGHT,ColorBlack);
    set_bkg_tiles(0, 6, titleScreenWidth, titleScreenHeight, imgTitleScreen);

    if (titleStep == tsMainMenu)
    {
        printMessage(2, 4, "START", true);
        printMessage(2, 5, "HELP", true);
        printMessage(2, 6, "OPTIONS", true);
        printMessage(2, 7, "CREDITS", true);
    }
    else
    {
        if (titleStep == tsDifficulty)
        {
            printMessage(1, 4, "VERY EASY", true);
            printMessage(1, 5, "EASY", true);
            printMessage(1, 6, "NORMAL", true);
            printMessage(1, 7, "HARD", true);
            printMessage(1, 8, "VERY HARD", true);
            printMessage(1, 9, "RANDOM", true);
        }
        else
        {
            if (titleStep == tsGameMode)
            {
                printMessage(2, 4, "ROTATE", true);
                printMessage(2, 5, "SLIDE", true);
                printMessage(2, 6, "ROSLID", true);

            }
            else
            {
                if (titleStep == tsCredits)
                {
                    printMessage(3, 4*tileSize/8, "CREATED BY");
                    printMessage(2, 5*tileSize/8, "WILLEMS DAVY");
                    printMessage(2, 6*tileSize/8, "JOYRIDER3774");
                }
                else
                {
                    if(isSoundOn())
                    {
                        printMessage(1, 4, "SOUND ON", true);
                    }
                    else
                    {
                        printMessage(1, 4, "SOUND OFF", true);
                    }
                    
                    printMessage(1,5, "SKIN", true);
                    printNumber(7,5, currentSkin() + 1, 1, true);
                }
            }
        }
    }


    //set menu tile
    if (titleStep == tsMainMenu)
    {
        set_bkg_tile_xy(1, 4 + mainMenu, leftMenu); 
    }
    else
    {
        //set menu tile
        if (titleStep == tsGameMode)
        {
            set_bkg_tile_xy(1, 4 + gameMode, leftMenu); 
        }
        else
        {
            if(titleStep == tsDifficulty)
            {
                set_bkg_tile_xy(0, 4 + difficulty, leftMenu);
            }
            else
            {
                if(titleStep == tsOptions)
                {
                    set_bkg_tile_xy(0, 4 + option, leftMenu);
                }    
            }
        }    
    }
    needRedraw = 0;
}

void initTitleScreen(void)
{   
    setBlockTilesAsBackground();
    SelectMusic(musNoMusic);
    updateBackgroundTitleScreen();
    needRedraw = 1;
}

void doUp(void)
{
    if (titleStep == tsMainMenu)
    {
        playMenuSelectSound();
        needRedraw = 1;

        if (mainMenu > mmStartGame)
        {
            mainMenu--;
        }
        else
        {
            mainMenu = mmCount - 1;
        }
    }
    else
    {
        if (titleStep == tsGameMode)
        {
            playMenuSelectSound();
            needRedraw = 1;

            if (gameMode > gmRotate)
            {
                gameMode--;
            }
            else
            {
                gameMode = gmCount - 1;
            }
        }
        else
        {
            if (titleStep == tsDifficulty)
            {
                playMenuSelectSound();
                needRedraw = 1;
                if (difficulty > diffVeryEasy)
                {
                    difficulty--;
                }
                else
                {
                    difficulty = diffCount - 1;
                }
            }
            else
            {
                if (titleStep == tsOptions)
                {
                    playMenuSelectSound();
                    needRedraw = 1;
                    if (option > opSound)
                    {
                        option--;
                    }
                    else
                    {
                        option = opCount - 1;
                    }
                }
            }
        }
    }
}

void doDown(void)
{
    if (titleStep == tsMainMenu)
    {
        needRedraw = 1;
        playMenuSelectSound();
        if (mainMenu < mmCount - 1)
        {
            mainMenu++;
        }
        else
        {
            mainMenu = mmStartGame;
        }
    }
    else
    {
        if (titleStep == tsGameMode)
        {
            playMenuSelectSound();
            needRedraw = 1;
            if (gameMode < gmCount - 1)
            {
                gameMode++;
            }
            else
            {
                gameMode = gmRotate;
            }
        }
        else
        {
            if (titleStep == tsDifficulty)
            {
                playMenuSelectSound();
                needRedraw = 1;
                if (difficulty < diffCount - 1)
                {
                    difficulty++;
                    
                }
                else
                {
                    difficulty = diffVeryEasy;
                }
            }
            else
            {
                if (titleStep == tsOptions)
                {
                    playMenuSelectSound();
                    needRedraw = 1;
                    if (option < opCount - 1)
                    {
                        option++;
                    }
                    else
                    {
                        option = opSound;
                    }
                }
            }
        }
    }
}

void titleScreen(void)
{
    if (gameState == gsInitTitle)
    {
        initTitleScreen();
        gameState -= gsInitDiff;
    }

   if ((currButtons & BUTTON_UP) && (!(prevButtons & BUTTON_UP)))
    {
        doUp();
    }
    if ((currButtons & BUTTON_DOWN) && (!(prevButtons & BUTTON_DOWN)))
    {
        doDown();
    }
    if ((currButtons & BUTTON_B) && (!(prevButtons & BUTTON_B)))
    {
        if ((titleStep == tsOptions) ||
            (titleStep == tsCredits))
        {
            titleStep = tsMainMenu;
            playMenuBackSound();
            needRedraw = 1;
        }
        else
        {
            if (titleStep > tsMainMenu)
            {
                titleStep--;
                playMenuBackSound();
                needRedraw = 1;
            }
        }
    }
    if ((currButtons & BUTTON_A) && (!(prevButtons & BUTTON_A)))
    {
        playMenuAcknowlege();
        if (mainMenu == mmOptions)
        {
            if(titleStep != tsOptions)
            {
                titleStep = tsOptions;
                needRedraw = 1;
            }
            else
            {
                switch(option)
                {
                    case opSound:
					{
                        setSoundOn(!isSoundOn());
                        setSoundOnSaveState(isSoundOn());
                        setMusicOn(isSoundOn());
                        needRedraw = 1;
                        break;
					}
                    case opSkins:
                    {
#if FORCESKIN >= 0
                        //only one skin is built in, FORCESKIN (a 1 bpp buffer forces the black & white one)
                        playErrorSound();
                        break;
#endif
                        int skin = skinSaveState();
                        skin++;
                        if(skin >= maxSkins)
                            skin = 0;
                        setSkinSaveState(skin);
                        preloadImages();
                        needRedraw = 1;
                        break;
                    }
                }
            }
        }

        if (mainMenu == mmCredits)
        {
            if(titleStep != tsCredits)
            {
                titleStep = tsCredits;
                needRedraw = 1;
            }
            else
            {
                titleStep = tsMainMenu;
                needRedraw = 1;
            }
        }

        if(mainMenu == mmHelp)
        {
            if (titleStep < tsGameMode)
            {
                titleStep++;
                needRedraw = 1;
            }
            else
            {
                switch (gameMode)
                {
                    case gmRotate:
                        gameState = gsInitHelpRotate;
                        break;
                    case gmSlide:
                        gameState = gsInitHelpSlide;
                        break;
                    case gmRotateSlide:
                        gameState = gsInitHelpRotateSlide;
                        break; 
                }
            }
        }
        if(mainMenu == mmStartGame)
        {
            
            if (titleStep < tsDifficulty)
            {
                titleStep++;
                needRedraw = 1;
            }
            else
            {
                if (difficulty == diffRandom)
                {
                    selectedLevel = 1;
                }
                else
                {
                    selectedLevel = lastUnlockedLevel(gameMode, difficulty);
                }
                gameState = gsInitLevelSelect;
                    
                if (gameMode == gmRotate)
                {
                    posAdd = 0;
                }
                else
                {
                    posAdd = 1;
                }
                //set randomseet to systime here
                //it will be reused all the time
                //with the level generating
                //but not when going back from
                //level playing to level selector
                //when calling init level there
                randomSeedGame = Platform_RandomSeed() + framecount;
                initLevel(randomSeedGame);
            } 
        }  
    }
    if (needRedraw)
    {
        updateBackgroundTitleScreen();
    }
}
