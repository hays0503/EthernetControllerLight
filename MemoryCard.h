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

    bool CreateCard(uint8_t *patch, char *Card);
    bool DeleteCard(uint8_t *patch, char *Card);
    bool GetCard(uint8_t *patch, const uint8_t Card[6]);
    bool GetCard(uint8_t *patch, const char Card[6]);
    bool ClearCards(uint8_t *patch);
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
 * Очищает все карты
 */
bool MemoryCard::ClearCards(uint8_t *patch)
{
    File root = SD.open(patch);
    while (true)
    {
        File entry = root.openNextFile();
        if (!entry)
        { // no more files

            break;
        }
        SD.remove(entry.name());
    }

}

/**
 * Создание файла с номером ключ карты
 */
bool MemoryCard::CreateCard(uint8_t *patch, char *Card)
{
    if (!GetCard(patch, Card))
    {
        char *fullpatch = new char[13];
        snprintf(fullpatch, 13, "%s%s", patch, Card);
        Serial.println(F("Creating Card"));

        File myFile = SD.open(fullpatch, FILE_WRITE);

        myFile.close();
        delete fullpatch;
        fullpatch = nullptr;
        return true;
    }
    return false;
}

/**
 * Удаление файла с номером ключ карты
 */
bool MemoryCard::DeleteCard(uint8_t *patch, char *Card)
{
    if (GetCard(patch, Card))
    {
        char *fullpatch = new char[13];
        snprintf(fullpatch, 13, "%s%s", patch, Card);
        Serial.println(F("Removing Card..."));
        SD.remove(fullpatch);
        delete fullpatch;
        fullpatch = nullptr;
        return true;
    }
    return false;
}

/**
 * Поиск файла ключ карты
 */
bool MemoryCard::GetCard(uint8_t *patch, const uint8_t Card[6])
{
    char *fullpatch = new char[13];
    snprintf(fullpatch, 13, "%s%02X%02X%02X", patch, Card[3], Card[2], Card[1]);
    if (SD.exists(fullpatch))
    {
        Serial.println(F("Card exists."));
        delete fullpatch;
        fullpatch = nullptr;
        return true;
    }
    else
    {
        Serial.println(F("Card doesn't exist."));
        delete fullpatch;
        fullpatch = nullptr;
        return false;
    }
}

/**
 * Поиск файла ключ карты
 */
bool MemoryCard::GetCard(uint8_t *patch, const char Card[6])
{
    char *fullpatch = new char[13];
    snprintf(fullpatch, 13, "%s%s", patch, Card);
    if (SD.exists(fullpatch))
    {
        Serial.println(F("Card exists."));
        delete fullpatch;
        fullpatch = nullptr;
        return true;
    }
    else
    {
        Serial.println(F("Card doesn't exist."));
        delete fullpatch;
        fullpatch = nullptr;
        return false;
    }
}

#endif // MemoryCard_H
