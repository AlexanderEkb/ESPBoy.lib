/**
 * @file ESPBoyBeep.cpp
 * \brief
 * Classes to generate simple square wave tones on the ESPBoy speaker pins.
 */

#include <Arduino.h>
#include "ESPBoyBeep.h"
#include "ESPBoyCore.h"

uint8_t BeepPin1::duration = 0;

void BeepPin1::begin()
{
  // FIXME:
//  TCCR3A = 0;
//  TCCR3B = (bit(WGM32) | bit(CS31)); // CTC mode. Divide by 8 clock prescale
}

void BeepPin1::tone(uint16_t count)
{
  tone(count, 0);
}

void BeepPin1::tone(uint16_t count, uint8_t dur)
{
	::tone(PIN_SPEAKER, count, dur);
}

void BeepPin1::timer()
{
  // FIXME:
}

void BeepPin1::noTone()
{
	::noTone(PIN_SPEAKER);
}


// Speaker pin 2, Timer 4A, Port C bit 7, Arduino pin 13

uint8_t BeepPin2::duration = 0;

void BeepPin2::begin()
{
  // FIXME:
}

void BeepPin2::tone(uint16_t count)
{
	::tone(PIN_SPEAKER, count);
}

void BeepPin2::tone(uint16_t count, uint8_t dur)
{
	::tone(PIN_SPEAKER, count, dur);
}

void BeepPin2::timer()
{
  // FIXME:
}

void BeepPin2::noTone()
{
	::noTone(PIN_SPEAKER);
}