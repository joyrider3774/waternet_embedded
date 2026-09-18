# Waternet Embedded Version
![DownloadCountTotal](https://img.shields.io/github/downloads/joyrider3774/waternet_embedded/total?label=total%20downloads&style=plastic) ![DownloadCountLatest](https://img.shields.io/github/downloads/joyrider3774/waternet_embedded/latest/total?style=plastic) ![LatestVersion](https://img.shields.io/github/v/tag/joyrider3774/waternet_embedded?label=Latest%20version&style=plastic) ![License](https://img.shields.io/github/license/joyrider3774/waternet_embedded?style=plastic)

Waternet is a multiplatform puzzle game written for old consoles and handhelds like Game Boy (Color), Game Gear, Master System, Analogue Pocket and Mega Duck. This version runs on the small handhelds listed below and on Windows.

## Devices
Every [release](https://github.com/joyrider3774/waternet_embedded/releases) has a build for every device. `releases/` is where a build of your own puts them, it is not part of the repository:

| Device | File | How to install |
| ------ | ---- | -------------- |
| [ESPboy](https://www.espboy.com/) | ESPboy_Waternet.bin | flash it, the board is a LOLIN(WEMOS) D1 mini |
| [Gamebuino META](https://gamebuino.com/gamebuino-meta) | GamebuinoMeta_Waternet.bin | copy it into a folder on the SD card, the .hex is for flashing it directly |
| [Adafruit PyBadge](https://www.adafruit.com/product/4200) | PyBadge_Waternet.uf2 | double press reset and copy it onto the drive that appears |
| [Adafruit PyGamer](https://www.adafruit.com/product/4242) | PyGamer_Waternet.uf2 | same as the PyBadge |
| [Pimoroni PicoSystem](https://shop.pimoroni.com/products/picosystem) | PicoSystem_Waternet.uf2 | hold X while switching on and copy it onto the drive that appears |
| [Pimoroni Explorer](https://shop.pimoroni.com/products/explorer?variant=42092697845843) | Explorer_Waternet.uf2 | hold BOOT while pressing RESET and copy it onto the drive that appears |
| [Pimoroni Tufty 2350](https://shop.pimoroni.com/products/tufty-2350?variant=55811986227579) | Tufty_Waternet.uf2 | hold HOME while pressing RESET and copy it onto the drive that appears |
| [TinyCircuits Thumby Color](https://tinycircuits.com/products/thumby-color) | ThumbyColor_Waternet.uf2 | put it into bootloader mode and copy it onto the RPI-RP2 drive that appears |
| [Playdate](https://play.date/) | Playdate_Waternet.pdx.zip | unzip it and sideload Waternet.pdx, the same pdx runs in the Playdate simulator |
| [Libretro / RetroArch](https://www.retroarch.com/) | Libretro_Waternet.zip | copy waternet_libretro.dll into RetroArch's cores folder and waternet_libretro.info into its info folder, then Load Core and Start Core |
| [Game Boy Advance](https://en.wikipedia.org/wiki/Game_Boy_Advance) | GBA_Waternet.gba | put it on a flash cart or open it in an emulator, the progress is saved in the cartridge's SRAM |
| [Nintendo DS](https://en.wikipedia.org/wiki/Nintendo_DS) | NDS_Waternet.nds | put it on a flash card or open it in an emulator, the progress is saved next to it in Waternet.sav |
| [Nintendo 3DS](https://en.wikipedia.org/wiki/Nintendo_3DS) | 3DS_Waternet.3dsx | copy it into /3ds/ on the SD card and start it from the Homebrew Launcher, or open it in an emulator, the progress is saved in sdmc:/3ds/Waternet/ |
| [Nintendo 64](https://en.wikipedia.org/wiki/Nintendo_64) | N64_Waternet.z64 | put it on a flash cart or open it in an emulator, the progress is saved in the cartridge EEPROM |
| [PlayStation](https://en.wikipedia.org/wiki/PlayStation_(console)) | PSX_Waternet.exe | open it in an emulator or send it to a console that runs unsigned code, the progress is not saved yet |
| [PlayStation Portable](https://en.wikipedia.org/wiki/PlayStation_Portable) | PSP_Waternet.PBP | rename it to EBOOT.PBP and put it in ms0:/PSP/GAME/Waternet/ on the memory stick, or open it in PPSSPP |
| [PlayStation Vita](https://en.wikipedia.org/wiki/PlayStation_Vita) | Vita_Waternet.vpk | install it with VitaShell on a Vita with homebrew enabled, or open it in Vita3K |
| Windows | Windows_Waternet.exe | runs on its own, the progress is saved next to it in Waternet.sav |

`python tools/build_releases.py` builds all of them, `python tools/convert_skins.py` turns the images in `assets/skins` into the headers the game includes. The Playdate build also needs the Playdate SDK, see `playdate/CMakeLists.txt`, the libretro core libretro-common, see `libretro/CMakeLists.txt`, the Game Boy Advance build devkitARM and libgba, see `gba/CMakeLists.txt`, the Nintendo DS build devkitARM, libnds and calico, see `nds/CMakeLists.txt`, the Nintendo 3DS build devkitARM and libctru, see `3ds/CMakeLists.txt`, the PlayStation build PSn00bSDK, see `psx/CMakeLists.txt`, the Nintendo 64 build the mips64-elf toolchain and libdragon, see `n64/CMakeLists.txt`, the PSP build the pspdev toolchain, see `psp/CMakeLists.txt` (pspdev has no Windows build, so on Windows it is built from WSL), and the Vita build VitaSDK, see `vita/CMakeLists.txt`.

### Buttons
The game's buttons on every device:

| Device | D-pad | A | B |
| ------ | ----- | - | - |
| ESPboy | d-pad | ACT | ESC |
| Gamebuino META | d-pad | A | B |
| Adafruit PyBadge | d-pad | A | B |
| Adafruit PyGamer | joystick | A | B |
| Pimoroni PicoSystem | d-pad | A | B |
| Pimoroni Explorer | A up, C down, B left, Y right | X | Z |
| Pimoroni Tufty 2350 | UP up, DOWN down, A left, C right | B | HOME |
| TinyCircuits Thumby Color | d-pad | A | B |
| Playdate | d-pad | A | B |
| Libretro | d-pad | A | B |
| Game Boy Advance | d-pad | A | B |
| Nintendo DS | d-pad | A | B |
| Nintendo 3DS | d-pad or circle pad | A | B |
| Nintendo 64 | d-pad | A | B |
| PlayStation | d-pad | Cross | Circle |
| PlayStation Portable | d-pad or the analog stick | Cross | Circle |
| PlayStation Vita | d-pad or the left stick | Cross | Circle |
| Windows | arrow keys | X | C |

On the Tufty 2350 a tap of HOME is B when it is let go. It has no speaker, the game is silent there. Holding RESET until the rear LEDs are dark puts it to sleep, a front button wakes it up again, with UP and DOWN held as well it goes into shipping mode instead.

The Thumby Color's display is 128x128, the game's own size, so it is shown 1:1 over the whole screen. That build has not been tried on the device itself yet.

The Playdate shows the black & white skin, scaled up in the middle of its display.

The Game Boy Advance shows the game scaled to 160x160 in the middle of its screen, with black bars at the sides.

On the Nintendo DS the game is scaled to 192x192 in the middle of the top screen, with black bars at the sides, and the bottom screen stays dark. What the game saves goes into Waternet.sav on the card it was started from, so a card that libfat can not write to (or an emulator without one) plays the game but forgets it afterwards. Its tones are square waves played as a sample: the DS's own tone channels count their frequency in a 16 bit timer and can not go below about 256 Hz.

On the Nintendo 3DS the game is scaled to 240x240 in the middle of the top screen, with black bars at the sides, and the bottom screen stays dark. What the game saves goes into sdmc:/3ds/Waternet/Waternet.sav. Its tones play through the console's DSP when the DSP firmware has been dumped to the SD card (sdmc:/3ds/dspfirm.cdc), and through CSND when it has not: on hardware either one plays, in an emulator only the DSP one does.

On the Nintendo 64 the game is drawn into memory in the colours the RDP takes and the RDP shows it scaled to 240x240 in the middle of its 320x240 screen, with black bars at the sides. Its tones are a square wave written into the buffers the sound hardware plays from. The progress is saved in the cartridge EEPROM, which the ROM says it has, so a cartridge or an emulator without one plays the game but forgets it afterwards.

On the PlayStation the game is drawn into memory in the colours the GPU takes, handed to it as a texture and shown scaled to 240x240 in the middle of its 320x240 screen, with black bars at the sides. Its tones are a square wave the SPU plays from a single looping block. The memory card is not written yet, so what the game saves is gone when the console is switched off.

On the PlayStation Portable the game is doubled to 256x256 in the middle of the display, and the high scores are saved next to the EBOOT.PBP in Waternet.sav.

On the PlayStation Vita the game is blown up four times to 512x512 in the middle of the display, and the high scores are saved in ux0:data/Waternet/Waternet.sav.

On the Gamebuino META holding HOME for a second goes back to its loader.

## Game Features:
- 3 Game modes (Rotate, Slide, RoSlid)
- 5 Difficulties per game mode (very easy, easy, normal, hard, very hard)
- 25 Levels per difficulty
- Unlimited random levels mode, per game mode
- 4 Skins to choose from (default, black & white, viaduct and sonic)
- Help screens for every game mode
- Music and sound can be switched on or off
- Autosaves progress, options and the chosen skin

## Playing the Game:
The aim of the game, in any game mode is always to connect all pipes so that water can flow through them from the water point source. How you can accomplish this depends on the game mode. The game has a help section in the main menu where you can always check up on the rules of each game mode. The game has 375 levels, spread over 3 game mode each with 5 difficulties, so thats 25 levels per difficulty / game mode. To top that off the game also provides a random difficulty which will generate never ending, random levels for the game mode you had chosen.

## Game Modes
The Game has 3 game modes

### Rotate Mode
You need to connect all the pipes so water flows through them, by pressing the A button on a pipe, to rotate the single pipe.

### Slide Mode
You need to connect all the pipes so water flows through them, by pressing the A  button on the arrows of a row or column. The row or column will move all pipes in the direction the arrow is pointing at.

### Roslid Mode
You need to connect all the pipes so water flows through them, by pressing the A button on the arrows of a row or column. The row or column will move all pipes in the direction the arrow is pointing at. You can also press the A button on a pipe, to rotate the single pipe. This is a combination Rotate and Slide mode.

## Controls

| Button | Action |
| ------ | ------ |
| Dpad | Select menu's, options or levels to play, in random mode left or right creates a new level. During gameplay move the selector tile around. |
| A | Confirm in menu and level selector, rotate or slide action while playing, continue to the next level when a level is solved |
| B | Back in menu and level selector. During gameplay pause the game (A resumes, B quits to the level selector). Hold it when a level is solved to see the solved level |
| (A) + Left + Down | Show or hide the debug info |

## Credits
The Game Concept is based on net and netslide games from the [simon tatham's puzzle collections](https://www.chiark.greenend.org.uk/~sgtatham/puzzles/)
The viaduct skin is based on the "FP_Aquaeduct" skin from the gp2x colored version created by Fusion Power
The sonic skin is based on the "FP_Brixx" skin from the gp2x colored version, which was a small tribute to Sonic 2 "Emerald Hill" Stage and also created by Fusion Power
