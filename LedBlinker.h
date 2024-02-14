// #include <stdint.h>

#ifndef LEDBLINKER_H

#define LEDBLINKER_H

class LedBlinker
{
  public:

  static void LedBlinkerPulse(uint8_t PinLed)
  {
    digitalWrite(PinLed, HIGH);  // turn the LED on (HIGH is the voltage level)
    delay(100);                      // wait for a second
    digitalWrite(PinLed, LOW);   // turn the LED off by making the voltage LOW
    delay(100);                      // wait for a second
    digitalWrite(PinLed, HIGH);  // turn the LED on (HIGH is the voltage level)
    delay(100);                      // wait for a second
    digitalWrite(PinLed, LOW);   // turn the LED off by making the voltage LOW
    delay(100);                      // wait for a second
    digitalWrite(PinLed, HIGH);  // turn the LED on (HIGH is the voltage level)
    delay(100);                      // wait for a second
    digitalWrite(PinLed, LOW);   // turn the LED off by making the voltage LOW
    delay(100); 
  };

    static void LedBlinkerStrobe(uint8_t PinLed)
  {
    digitalWrite(PinLed, HIGH);  // turn the LED on (HIGH is the voltage level)
    delay(3000);                      // wait for a second
    digitalWrite(PinLed, LOW);   // turn the LED off by making the voltage LOW
    delay(100);                      // wait for a second    
  };

  static void LedBlinkerSOS(uint8_t PinLed)
  {
    //S
    for(uint8_t i=0;i<3;i++){
      digitalWrite(PinLed, HIGH);  
      delay(150);                     
      digitalWrite(PinLed, LOW);   
      delay(150);                     
    }
    delay(500);  
    //O
    for(uint8_t i=0;i<3;i++){
      digitalWrite(PinLed, HIGH);  
      delay(500);                     
      digitalWrite(PinLed, LOW);   
      delay(500);                     
    }
    delay(150);        
    //S
    for(uint8_t i=0;i<3;i++){
      digitalWrite(PinLed, HIGH);  
      delay(150);                     
      digitalWrite(PinLed, LOW);   
      delay(150);                     
    }
    delay(500);                  
  };



};

#endif