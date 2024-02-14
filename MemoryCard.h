#if !defined(MemoryCard_H)
#define MemoryCard_H

#include <SD.h>
#include "LedBlinker.h"
#include "Setting.h"

class MemoryCard
{
private:

public:
    MemoryCard(/* args */);
    ~MemoryCard();

    bool CreateCard(uint8_t* patch,uint8_t *Card);
    bool DeleteCard(uint8_t* patch,uint8_t *Card);
    bool GetCard(uint8_t* patch,uint8_t *Card);
};

MemoryCard::MemoryCard(/* args */)
{
    if (!SD.begin(4))
    {
        Serial.println(F("Initialization SD card failed!"));
        while (true)
        {
            LedBlinker::LedBlinkerStrobe(A2);
        }
    }
    Serial.println(F("Initialization SD card done."));
}

MemoryCard::~MemoryCard()
{
}

/**
 * Создание файла с номером ключ карты
 */
bool MemoryCard::CreateCard(uint8_t* patch,uint8_t *Card)
{
    if(!GetCard(patch,Card))
    {
        Serial.println(F("Creating Card"));

        File myFile = SD.open(Card, FILE_WRITE);

        myFile.close();
        return true;
    }
    return false;
}

/**
 * Удаление файла с номером ключ карты
 */
bool MemoryCard::DeleteCard(uint8_t* patch,uint8_t *Card)
{
    if(!GetCard(patch,Card))
    {
        Serial.println(F("Removing Card..."));
        SD.remove(Card);
        return true;
    }
    return false;
}

/**
 * Поиск файла ключ карты
 */
bool MemoryCard::GetCard(uint8_t* patch,uint8_t *Card)
{
    uint8_t* fullpatch;
    strcat(fullpatch, patch);
    strcat(fullpatch, Card);
    if (SD.exists(fullpatch))
    {
        Serial.println(F("Card exists."));
        return true;
    }
    else
    {
        Serial.println(F("Card doesn't exist."));
        return false;
    }
}

#endif // MemoryCard_H
