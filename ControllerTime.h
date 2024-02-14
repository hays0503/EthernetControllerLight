#ifndef ControllerTime_H

#define ControllerTime_H

#include <ThreeWire.h>                           
#include <RtcDS1302.h>



void setTimeController(RtcDS1302<ThreeWire>* Rtc,uint32_t* time)
{
    //Serial.print("SetTime: ");
    ////Serial.println(*time);
    Rtc->SetDateTime(RtcDateTime(*time-c_UnixEpoch32));
    //Serial.print("TimeInControlle: ");
    ////Serial.println(Rtc->GetDateTime().Epoch32Time());
}

uint32_t getTimeController(RtcDS1302<ThreeWire>* Rtc)
{
    return Rtc->GetDateTime().Epoch32Time();
}

#endif
