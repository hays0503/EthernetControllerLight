#include "EthernetAdaptor.h"
#include "Check_memory.h"

uint8_t EthernetAdaptor::SendTo() {
  return 0;
};

uint8_t fromHexChar(char c) {
  uint8_t result = 0;
  if ((c >= '0') && (c <= '9')) {
    result = c - '0';
  } else if ((c >= 'A') && (c <= 'F')) {
    result = 10 + c - 'A';
  } else if ((c >= 'a') && (c <= 'f')) {
    result = 10 + c - 'a';
  }
  return result;
};

void StrToMac(const char *macStr, uint8_t macArray[]) {

  for (size_t i = 0; i < 6; ++i) {
    // Пропускаем двоеточие
    while (*macStr == ':') {
      ++macStr;
    }

    // Читаем два символа и преобразуем их в байт
    macArray[i] = (fromHexChar(macStr[0]) << 4) | fromHexChar(macStr[1]);

    // Переходим к следующему блоку
    macStr += 2;
  }

};

void StrToIpAddress(const char *Ip, IPAddress &pIp) {
  int StrLen = strlen(Ip);
  uint8_t number = 0;
  uint8_t place_number = 1;
  uint8_t NumberOctet = 0;
  uint8_t Octet[4] = { 0 };

  for (int i = StrLen - 1; i >= 0; --i) {
    char currentChar = Ip[i];

    if (currentChar == '.') {
      Octet[NumberOctet] = number;
      number = 0;
      place_number = 1;
      ++NumberOctet;
    } else {
      number += (currentChar - '0') * place_number;
      place_number *= 10;
    }
    if (i == 0) {
      Octet[NumberOctet] = number;
    }
  }


  pIp[0] = Octet[3];
  pIp[1] = Octet[2];
  pIp[2] = Octet[1];
  pIp[3] = Octet[0];
};

uint8_t EthernetAdaptor::ConfigTCPip(
  const char *IpCurrent,
  const char *Dns,
  const char *Mask,
  const char *Mac) {


  Serial.println(F("+++Set Ip+++"));
  StrToIpAddress(IpCurrent, this->pMyIp);

   Serial.println(F("+++Set Dns+++"));
  StrToIpAddress(Dns, this->pMyDns);

   Serial.println(F("+++Set Mask+++"));
  StrToIpAddress(Mask, this->pMyMask);

   Serial.println(F("+++Set Mac+++"));
  StrToMac(Mac, MyMac);

   Serial.println(F("+++Set Ethernet.begin+++"));

  Ethernet.begin(
    MyMac,
    pMyIp,
    pMyDns,
    pMyMask);

   delay(1000);
   Serial.print(F("My IP address: "));
   Serial.println(Ethernet.localIP());
  
   Serial.print(F("My gateway IP: "));
   Serial.println(Ethernet.gatewayIP());
  
   Serial.print(F("My dns Server IP: "));
   Serial.println(Ethernet.dnsServerIP());
  
   Serial.print(F("My subnet Mask: "));
   Serial.println(Ethernet.subnetMask());
  
   // give the Ethernet shield a second to initialize:
   Serial.println(F("Ethernet shield initialize......."));


  return 0;
};

uint8_t EthernetAdaptor::ConfigClient(const char *IpClient, const char *Port) {
  // uint8_t sizeIp = strlen(IpClient);
  // for (uint8_t i = 0; i < sizeIp; i++) {
  //   strIpClient[i] = IpClient[i];
  // }

  StrToIpAddress(IpClient, this->pClientIp);

  this->pClientPort = String(Port).toInt();

   Serial.print(F("IpClient: "));
   Serial.println(IpClient);
   Serial.print(F("Client Port: "));
   Serial.println(pClientPort);

  return 0;
}

int8_t EthernetAdaptor::Post(const char *Body, uint8_t size, char *&Answer, uint16_t &Answer_size) {


  Serial.print(F("ID = 0 SPACE FREE = "));
  Serial.print(memoryFree());
  Serial.print('\n');


  if (!client.connect(this->pClientIp, this->pClientPort)) {
    Serial.println(F("Connection to client failed"));
    return -1;
  } else {

    ///////////////////////////////////////////////////////////////////////////////////////////
    client.println(F("GET / HTTP/1.0"));
    client.println(F("Host: 192.168.0.33"));
    client.print(F("Content-Length: "));
    client.println(size);
    ///////////////////////////////////////////////////////////////////////////////////////////

    ///////////////////////////////////////////////////////////////////////////////////////////
    // Отправляем Пакет
    client.print(F("Body: "));
    for (uint8_t sumbol = 0; sumbol < size; sumbol++) {
      Serial.print(Body[sumbol]);
      client.print(Body[sumbol]);
    }
    Serial.print('\n');
    client.print('\n');
    ///////////////////////////////////////////////////////////////////////////////////////////

    ///////////////////////////////////////////////////////////////////////////////////////////
    // Отправка закончена
    client.println(F("Connection: close"));
    if (client.println() == 0) {
      Serial.println(F("Failed to send request"));
      client.stop();
      return -1;
    }
    ///////////////////////////////////////////////////////////////////////////////////////////
    // Проверка статуса запроса

    Serial.print(F("ID = 1 SPACE FREE = "));
    Serial.print(memoryFree());
    Serial.print('\n');

    if(true)
    {
        char status[32] = { 0 };
        client.readBytesUntil('\r', status, sizeof(status));
        if (strcmp(status, "HTTP/1.1 200 OK") != 0) {
        Serial.print(F("Unexpected response: "));
        Serial.println(status);
        client.stop();
        return -1;
      }
    }

    Serial.print(F("ID = 2 SPACE FREE = "));
    Serial.print(memoryFree());
    Serial.print('\n');

    ///////////////////////////////////////////////////////////////////////////////////////////
    // Поиск размера сообщение
    if (!client.find("content-length: ")) {
      Serial.println(F("Invalid response no content-length"));
      client.stop();
      return -1;
    } else {
      char content_length[8] = { 0 };
      client.readBytesUntil('\r', content_length, sizeof(content_length));
      Answer_size = atoi(content_length);
      Serial.println(content_length);
      Serial.print(F("Content_length_number: "));
      Serial.println(Answer_size);
    }

    ///////////////////////////////////////////////////////////////////////////////////////////
    // Пропуск HTTP заголовка
    if (!client.find("\r\n\r\n")) {
      Serial.println(F("Invalid response"));
      client.stop();
      return -1;
    }
    ///////////////////////////////////////////////////////////////////////////////////////////
    //Читаем ответ
    Serial.print(F("ID = 3 SPACE FREE = "));
    Serial.print(memoryFree());
    Serial.print('\n');
    //Выделяем память под ответ
    Answer = new char[Answer_size+1];
    Serial.print(F("ID = 4 SPACE FREE = "));
    Serial.print(memoryFree());
    Serial.print('\n');
    uint16_t i = 0;
    Serial.print(F("ID = 5 SPACE FREE = "));
    Serial.print(memoryFree());
    Serial.print('\n');
    while (client.connected()) {
      if (client.available()) {
        char c = client.read();
        if(c!='\n'){
            Answer[i] = c;
            i++;
        }
        
      }
    }
    Serial.print(F("ID = 6 SPACE FREE = "));
    Serial.print(memoryFree());
    Serial.print('\n');
    Answer[Answer_size]='\0';
    ///////////////////////////////////////////////////////////////////////////////////////////
    Serial.print('\n');
    client.stop();
    delay(100);

    return 0;
  };
};
