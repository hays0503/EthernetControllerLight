

#ifndef EthernetAdaptor_H
#define EthernetAdaptor_H

#include <Ethernet2.h>

class EthernetAdaptor
{
public:
  /**
    \brief Отправить на url
    \return Результат отправки сообщение http code
  */
  uint8_t SendTo();

// private:
  IPAddress pMyIp;
  IPAddress pMyDns;
  IPAddress pMyMask;
  uint8_t MyMac[6];

  IPAddress pClientIp;
  int pClientPort;
  EthernetClient client;

  char strIpClient[15];

  /**
   * \brief Отправка пакета post
   *
   * \param Body Строка c данными
   * \param size Размер сообщения
   * \param Answer Строка c данными ответа
   * \param Answer_size Размер сообщения ответа
   */
  int8_t Post(const char* Body,uint8_t size,char*& Answer,uint16_t& Answer_size);

  /**
   * \brief Конфигурирование TCP\Ip устройства
   *
   * \param IpCurrent Ip устройства
   * \param Dns Dns устройства
   * \param Mac Mac устройства
   * \param Mask Mask подсети устройства
   */
  uint8_t ConfigTCPip(
    const char *IpCurrent,
    const char *Dns,
    const char *Mask,
    const char *Mac
    );

  /**
   * \brief Конфигурирование клиента
   *
   * \param IpClient Ip устройства куда будут отсылаться пакеты
   */
  uint8_t ConfigClient(const char *IpClient, const char *Port);
};

#endif // EthernetAdaptor
