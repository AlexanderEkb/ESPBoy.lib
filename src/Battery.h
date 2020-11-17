/******************************************************************************
 * Battery monitor. 
 */

#include <stdint.h>
#include <stdio.h>
#include "Arduino.h"
#include "Battery.inc"

class Battery {
  public:
    static uint32_t getState() {return state;};
    static void refresh() {
        const uint32_t rawValue = analogRead(A0);
        const uint32_t rawVoltage = (rawValue * 1000) / 1024;
        const uint32_t batteryVoltage = (rawVoltage * (r1 + r2)) / r2;
        uint16_t buffer[BATTERY_GLYPH_WIDTH * BATTERY_GLYPH_HEIGHT];

        if(batteryVoltage < fullDischarge) {
            state = 0;
        } else if(batteryVoltage < fullCharge) {
            const uint32 range = fullCharge - fullDischarge;
            const uint32_t delta = batteryVoltage - fullDischarge;
            state = (delta * 100) / range;
        } else {
            state = 100;
        }
    };
    static void Draw(uint16_t borderColor) {
        size_t size = BATTERY_GLYPH_WIDTH * BATTERY_GLYPH_HEIGHT;
        uint16_t buffer[size];
        uint16_t * pGlyph;
        if(state > 80) {
            pGlyph = (uint16_t *)Battery100;
        } else if(state > 60) {
            pGlyph = (uint16_t *)Battery80;
        } else if(state > 40) {
            pGlyph = (uint16_t *)Battery60;
        } else if(state > 20) {
            pGlyph = (uint16_t *)Battery40;
        } else {
            pGlyph = (uint16_t *)Battery20;
        }
        memcpy(buffer, pGlyph, size);
        for(uint32_t nIndex=0; nIndex<size; nIndex++) {
            if(buffer[nIndex] == BATTERY_TRANSPARENT_COLOR) {
                buffer[nIndex] = borderColor;
            }
        }
        theDisplay.drawRGBBitmap(
            BATTERY_GLYPH_X,
            BATTERY_GLYPH_Y,
            buffer,
            BATTERY_GLYPH_WIDTH,
            BATTERY_GLYPH_HEIGHT
        );
    }
  private:
    static const uint32_t r1 = 56000;
    static const uint32_t r2 = 15000;
    static const uint32_t fullCharge = 4200;
    static const uint32_t fullDischarge = 3500;
    static uint32_t state;
};