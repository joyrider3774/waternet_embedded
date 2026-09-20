#include <stdint.h>
#include <string.h>

#include "commonvars.h"
#include "sound.h"
#include "savestate.h"

//I (joyrider3774) created the music in this tool : https://onlinesequencer.net

uint8_t music_loop, prev_music, music_sustain_note, music_on, sound_on, sfx_sustain;
uint16_t music_note, music_tempo, music_length;
volatile uint8_t sfx_setting_note, sfx_sustain_zero_set, music_sustain_zero_set, selecting_music;
//points straight at the PLATFORM_PROGMEM tables, copying them into a fixed buffer overflowed it
//(music_game alone is 404 bytes) and corrupted the globals that follow it
const uint16_t* musicArray = nullptr;

#define NOTE_REST 0
#define NOTE_C4  262
#define NOTE_CS4 277
#define NOTE_D4  294
#define NOTE_DS4 311
#define NOTE_E4  330
#define NOTE_F4  349
#define NOTE_D6  1175
#define NOTE_DS6 1245
#define NOTE_E6  1319
#define NOTE_F6  1397

constexpr uint16_t pause1 = 4U;
constexpr uint16_t pause2 = 1U;
constexpr uint16_t pause3 = 3U;
constexpr uint16_t pause4 = 3U;



//https://onlinesequencer.net/2498607
const uint16_t PLATFORM_PROGMEM music_levelsCleared[] ={
    NOTE_D6,pause4,
    NOTE_DS6,pause4,
    NOTE_D6,pause4,
    NOTE_REST,pause4,
    NOTE_D6,pause4,
    NOTE_E6,pause4,
    NOTE_F6,pause4,
    NOTE_E6,pause4,
    NOTE_E6,pause4,
    NOTE_E6,pause4,
    NOTE_D6,pause4,
    NOTE_REST,pause4,
    NOTE_DS6,pause4,
    NOTE_E6,pause4,
    NOTE_DS6,pause4,
    NOTE_REST,pause4,
    NOTE_E6,pause4,
    NOTE_F6,pause4,
    NOTE_E6,pause4,
    NOTE_REST,pause4,
    NOTE_D6,pause4,
    NOTE_DS6,pause4,
    NOTE_D6,pause4,
    NOTE_E6,pause4,
    NOTE_E6,pause4,
    NOTE_DS6,pause4,
    NOTE_DS6,pause4,
    NOTE_D6,pause4,
    NOTE_D6,pause4,
    NOTE_DS6,pause4,
    NOTE_E6,pause4,
    NOTE_DS6,pause4,
    NOTE_REST,pause4,
    NOTE_E6,pause4,
    NOTE_DS6,pause4,
    NOTE_E6,pause4,
    NOTE_DS6,pause4,
    NOTE_DS6,pause4,
    NOTE_E6,pause4,
    NOTE_REST,pause4,
    NOTE_E6,pause4,
    NOTE_F6,pause4,
    NOTE_DS6,pause4,
    NOTE_E6,pause4,
    NOTE_REST,pause4,
    NOTE_F6,pause4,
    NOTE_E6,pause4,
    NOTE_F6,pause4,
};

//https://onlinesequencer.net/2484974
const uint16_t PLATFORM_PROGMEM music_won[] ={
  NOTE_DS6, pause2,
  NOTE_D6,  pause2,
  NOTE_DS6, pause2,
  NOTE_E6, pause2,
  NOTE_DS6, pause2,
  NOTE_E6, pause2,
  NOTE_F6, pause2 << 2,
  NOTE_REST, pause2,
};


void setMusicOn(uint8_t value)
{
    music_on = value;
    if(music_on)
    {
        if (prev_music != 0)
            SelectMusic(prev_music);
    }
    else
    {
        Platform_StopTone();
    }
}

void setSoundOn(uint8_t value)
{
    sound_on = value;
}

uint8_t isMusicOn()
{
    return music_on;
}

uint8_t isSoundOn()
{
    return sound_on;
}

void initSound()
{
    sound_on = isSoundOnSaveState();
}


void SelectMusic(uint8_t musicFile)
{
    if (prev_music != musicFile)
    {
        selecting_music = 1;
        prev_music = musicFile;
        musicArray = nullptr;
        music_length = 0;
        music_loop = 0;
        switch (musicFile)
        {
            case musLevelClear:
                musicArray = music_won;
                music_length = sizeof(music_won) / sizeof(music_won[0]);
                break;
            case musAllLevelsClear:
                musicArray = music_levelsCleared;
                music_length = sizeof(music_levelsCleared) / sizeof(music_levelsCleared[0]);
                break;
        }
        music_note = 0;
        music_tempo = 0;
        music_sustain_zero_set = 1;
        music_sustain_note = 0;
        Platform_StopTone();
    }
    selecting_music = 0;
}

void playNote()
{    
    if(musicArray && (music_note + 1 < music_length))
    {
        Platform_PlayTone(PLATFORM_READ_WORD(&musicArray[music_note]), 0);
        music_sustain_note = 10;

        //Set the new delay to wait
        music_tempo = PLATFORM_READ_WORD(&musicArray[music_note + 1]);

        //Skip to the next note
        music_note += 2;
               
        if (music_note > music_length - 1)
        {
            if(music_loop)
            {
                music_note = 0;
            }
            else
            {
                Platform_StopTone();
            }
        }
    }
}


void musicTimer()
{
    //for nintendo systems as sega one checks it earlier
    if (selecting_music)
    {
        return;
    }

    //Play some music
    if (music_tempo == 0)
    {
        if(music_on)
        {
            playNote();
        }
    }
    //Else wait for the next note to play
    else 
    {
        music_tempo--;
        if(music_sustain_note == 0)
        {
            if (!music_sustain_zero_set)
            {
                music_sustain_zero_set = 1;
            }
        }
        else
        {
            music_sustain_note--;
        }
    }
}

void initMusic()
{
    music_on = isSoundOnSaveState();
    music_sustain_note = 0;
    prev_music = 0;
    music_note = 0;
    music_length = 0;
    music_tempo = 0;
    music_loop = 0;
    //set to 1 so nothing plays until a music was selected
    selecting_music = 1;
    music_sustain_zero_set = 1;
}


void playSound(uint16_t freq)
{
    if(!sound_on)
        return;
    Platform_PlayTone(freq, sfxSustain);
}

void playGameMoveSound()
{
    playSound(750);
}

void playErrorSound()
{
   playSound(300);
}

void playMenuSelectSound()
{
    playSound(1250);
}

void playMenuBackSound()
{
   playSound(1000);
}

void playMenuAcknowlege()
{
    playSound(900);
}

void playGameAction()
{
    playSound(600);
}
