#include <string.h>
#include <stdint.h>

#include "commonvars.h"


constexpr int storageStart = 493;
constexpr uint8_t soundOptionBit = 0U;
constexpr uint8_t musicOptionBit = 1U; 


uint32_t levelLocksPacked[3];
uint8_t options = 0; //bit 0 sound on/off, bit 1 music on/off
uint8_t skin = 0;

//everything saved has to fit in what the platform stores
static_assert(storageStart + sizeof(levelLocksPacked) + sizeof(options) + sizeof(skin) <= PLATFORM_STORAGE_SIZE, "the save state does not fit in PLATFORM_STORAGE_SIZE");

int addrLevelLocksPacked, addrOptions, addrSkins;

uint8_t clearBit8(uint8_t val, uint8_t bit)
{
    if(bit > 7)
        return val;
    return val & (~(1U << bit));
}

uint8_t setBit8(uint8_t val, uint8_t bit)
{
    if (bit > 7)
        return val;
    return val | (1U << bit);
}

uint8_t checkBit8(uint8_t val, uint8_t bit)
 {
    if (bit > 7)
        return 0; 
    return ((val >> bit) & 1U);
 }

uint32_t clearBit32(uint32_t val, uint8_t bit)
{
    if(bit > 31)
        return val;
    return val & (~(1UL << bit));
}

uint32_t setBit32(uint32_t val, uint8_t bit)
{
    if (bit > 31)
        return val;
    return val | (1UL << bit);
}

uint8_t checkBit32(uint32_t val, uint8_t bit)
 {
    if (bit > 31)
        return 0; 
    return ((val >> bit) & 1UL);
 }

void packLevelLock(uint8_t mode, uint8_t diff, uint8_t level)
{
    uint8_t levelIndex = (mode * diffCount) + diff;
    uint8_t pack = levelIndex / 6; // 6 x 5 bit nr in a pack so have to divide levelindex by 6 to get packnr
    uint8_t bit = (levelIndex * 5) - (pack * 30); //6 x 5 bit nr in a pack so level index * 5 = bit nr acrross all packs - pack * 30 to know bit for current pack 
    for (uint8_t i = 0; i<5; i++)
    {
        if (checkBit8(level, i))
            levelLocksPacked[pack] = setBit32(levelLocksPacked[pack], bit + i);
        else
            levelLocksPacked[pack] = clearBit32(levelLocksPacked[pack], bit + i);
    }
}

uint8_t unPackLevelLock(uint8_t mode, uint8_t diff)
{
    uint8_t levelIndex = (mode * diffCount) + diff;
    uint8_t pack = levelIndex / 6; // 6 x 5 bit nr in a pack so have to divide levelindex by 6 to get packnr
    uint8_t bit = (levelIndex * 5) - (pack * 30); //6 x 5 bit nr in a pack so level index * 5 = bit nr acrross all packs - pack * 30 to know bit for current pack 
    uint8_t result = 0;
    for (uint8_t i = 0; i<5; i++)
    {
        if (checkBit32(levelLocksPacked[pack], bit+i))
            result = setBit8(result, i);
    }
    return result;
}
            
uint8_t validateSaveState()
{
    uint8_t levelsUnlocked = 0;
    for (uint8_t j=0; j<gmCount; j++)
    {
        for (uint8_t i=0; i<diffCount; i++)
        {
            levelsUnlocked = unPackLevelLock(j, i);
            if ((levelsUnlocked == 0) || (levelsUnlocked > levelCount))
                return 0;
        }
    }
    if (options > 3) //bit 0 & 1 = 3
        return 0;
    if (skin > maxSkins)
        return 0;
    return 1;
}

void initSaveState()
{
    //read storage
    addrLevelLocksPacked = storageStart;
    Platform_StorageRead(addrLevelLocksPacked, (uint8_t*)&levelLocksPacked, sizeof(levelLocksPacked));
    addrOptions = addrLevelLocksPacked + sizeof(levelLocksPacked);
    Platform_StorageRead(addrOptions, (uint8_t*)&options, sizeof(options));
    addrSkins = addrLevelLocksPacked + sizeof(levelLocksPacked) + sizeof(options);
    Platform_StorageRead(addrSkins, (uint8_t*)&skin, sizeof(skin));
    if(!validateSaveState())
    { 
        levelLocksPacked[0] = 0UL;
        levelLocksPacked[1] = 0UL;
        levelLocksPacked[2] = 0UL;
        for (uint8_t j=0; j<gmCount; j++)
            for (uint8_t i=0; i<diffCount; i++)
                packLevelLock(j, i, 1U); //1st level unlocked
        options = 3; //bit 0 & 1 set = music & sound on
        skin = 0;
    }
}

void saveSaveState()
{
    Platform_StorageWrite(addrLevelLocksPacked, (const uint8_t*)&levelLocksPacked, sizeof(levelLocksPacked));
    Platform_StorageWrite(addrOptions, (const uint8_t*)&options, sizeof(options));
    Platform_StorageWrite(addrSkins, (const uint8_t*)&skin, sizeof(skin));
}

void setSkinSaveState(uint8_t value)
{
    if(value < maxSkins)
    {
        skin = value;
        saveSaveState();
    }
}

uint8_t skinSaveState()
{
    return skin;
}

void setMusicOnSaveState(uint8_t value)
{
    if (value)
    {
        options = setBit8(options, musicOptionBit);
    }
    else
    {
        options = clearBit8(options, musicOptionBit);
    }
    saveSaveState();  
}

uint8_t isMusicOnSaveState()
{
    return checkBit8(options, musicOptionBit);
}

void setSoundOnSaveState(uint8_t value)
{
    if (value)
        options = setBit8(options, soundOptionBit);
    else
        options = clearBit8(options, soundOptionBit);
    saveSaveState();
}

uint8_t isSoundOnSaveState()
{
    return checkBit8(options, soundOptionBit);
}

uint8_t levelUnlocked(uint8_t mode, uint8_t diff, uint8_t level)
{
    return (unPackLevelLock(mode, diff) > level);
}

uint8_t lastUnlockedLevel(uint8_t mode, uint8_t diff)
{
    return unPackLevelLock(mode, diff);
}

void unlockLevel(uint8_t mode, uint8_t diff, uint8_t level)
{
    if (level + 1> lastUnlockedLevel(mode, diff))
    {
        packLevelLock(mode, diff, level + 1);
        saveSaveState();
    }
}