#include <stdint.h>

#include "helperfuncs.h"
#include "commonvars.h"
#include "printfuncs.h"
#include "intro.h"

#define FRAMEDELAY 10

int frames = 0;
int ay;

void initIntro(void)
{
	frames = 0;
    setBlockTilesAsBackground();
    ay = WINDOW_HEIGHT;
}

void intro(void)
{
    if (gameState == gsInitIntro)
    {
        initIntro();
        gameState -= gsInitDiff;
    }
    
    frames++;
    GFX.fillRect(0,0,WINDOW_WIDTH, WINDOW_HEIGHT, ColorBlack);
    if (frames < FRAMEDELAY)
    {
        printMessage((16-12) >> 1, 7, "WILLEMS DAVY");
    }
    else
    {
        if (frames < FRAMEDELAY *2)
        {
            printMessage((16-8) >> 1, 7, "PRESENTS");
        }
        else
        {
            set_bkg_tiles(0, ay, titleScreenWidth, titleScreenHeight,imgTitleScreen);
            if(ay > 16)
            {
                ay -= 10;
            }
            else
            {
                gameState = gsInitTitle;
            }
        }
    }
        
    if (((currButtons & BUTTON_A) && (!(prevButtons & BUTTON_A))) ||
        ((currButtons & BUTTON_B) && (!(prevButtons & BUTTON_B))))
    {            
        gameState = gsInitTitle;
    }

}
