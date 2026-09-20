#ifndef defines_h
#define defines_h

//the device comes first: the display library and SCREENBUFFER are device settings, see
//PlatformESPboy.h / PlatformSDL.h
#include "PlatformDevice.h"

//the ESPboy display
#define WINDOW_WIDTH 128
#define WINDOW_HEIGHT 128

#define tileSize 12

#define maxBoardWidth 10
#define maxBoardHeight 8

#define maxBoardBgWidth 10
#define maxBoardBgHeight 8

#define maxBoardSize 80

#define gsGame 0
#define gsTitle 1
#define gsLevelSelect 2
#define gsLevelsCleared 3
#define gsHelpRotate 4
#define gsHelpRotate2 5
#define gsHelpRotate3 6
#define gsHelpRotateSlide 7
#define gsHelpRotateSlide2 8
#define gsHelpRotateSlide3 9
#define gsHelpRotateSlide4 10
#define gsHelpSlide 11
#define gsHelpSlide2 12
#define gsHelpSlide3 13
#define gsIntro 14

#define gsInitDiff 50

#define gsInitGame (gsInitDiff + gsGame)
#define gsInitTitle (gsInitDiff + gsTitle)
#define gsInitLevelSelect (gsInitDiff + gsLevelSelect)
#define gsInitLevelsCleared (gsInitDiff + gsLevelsCleared)
#define gsInitHelpRotate (gsInitDiff + gsHelpRotate)
#define gsInitHelpRotate2 (gsInitDiff + gsHelpRotate2)
#define gsInitHelpRotate3 (gsInitDiff + gsHelpRotate3)
#define gsInitHelpRotateSlide (gsInitDiff + gsHelpRotateSlide)
#define gsInitHelpRotateSlide2 (gsInitDiff + gsHelpRotateSlide2)
#define gsInitHelpRotateSlide3 (gsInitDiff + gsHelpRotateSlide3)
#define gsInitHelpRotateSlide4 (gsInitDiff + gsHelpRotateSlide4)
#define gsInitHelpSlide (gsInitDiff + gsHelpSlide)
#define gsInitHelpSlide2 (gsInitDiff + gsHelpSlide2)
#define gsInitHelpSlide3 (gsInitDiff + gsHelpSlide3)
#define gsInitIntro (gsInitDiff + gsIntro)

#define diffVeryEasy 0
#define diffEasy 1
#define diffNormal 2
#define diffHard 3
#define diffVeryHard 4
#define diffRandom 5
#define diffCount 6

#define gmRotate 0
#define gmSlide 1
#define gmRotateSlide 2
#define gmCount 3

#define mmStartGame 0
#define mmHelp 1
#define mmOptions 2
#define mmCredits 3
#define mmCount 4

#define opSound 0
#define opSkins 1
#define opCount 2

#define tsMainMenu 0
#define tsGameMode 1
#define tsDifficulty 2
#define tsOptions 3
#define tsCredits 4

#define levelCount 25
#define arrowDown 122
#define arrowUp 120
#define arrowLeft 123
#define arrowRight 121
#define leftMenu 118
#define emptyFloor 49

#define maxSkins 4
//the same in every skin, so the screens that draw them need no skin header
#define titleScreenWidth 128
#define titleScreenHeight 40
#define congratsScreenWidth 128
#define congratsScreenHeight 64
#define skinBlackWhite 1

//FORCESKIN: -1 = every skin is built in and can be picked in the options, n = only skin n
//(0 default, 1 black & white, 2 viaduct, 3 sonic) is built in and always used, which saves the flash of the others on a
//small device. A 1 bpp buffer can only show the black & white skin, so it forces that one.
//Set by the device header or the build
#if SCREENBUFFER == 1
  #if defined(FORCESKIN) && (FORCESKIN >= 0) && (FORCESKIN != skinBlackWhite)
  #error "a 1 bpp buffer can only show the black & white skin, FORCESKIN has to be -1 or 1"
  #endif
  #undef FORCESKIN
  #define FORCESKIN skinBlackWhite
#elif !defined(FORCESKIN)
  #define FORCESKIN -1
#endif
//1 when the images of skin n are part of the build
#define SKINBUILT(n) ((FORCESKIN < 0) || (FORCESKIN == (n)))

#define FRAMERATE 15
//1 = every frame waits until 1/FRAMERATE of a second has passed, 0 = a frame starts as soon
//as the last one is done, to see how fast the game can go. Movement, animation, input
//repeat and music all count frames, so without the lock they run faster as well.
//A build can set it itself
#ifndef FPSLOCK
#define FPSLOCK 1
#endif
//1 = the debug header (frame rate, free heap and stack) is always shown, Up + Down does not
//hide it. 0 = it starts hidden and Up + Down shows and hides it. A build can set it itself
#ifndef FORCEDEBUG
#define FORCEDEBUG 0
#endif

#define SMALL_X_OFFSET 0
#define SMALL_Y_OFFSET 8
#define BIG_X_OFFSET 4
#define BIG_Y_OFFSET 4

#endif
