
#include <Arduino.h>
#include <SPI.h>
#include "WebJsonLight.h"
#include "EthernetAdaptor.h"
#include "Controller.h"
#include "LedBlinker.h"
#include "ControllerTime.h"
#include "MemoryCard.h"
#include "Setting.h"

uint8_t Active = 0;
uint8_t Online = 0;
uint8_t Mode = 0;
uint8_t Reader = 0;


ThreeWire myWire(7, 6, 8);  // Указываем вывода IO, SCLK, CE
RtcDS1302<ThreeWire> Rtc(myWire);

Matrix *MatrixArr[2];  // <--- Считыватели

EthernetAdaptor Obj_EthernetAdaptor;  // <--- Адаптер для общение

WebJsonLightSender Obj_WebJsonLightSender;
WebJsonLightReceive Obj_WebJsonLightReceive;
MemoryCard *Obj_MemoryCard;

uint8_t Card[8];
char *Ans = nullptr;
uint16_t Ans_size_msg = 0;


void setup() {
  pinMode(GREEN_LED, OUTPUT);
  pinMode(RED_LED, OUTPUT);
  Serial.begin(9600);

  MatrixArr[0] = new Matrix(2);
  MatrixArr[1] = new Matrix(3);
  Rtc.Begin();


  Serial.println(F("+++System start+++"));

  Serial.println(F("+++System initialize+++"));

  // наш личный "setup"
  Obj_EthernetAdaptor.ConfigTCPip(
    IP_CURRENT,
    DNS_CURRENT,
    MASK_CURRENT,
    MAC_CURRENT);

  Obj_EthernetAdaptor.ConfigClient(
    IP_CLIENT,
    PORT_CLIENT);

  Obj_MemoryCard = new MemoryCard();

  Serial.println(F("+++End system initialize+++"));



PowerOnLabel:

  while (Active == 0) {
    Serial.println(F("+++Send Power on+++"));
    //////////////Подача//питание//////////////////
    char* MSG  = new char[20];
    uint8_t size_msg = Obj_WebJsonLightSender.Power_On(MSG, 1111, Active, Mode);
    if (Obj_EthernetAdaptor.Post(MSG, size_msg, Ans, Ans_size_msg) != -1) {
      delete MSG;
      MSG = nullptr;
      Serial.println(F("+++Parse answer+++"));
      if (Obj_WebJsonLightReceive.Search(Ans, fnSetActive)) {
        Serial.println(F("+++OK+++"));
      }
      delay(3000);
    } else {
      delete MSG;
      MSG = nullptr;
      Serial.println(F("RED_LED0"));
      LedBlinker::LedBlinkerSOS(RED_LED);
    }

    delete Ans;
    Ans = nullptr;

    ///////////////////////////////////////////////
  }

  for (;;) {
    // наш личный "loop"

    for (Reader = 0; Reader < 2; Reader++) {

      // Проверить Считыватель
      if (MatrixArr[Reader]->Read()) {
        ///////////////////////////////////////////////
        // Считыватель
        MatrixArr[Reader]->getCard(Card);  // Тут мы забрали ключ

        /////////////Событие///////////////////////////
        if (!Online) {
          if(Obj_MemoryCard->GetCard(PATCH_TO_CARD,Card)){
              Serial.println(F("+++fnOpenDoor+++"));
              fnOpenDoor(Card, &Reader);
          }


          delete Ans;     // Очищаем для следующего ответа
          Ans = nullptr;  // Очищаем для следующего ответа

        }
        ///////////////////////////////////////////////
        else
        /////////////Проверка//на//доступ////////////////////////////
        {
          char* MSG = new char[14];
          uint8_t size_msg = Obj_WebJsonLightSender.Check_access(MSG, Card, Reader);
          if (Obj_EthernetAdaptor.Post(MSG, size_msg, Ans, Ans_size_msg) != -1)
          {
            delete MSG;
            MSG = nullptr;
            Obj_WebJsonLightReceive.Search(
              Ans,
              fnSetActive,   // Активировать контроллер
              fnSetMode,     // Установить режим котроллера
              fnSetTime,     // Установить время в контроллере
              fnAddCards,    // Добавить карту в контроллер
              fnDelCards,    // Удалить карту из контроллера
              fnClearCards,  // Удалить все карты
              fnCheckAcess,  // Побочные действия на 2 факторную аунтификацию
              fnOpenDoor     // Открыть дверь
            );
          }else{
            delete MSG;
            MSG = nullptr;
            Serial.println(F("RED_LED1"));
            LedBlinker::LedBlinkerSOS(RED_LED);
          }
        }
        ///////////////////////////////////////////////
      }
      //////////////Пинг//////////////////////////////

      if (true) {
        Serial.println(F("!!!!!!!!!!!!!!!!!!PING!!!!!!!!!!!!!!!!!!!!"));
        char* MSG = new char[8];
        uint8_t size_msg = Obj_WebJsonLightSender.Ping(MSG, Active, Mode);
        if (Obj_EthernetAdaptor.Post(MSG, size_msg, Ans, Ans_size_msg) != -1)
        {
          delete MSG;
          MSG = nullptr;
          ////////////////////////////////////////////////////
          Obj_WebJsonLightReceive.Search(
                Ans,
                fnSetActive,   // Активировать контроллер
                fnSetMode,     // Установить режим котроллера
                fnSetTime,     // Установить время в контроллере
                fnAddCards,    // Добавить карту в контроллер
                fnDelCards,    // Удалить карту из контроллера
                fnClearCards,  // Удалить все карты
                fnCheckAcess,  // Побочные действия на 2 факторную аунтификацию
                fnOpenDoor     // Открыть дверь
                );
          if (Active == 0) {
            goto PowerOnLabel;
          }

        } else {
          Serial.println(F("RED_LED2"));
          LedBlinker::LedBlinkerSOS(RED_LED);
        }
        ////////////////////////////////////////////////////
        delete Ans;
        Ans = nullptr;
      }

      ///////////////////////////////////////////////
    }
  }
};

void loop(){};


/**
 * \brief Активировать контроллер
*/
void fnSetActive(uint8_t *active, uint8_t *online) {
  Active = *active;
  Online = *online;
}

/**
 * \brief Установить режим котроллера
*/
void fnSetMode(uint8_t *mode) {
  Mode = *mode;
}

/**
 * \brief Установить время в контроллере 
*/
void fnSetTime(uint32_t *time) {
  setTimeController(&Rtc, time);
}

/**
 * \brief Добавить карту в контроллер
*/
void fnAddCards(char *card) {
  //Добавляем карту
  Obj_MemoryCard->CreateCard(PATCH_TO_CARD,card);
}

/**
 * \brief Удалить карту из контроллера
*/
void fnDelCards(char *card) {
  //Удаляем карту
  Obj_MemoryCard->DeleteCard(PATCH_TO_CARD,card);
}

/**
 * \brief Удалить все карты
*/
void fnClearCards(uint8_t *mode) {
  //Очищаем карты
  Obj_MemoryCard->ClearCards(PATCH_TO_CARD);
}

/**
 * \brief Открыть дверь
*/
void fnOpenDoor(uint8_t *card, uint8_t *reader) {
  //Индикация открытия
  LedBlinker::LedBlinkerPulse(GREEN_LED);

  ////////////////////////Ответ///////////////////////////////
  char MSG[26];
  uint8_t size_msg = Obj_WebJsonLightSender.Event(MSG, 4, card, *reader, getTimeController(&Rtc));
  Obj_EthernetAdaptor.Post(MSG, size_msg, Ans, Ans_size_msg);
  /////////////////////////////////////////////////////////////////////////////////
}

/**
 * \brief Побочные действия на 2 факторную аунтификацию
*/
void fnCheckAcess(uint8_t *confirm) {
  fnOpenDoor(Card, &Reader);
}
