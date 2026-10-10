#include <stdint.h>

#include "helperfuncs.h"
#include "commonvars.h"
#include "printfuncs.h"
#include "intro.h"
//the screen is painted a strip at a time where there is no buffer
#include "bandrender.h"

#define FRAMEDELAY 10

int frames = 0;
int ay;

void initIntro(void)
{
	frames = 0;
    setBlockTilesAsBackground();
    ay = WINDOW_HEIGHT;
}

static void drawIntroOnce(void);

void intro(void)
{
    if (gameState == gsInitIntro)
    {
        initIntro();
        gameState -= gsInitDiff;
    }
    
    frames++;
    //A strip at a time, so the intro is not seen being painted. The drawing below only reads
    //frames and changes nothing, which is what lets it be called once per strip; see bandrender.h
    if (BandRender_Begin(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT, ColorBlack))
    {
        while (BandRender_Next())
            drawIntroOnce();
    }
    else
        drawIntroOnce();

    //The title slides up a step a frame, once the two words before it have had their turn. It
    //used to be stepped inside the drawing, which is why it must be here now
    if (frames >= FRAMEDELAY * 2)
    {
        if (ay > 16)
            ay -= 10;
        else
            gameState = gsInitTitle;
    }
}

static void drawIntroOnce(void)
{
    //the strip already starts as this colour, see BandRender_Begin
    if (!BandRender_Drawing())
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
            //where the title has slid to, which intro() moves on once a frame: this is run
            //once per strip and taking the step here took it as many times
            set_bkg_tiles(0, ay, titleScreenWidth, titleScreenHeight,imgTitleScreen);
        }
    }
        
    if (((currButtons & BUTTON_A) && (!(prevButtons & BUTTON_A))) ||
        ((currButtons & BUTTON_B) && (!(prevButtons & BUTTON_B))))
    {            
        gameState = gsInitTitle;
    }

}
