/******************************************************************************
 * Battery monitor. 
 */

#include <stdint.h>
#include <stdio.h>
#include "Arduino.h"

class Battery {
  public:
    static uint32_t getState() {return state;};
    static void refresh() {
        const uint32_t rawValue = analogRead(A0);
        const uint32_t rawVoltage = (rawValue * 1000) / 1024;
        const uint32_t batteryVoltage = (rawVoltage * (r1 + r2)) / r2;

        if(batteryVoltage < fullDischarge) {
            state = 0;
        } else if(batteryVoltage < fullCharge) {
            const uint32 range = fullCharge - fullDischarge;
            const uint32_t delta = batteryVoltage - fullDischarge;
            state = (delta * 100) / range;
        } else {
            state = 100;
        }

        char buf[80];
        snprintf(buf, 80, "0x%x:%d:%d", rawValue, batteryVoltage, state);
        Serial.println(buf);
    };
  private:
    static const uint32_t r1 = 56000;
    static const uint32_t r2 = 15000;
    static const uint32_t fullCharge = 4200;
    static const uint32_t fullDischarge = 3500;
    static uint32_t state;
};