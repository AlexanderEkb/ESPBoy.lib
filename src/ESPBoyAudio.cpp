/**
 * @file ESPBoyAudio.cpp
 * \brief
 * The ESPBoyAudio class for speaker and sound control.
 */

#include "ESPBoy.h"
#include "ESPBoyAudio.h"
#include "Storage.h"

bool ESPBoyAudio::audio_enabled = false;

void ESPBoyAudio::on()
{
  audio_enabled = true;
}

void ESPBoyAudio::off()
{
  audio_enabled = false;
  // shut off audio pins by setting them as inputs
  // FIXME: Reset audio output
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
  Storage.update(EEPROM_AUDIO_ON_OFF, audio_enabled);
}

void ESPBoyAudio::begin()
{
  uint8_t state = Storage.read(EEPROM_AUDIO_ON_OFF);
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
