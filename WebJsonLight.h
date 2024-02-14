#ifndef WEBJSONLIGHT_H
#define WEBJSONLIGHT_H

// #include <stdio.h>
// #include <string.h>
// #include <inttypes.h>

class WebJsonLightSender
{
public:
  // PO,1234,A0,M0 - Power_on,sn,active,mode - len 14
  uint8_t Power_On(char *Power_On_Msg, uint16_t sn, uint8_t active, uint8_t mode)
  {
    snprintf(
        Power_On_Msg,
        14,
        "PO,%d,%s,%s",
        sn,
        (active == 0 ? "A0" : "A1"),
        (mode == 0 ? "M0" : "M1"));
    return 13;
  };

  // CA,FFFAAA,RE0 - Check_access,Card,Reader - len 14
  uint8_t Check_access(char *Check_access_Msg, const uint8_t card[6], uint8_t reader)
  {
    snprintf(
        Check_access_Msg,
        14,
        "CA,%02X%02X%02X,%s",
        card[3], card[2], card[1],
        (reader == 0 ? "RE0" : "RE1"));
    return 13;
  };

  // P,A0,M0 - Ping,active,mode - len 8
  uint8_t Ping(char *Ping_Msg, uint8_t active, uint8_t mode)
  {
    snprintf(
        Ping_Msg,
        8,
        "P,%s,%s",
        (active == 0 ? "A0" : "A1"),
        (mode == 0 ? "M0" : "M1"));
    return 7;
  };

  // E,4,FFFAAA,RE0,1706502846 - Event,Event_code,card,reader,Time(Unix Epoch) - len 25
  uint8_t Event(char *Event_Msg, uint8_t Event_code, const uint8_t card[6], uint8_t reader, uint32_t time)
  {
    snprintf(
        Event_Msg,
        26,
        "E,%d,%02X%02X%02X,%s,%" SCNd32,
        Event_code,
        card[3], card[2], card[1],
        (reader == 0 ? "RE0" : "RE1"),
        time);
    return 25;
  };
};

typedef void (*CB_Set_Active)(uint8_t *active, uint8_t *online);
typedef void (*CB_Set_Mode)(uint8_t *mode);
typedef void (*CB_Set_Time)(uint32_t *time);
typedef void (*CB_Add_Cards)(char *card);
typedef void (*CB_Del_Cards)(char *card);
typedef void (*CB_Clear_Cards)(uint8_t *clear_card_flag);
typedef void (*CB_Check_Access)(uint8_t* card);
typedef void (*CB_Open_Door)(uint8_t* card, uint8_t* reader);

class WebJsonLightReceive
{
public:
  
  /**
    \brief Поиск и сверка по команде с последующим вызом
    функции обратного вызова

    \param Msg  
  */
  bool Search(
      char *Msg,
      CB_Set_Active cbSet_Active = nullptr,
      CB_Set_Mode cbSet_Mode = nullptr,
      CB_Set_Time cbSet_Time = nullptr,
      CB_Add_Cards cbAdd_Cards = nullptr,
      CB_Del_Cards cbDel_Cards = nullptr,
      CB_Clear_Cards cbClear_Cards = nullptr,
      CB_Check_Access cbCheck_Access = nullptr,
      CB_Open_Door cbOpen_Door = nullptr)
  {
    switch (Msg[0])
    {
    case 'S':
      switch (Msg[1])
      {
      case 'A': // Command (SA) - SET_ACTIVE
        if (cbSet_Active != nullptr)
        {
          Serial.println("Command (SA) - SET_ACTIVE");
          uint8_t active, online;
          if (Set_Active(Msg, &active, &online))
          {
            cbSet_Active(&active, &online);
            return true;
          }
        }
        break;

      case 'M': // Command (SM) - SET_MODE
        if (cbSet_Mode != nullptr)
        {
          Serial.println("Command (SM) - SET_MODE");
          uint8_t mode;
          if (Set_Mode(Msg, &mode))
          {
            cbSet_Mode(&mode);
            return true;
          }
        }
        break;

      case 'T': // Command (ST) - SET_TIME
        if (cbSet_Time != nullptr)
        {
          Serial.println("Command (ST) - SET_TIME");
          uint32_t time;
          if (Set_Time(Msg, &time))
          {
            cbSet_Time(&time);
            return true;
          }
        }
        break;
      }
      break;

    case 'O': // Command (OD) - OPEN_DOOR
      if (Msg[1] == 'D' && cbOpen_Door != nullptr)
      {
        Serial.println("Command (OD) - OPEN_DOOR");
        uint8_t direction;
        if (Open_Door(Msg, &direction))
        {
          cbOpen_Door(0,&direction);
          return true;
        }
      }
      break;

    case 'A': // Command (AC) - ADD_CARD
      if (Msg[1] == 'C' && cbAdd_Cards != nullptr)
      {
        Serial.println("Command (AC) - ADD_CARD");
        char card[6];
        if (Add_Cards(Msg, card))
        {
          cbAdd_Cards(card);
          return true;
        }
      }
      break;

    case 'D': // Command (DC) - DEL_CARD
      if (Msg[1] == 'C' && cbDel_Cards != nullptr)
      {
        Serial.println("Command (DC) - DEL_CARD");
        char card[6];
        if (Del_Cards(Msg, card))
        {
          cbDel_Cards(card);
          return true;
        }
      }
      break;

    case 'C':
      switch (Msg[1])
      {
      case 'C': // Command (CC) - CLEAR_CARDS
        if (cbClear_Cards != nullptr)
        {
          Serial.println("Command (CC) - CLEAR_CARDS");
          uint8_t Confirm;
          if (Clear_Cards(Msg, &Confirm))
          {
            cbClear_Cards(&Confirm);
            return true;
          }
        }
        break;

      case 'A': // Command (CA) - CHECK_ACCESS
        if (cbCheck_Access != nullptr)
        {
          Serial.println("Command (CA) - CHECK_ACCESS");
          uint8_t Confirm;
          if (Check_Access(Msg, &Confirm))
          {
            cbCheck_Access(&Confirm);
            return true;
          }
        }
        break;
      }
      break;
    }

    return false;
  }

  // SA,A1,O0 - Set_Active,Active,Online - len 10
  bool Set_Active(char *Set_Active_Msg, uint8_t *active, uint8_t *online)
  {
    int ta, to;
    // если данные не удалось присвоить,
    if (sscanf(Set_Active_Msg, "SA,A%d,O%d", &ta, &to) != 2)
    {
      return false;
    }
    *active = ta;
    *online = to;
    return true;
  }

  // OD,D1 - direction - len 5
  bool Open_Door(char *Open_Door_Msg, uint8_t *direction)
  {
    int td;
    // если данные не удалось присвоить,
    if (sscanf(Open_Door_Msg, "OD,D%d", &td) != 1)
    {
      return false;
    }
    *direction=td;
    return true;
  }

  // SM,M1 - direction - len 5
  bool Set_Mode(char *Set_Mode_Msg, uint8_t *mode)
  {
    int mt;
    // если данные не удалось присвоить,
    if (sscanf(Set_Mode_Msg, "SM,M%d", &mt) != 1)
    {
      return false;
    }
    *mode = mt;
    return true;
  }

  // ST,1706858575 - len 15
  bool Set_Time(char *Set_Time_Msg, uint32_t *time)
  {
    // если данные не удалось присвоить,
    if (sscanf(Set_Time_Msg, "ST,%" SCNd32, time) != 1)
    {
      return false;
    }
    return true;
  }

  // AC,FFFAAA - len 10
  bool Add_Cards(char *Add_Cards_Msg, char *card)
  {
    // если данные не удалось присвоить,
    if (sscanf(Add_Cards_Msg, "AC,%6s", card) != 1)
    {
      return false;
    }
    return true;
  }

  // DC,FFFAAA - len 10
  bool Del_Cards(char *Del_Cards_Msg, char *card)
  {
    // если данные не удалось присвоить,
    if (sscanf(Del_Cards_Msg, "DC,%6s", card) != 1)
    {
      return false;
    }
    return true;
  }

  // CC,1 - len 5
  bool Clear_Cards(char *Clear_Cards_Msg, uint8_t *clear_card_flag)
  {
    int cc_t;
    // если данные не удалось присвоить,
    if (sscanf(Clear_Cards_Msg, "CC,%d", &cc_t) != 1)
    {
      return false;
    }
    *clear_card_flag=cc_t;
    return true;
  }

  bool Check_Access(char *Check_Access_Msg, uint8_t *Check_Access_flag)
  {
    int ca_t;
    // если данные не удалось присвоить,
    if (sscanf(Check_Access_Msg, "CA,%d", &ca_t) != 1)
    {
      return false;
    }
    *Check_Access_flag=ca_t;
    return true;
  }
};
#endif
