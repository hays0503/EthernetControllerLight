#include <OneWire.h>

class Matrix
{
  public:
    /**
    * Pin - выход считывателя 
    */

    Matrix(uint8_t Pin);
    bool Read();
    void getCard(uint8_t* Card);    
    static bool Compare(uint8_t* CardA,uint8_t* CardB);

    uint8_t address[8];    // буфер приема
    uint8_t Pin;
  
  private:
    OneWire* ds1990; // выход считывателя
    

};

Matrix::Matrix(uint8_t Pin):Pin(Pin)
{
  ds1990 = new OneWire(Pin);
};

bool Matrix::Compare(uint8_t* CardA,uint8_t* CardB)
{
  for (uint8_t i = 0; i < 8; i++)
  {
    if(CardA[i]!=CardB[i])
    {
      return false;
    }
  }
  return true;
};

bool Matrix::Read()
{
  if (ds1990->reset())             // Проверяем, есть ли на линии устройство iButton
  {
    ds1990->write(0x33);           // отправляем команду "считать ROM"
    delay(1);                     // Задержка,для подготовки приёма данных
    for (uint8_t i = 0; i < 8; i++)   // Цикл для считывания данных в массив
    {
      address[i] = ds1990->read(); // считываем
    }
    delay(500);
  }
  if(address[0]==1)
  {
    return true;
  }
  return false;
};

void Matrix::getCard(uint8_t* Card)
{

  for (uint8_t i = 0; i < 8; i++)// Цикл для вывода 8 байт в терминал
  {
    Card[i] = address[i];  // Выводим данные из массива в формате HEX
  }
  //Очищаем факт приложения карты 
  address[0] = 0;
}
