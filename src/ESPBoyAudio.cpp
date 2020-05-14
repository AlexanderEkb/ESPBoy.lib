/**
 * @file ESPBoyAudio.cpp
 * \brief
 * The ESPBoyAudio class for speaker and sound control.
 */

#include "ESPBoy.h"
#include "ESPBoyAudio.h"

bool ESPBoyAudio::audio_enabled = false;

void ESPBoyAudio::on()
{
  audio_enabled = true;
}

void ESPBoyAudio::off()
{
  audio_enabled = false;
  // shut off audio pins by setting them as inputs
#ifndef ESP8266  
#ifdef ESPBOY_10
  bitClear(SPEAKER_1_DDR, SPEAKER_1_BIT);
  bitClear(SPEAKER_2_DDR, SPEAKER_2_BIT);
#else
  bitClear(SPEAKER_1_DDR, SPEAKER_1_BIT);
#endif
#endif
}

void ESPBoyAudio::toggle()
{
  if (audio_enabled)
    off();
  else
    on();
}

void ESPBoyAudio::saveOnOff()
{
  EEPROM.begin(EEPROM_AUDIO_ON_OFF);
  EEPROM.update(EEPROM_AUDIO_ON_OFF, audio_enabled);
  EEPROM.end();
}

void ESPBoyAudio::begin()
{
  EEPROM.begin(EEPROM_AUDIO_ON_OFF);
  uint8_t state = EEPROM.read(EEPROM_AUDIO_ON_OFF);
  EEPROM.end();
  if (state) {
    if (true)
      on();
    else
      off();
  }
}

bool ESPBoyAudio::enabled()
{
  return audio_enabled;
}
