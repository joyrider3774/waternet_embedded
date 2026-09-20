

#include "level.h"
#include "commonvars.h"
#include "helperfuncs.h"
#include "sound.h"
#include "printfuncs.h"
#include "cursor.h"
#include "savestate.h"

void updateBackgroundGame(void)
{
    //background
    GFX.fillRect(0,0,WINDOW_WIDTH, WINDOW_HEIGHT,ColorBlack);
        
    //LEVEL:
    printMessage(0, (maxBoardBgHeight*tileSize/8) +1, "LEVEL:");
        
    //[LEVEL NR] 2 chars
    printNumber(6, (maxBoardBgHeight*tileSize/8) + 1, selectedLevel, 2);
        
        
    //MOVES:
    printMessage(0, (maxBoardBgHeight*tileSize/8) + 2, "MOVES:");

    printNumber(6, (maxBoardBgHeight*tileSize/8) + 2, moves, 3); 
        
    //A:XXXXXX (XXXXXX="ROTATE" or XXXXXX="SLIDE " or XXXXXX="ROSLID")
    if(gameMode == gmRotate)
    {
        printMessage(10, (maxBoardBgHeight*tileSize/8) + 2, "a:ROTATE");
    }
    else
    {
        if(gameMode == gmSlide)
        {
            printMessage(10, (maxBoardBgHeight*tileSize/8) + 2, "a:SLIDE");
        }
        else
        {
            if(gameMode == gmRotateSlide)
            {
                printMessage(10, (maxBoardBgHeight*tileSize/8) + 2, "a:ROSLID");
            }
        }
    }

    //B:BACK
    printMessage(10, (maxBoardBgHeight*tileSize/8) + 1, "b:BACK");

    drawLevel();
        
    if (paused)
    {
        printMessage(0, ((maxBoardBgHeight*tileSize/8) >> 1) - 3, "[**************]");
        printMessage(0, ((maxBoardBgHeight*tileSize/8) >> 1) - 2, "|PLEASE CONFIRM+");
        printMessage(0, ((maxBoardBgHeight*tileSize/8) >> 1) - 1, "|              +");
        printMessage(0, ((maxBoardBgHeight*tileSize/8) >> 1) + 0, "|   a PLAY     +");
        printMessage(0, ((maxBoardBgHeight*tileSize/8) >> 1) + 1, "|   b TO QUIT  +");
        printMessage(0, ((maxBoardBgHeight*tileSize/8) >> 1) + 2, "<##############>");
    }

    if (levelDoneBit && (fpsStep == 0))
    {
        printMessage(((16 - 13) >> 1), ((maxBoardBgHeight*tileSize/8) >> 1) - 3, "[************]");
        printMessage(((16 - 13) >> 1), ((maxBoardBgHeight*tileSize/8) >> 1) - 2, "| LEVEL DONE +");
        printMessage(((16 - 13) >> 1), ((maxBoardBgHeight*tileSize/8) >> 1) - 1, "|            +");
        printMessage(((16 - 13) >> 1), ((maxBoardBgHeight*tileSize/8) >> 1) + 0, "| a CONTINUE +");
        printMessage(((16 - 13) >> 1), ((maxBoardBgHeight*tileSize/8) >> 1) + 1, "| b SOLUTION +");
        printMessage(((16 - 13) >> 1), ((maxBoardBgHeight*tileSize/8) >> 1) + 2, "<############>");        
    }

    if ((!paused) && (!levelDoneBit))
        drawCursors(true);
    
    needRedraw = 0;
}

void initGame(void)
{
    SelectMusic(musNoMusic);
    //set background tiles
    setBlockTilesAsBackground();
    //set sprite for selector / cursor
    initCursors();
    setCursorPos(0, boardX + selectionX, boardY + selectionY);
    showCursors();
    levelDoneBit = 0;
    paused = 0;
    fpsStep = 0;
    updateBackgroundGame();
}

void doPause(void)
{
    paused = 1;
    setMusicOn(0);
    setSoundOn(0);
    hideCursors();
    needRedraw = 1;
}

void doUnPause(void)
{
    paused = 0;
    //music could have been disabled by menu options during pause screen so we use the savestate value
    setMusicOn(isSoundOnSaveState());
    setSoundOn(isSoundOnSaveState());
    setCursorPos(0, boardX + selectionX, boardY + selectionY);
    showCursors();
    playMenuAcknowlege();
    needRedraw = 1;
}

void game(void)
{
    needRedraw |= updateCursorFrame();
    if (gameState == gsInitGame)
    {
        initGame();
        gameState -= gsInitDiff;
    }
    
    if (fpsStep > 0)
    {
        fpsStep--;
        if (fpsStep == 0)
        {
            needRedraw = 1;
        }
    }
         
    if ((currButtons & BUTTON_DOWN) && (!(prevButtons & BUTTON_DOWN)))
    {
        if(!levelDone && !paused)
        {
            playGameMoveSound();
            //if not touching border on bottom
            if (selectionY + 1 < boardHeight + posAdd)
            {
                selectionY += 1;
                needRedraw = 1;
            }
            else
            //set to border on top
            {
                selectionY = -posAdd;
                needRedraw = 1;
            }
            setCursorPos(0, boardX + selectionX, boardY + selectionY);
        }
    } 
    if ((currButtons & BUTTON_UP) && (!(prevButtons & BUTTON_UP)))
    {
        if (!levelDone && !paused)
        {
            //if not touching border on top
            playGameMoveSound();
            if (selectionY -1 >= -posAdd)
            {
                selectionY -= 1;
                needRedraw = 1;
            }
            else
            //set to border on bottom
            {
                selectionY = boardHeight -1 +posAdd;
                needRedraw = 1;
            }
            setCursorPos(0, boardX + selectionX, boardY + selectionY);
        }
    }
    if ((currButtons & BUTTON_RIGHT) && (!(prevButtons & BUTTON_RIGHT)))
    {
        if (!levelDone && !paused)
        {
            playGameMoveSound();
            //if not touching border on right
            if(selectionX + 1 < boardWidth + posAdd)
            {
                selectionX += 1;
                needRedraw = 1;
            }
            else
            //set to border on left
            {
                selectionX = -posAdd;
                needRedraw = 1;
            }
            setCursorPos(0, boardX + selectionX, boardY + selectionY);
        }
    }

    if ((currButtons & BUTTON_LEFT) && (!(prevButtons & BUTTON_LEFT)))
    {
        if(!levelDone && !paused)
        {
            playGameMoveSound();
            //if not touching border on left
            if( selectionX -1 >= -posAdd)
            {
                selectionX -= 1;
                needRedraw = 1;
            }
            //set to border on right
            else
            {
                selectionX = boardWidth -1 + posAdd;
                needRedraw = 1;
            }
            setCursorPos(0, boardX + selectionX, boardY + selectionY);
        }
    }

    if ((currButtons & BUTTON_A) && (!(prevButtons & BUTTON_A)))
    {
        if(paused)
        {
            doUnPause();            
        }
        else
        {
            if(!levelDone)
            {
                if ((selectionX > -1) && (selectionX < boardWidth) &&
                    (selectionY > -1) && (selectionY < boardHeight))
                {   
                    if (gameMode != gmSlide)
                    {
                        rotateBlock((int)selectionX + ((int)selectionY * boardWidth));
                        moves++;
                        playGameAction();
                        needRedraw = 1;
                    }
                    else
                    {
                        playErrorSound();
                    }
                }
                else
                {
                    if ((selectionX > -1) && (selectionX < boardWidth))
                    {
                        if (selectionY == -1)
                        {
                            moveBlockDown((int)selectionX + ((int)(selectionY+1) * boardWidth));
                            moves++;
                            playGameAction();
                            needRedraw = 1;
                        }
                        else
                        {
                            if (selectionY == boardHeight)
                            {
                                moveBlockUp((int)selectionX + ((int)(selectionY-1) * boardWidth));
                                moves++;
                                playGameAction();
                                needRedraw = 1;
                            }
                        }
                    }
                    else
                    {
                        if ((selectionY > -1) && (selectionY < boardHeight))    
                        {
                            if (selectionX == -1)
                            {
                                moveBlockRight((int)(selectionX + 1) + ((int)selectionY * boardWidth));
                                moves++;
                                playGameAction();
                                needRedraw = 1;
                            }
                            else
                            {
                                if (selectionX == boardWidth)
                                {
                                    moveBlockLeft( (int)(selectionX - 1) + ((int)selectionY * boardWidth));
                                    moves++;
                                    playGameAction();
                                    needRedraw = 1;
                                }
                            }
                        }
                        else
                        {
                            playErrorSound();
                        }
                    }
                }
                updateConnected();
                levelDone = isLevelDone();
                if(levelDone)
                {
                    fpsStep = 11; //to display solution for brief moment
                    SelectMusic(musLevelClear);
                    //hide cursor it's only sprite we use
                    hideCursors();                    
                    levelDoneBit = 1; 
                    prevButtons = currButtons;
                    needRedraw = 1;
                }
            }
            else 
            {
                levelDoneBit = 0;
                //could have been enabled / disabled using menu's so need to replay as it's blocked from renabling
                //during pause leveldonebit = 1
                setMusicOn(isSoundOnSaveState());
                setSoundOn(isSoundOnSaveState());
                //goto next level
                if (difficulty == diffRandom)
                {
                    //ned new seed based on time
                    randomSeedGame = Platform_RandomSeed() + framecount;
                    initLevel(randomSeedGame);
                    SelectMusic(musNoMusic);
                    //show cursor again (it's actually to early but i'm not fixing that)
                    setCursorPos(0, boardX + selectionX, boardY + selectionY);
                    showCursors();
                    needRedraw = 1;
                }
                else
                {   
                    //goto next level if any
                    if (selectedLevel < maxLevel)
                    {
                        selectedLevel++;
                        unlockLevel(gameMode, difficulty, selectedLevel-1);
                        initLevel(randomSeedGame);
                        SelectMusic(musNoMusic);
                        //show cursor again (it's actually to early but i'm not fixing that)
                        setCursorPos(0, boardX + selectionX, boardY + selectionY);
                        showCursors();
                        needRedraw = 1;
                    }
                    else //Goto some congrats screen
                    {
                        gameState = gsInitLevelsCleared;                         
                    }
                }
            }
        }
    }

    if (currButtons & BUTTON_B)
    {
        if (prevButtons & BUTTON_B)
        {
            if (levelDone)
            {
                fpsStep = 1;
                needRedraw = 1;
            }
        }
        if (!(prevButtons & BUTTON_B))
        {
            if (!levelDone)
            {
                if (!paused)
                {
                    playMenuBackSound();
                    doPause();
                    needRedraw = 1;
                }
                else
                {
                    //need to reanble early 
                    setSoundOn(isSoundOnSaveState());
                    setMusicOn(isSoundOnSaveState());
                    hideCursors();
                    playMenuBackSound();
                    gameState = gsInitLevelSelect;
                    doUnPause();
                    //unpause sets cursor visible !
                    hideCursors();
                    //need to reset the level to initial state when going back to level selector
                    //could not find a better way unfortunatly
                    //also we do not want to reset the randomseed used for random level generating
                    //or a new level would have been created when going back we only want the level
                    //with random to change when pressing left and right in the level selector
                    //this way it stays consistent with the normal levels
                    //and the player can replay the level if he wants to
                    initLevel(randomSeedGame);
                }
            }
        }
    }
    if (needRedraw)
    {
        updateBackgroundGame();
    }
}
