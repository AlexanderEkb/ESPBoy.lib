/**
 * @file ESPBoyCore.cpp
 * \brief
 * The ESPBoyCore class for ESPboy hardware initilization and control.
 */

#include "ESPBoyCore.h"

#define RAW_BUTTON_A 				0x0001
#define RAW_BUTTON_B 				0x0002
#define RAW_BUTTON_C				0x0003
#define RAW_BUTTON_SELECT		    0x0004
#define	RAW_BUTTON_START		    0x0008
#define RAW_BUTTON_UP				0x0010
#define RAW_BUTTON_DOWN			    0x0020
#define RAW_BUTTON_LEFT			    0x0040
#define RAW_BUTTON_RIGHT		    0x0080
uint8_t externalButtons;		
void (*externalButtonsHandler)();
bool hasExternalButtonsHandler = false;

uint16_t ESPBoyCore::theBorderColor;

ESPBoyCore::ESPBoyCore() { 
	theBorderColor = ST77XX_GREEN;
}

void ESPBoyCore::blank() {

}

void ESPBoyCore::boot()
{
	bootPins();
	bootOLED();
	bootPowerSaving();
	
	// To flush the shifter...
	buttonsState();
}

// Pins are set to the proper modes and levels for the specific hardware.
// This routine must be modified if any pins are moved to a different port
void ESPBoyCore::bootPins()
{
	pinMode(4, OUTPUT);     // Initialize the LED_BUILTIN pin as an output
	digitalWrite(4, LOW);
	pinMode(PIN_SPEAKER, OUTPUT);		// Initialize speaker pin as output
	digitalWrite(PIN_SPEAKER, LOW);
}

void ESPBoyCore::bootOLED()
{
  SPI.begin();

  theDisplay.initR(INITR_BLACKTAB);
  theDisplay.setRotation(1);

  int16_t const width = theDisplay.width();
  int16_t const height = theDisplay.height();
  theDisplay.fillRect(0, 0, width, height, theBorderColor);

//  display(); // blank the display (sBuffer is global, so cleared automatically)  
}


void ESPBoyCore::LCDDataMode()
{
}

void ESPBoyCore::LCDCommandMode()
{
}


// Initialize the SPI interface for the display
void ESPBoyCore::bootSPI()
{
}


// Write to the SPI bus (MOSI pin)
void ESPBoyCore::SPItransfer(uint8_t data)
{
}

void ESPBoyCore::safeMode()
{
}



/* Power Management */

void ESPBoyCore::idle()
{
}

void ESPBoyCore::bootPowerSaving()
{
  // FIXME
}


// Shut down the display
void ESPBoyCore::displayOff()
{
	// TODO: Implement
}


// Restart the display after a displayOff()
void ESPBoyCore::displayOn()
{
	// TODO: Implement
}

uint8_t ESPBoyCore::width() { return WIDTH; }

uint8_t ESPBoyCore::height() { return HEIGHT; }

uint16_t ESPBoyCore::borderColor() { return theBorderColor; }


/* Drawing */

void ESPBoyCore::sendLCDCommand(uint8_t command)
{
	// TODO: Implement
}

// invert the display or set to normal
// when inverted, a pixel set to 0 will be on
void ESPBoyCore::invert(bool inverse)
{		
	// TODO: Implement
}

// turn all display pixels on, ignoring buffer contents
// or set to normal buffer display
void ESPBoyCore::allPixelsOn(bool on)
{	
	// TODO: Implement
}

// flip the display vertically or set to normal
void ESPBoyCore::flipVertical(bool flipped)
{	
	// TODO: Implement
}

// flip the display horizontally or set to normal
void ESPBoyCore::flipHorizontal(bool flipped)
{	
	// TODO: Implement
}

/* RGB LED */

void ESPBoyCore::setRGBled(uint8_t red, uint8_t green, uint8_t blue)
{
	// TODO: Implement
}

void ESPBoyCore::setRGBled(uint8_t color, uint8_t val)
{
	// TODO: Implement

}

void ESPBoyCore::freeRGBled()
{
	// TODO: Implement
}

void ESPBoyCore::digitalWriteRGB(uint8_t red, uint8_t green, uint8_t blue)
{
	// TODO: Implement
}

void ESPBoyCore::digitalWriteRGB(uint8_t color, uint8_t val)
{
	// TODO: Implement
}

/* Buttons */

void ESPBoyCore::setExternalButtons(uint8_t but) {
	externalButtons	= but;
}

void ESPBoyCore::setExternalButtonsHandler(void (*function)())
{
	externalButtonsHandler = function;
	hasExternalButtonsHandler = true;
}

uint8_t ESPBoyCore::buttonsState()
{

	uint8_t buttons = 0;
	digitalWrite(4, HIGH);
	uint8_t rawButtons = SPI.transfer(0xFF);
	digitalWrite(4, LOW);

	if(rawButtons & 0x20)
		buttons |= UP_BUTTON;
	if(rawButtons & 0x10)
		buttons |= DOWN_BUTTON;
	if(rawButtons & 0x40)
		buttons |= LEFT_BUTTON;
	if(rawButtons & 0x08)
		buttons |= RIGHT_BUTTON;
	if(rawButtons & 0x04)
		buttons |= A_BUTTON;
	if(rawButtons & 0x02)
		buttons |= B_BUTTON;

	// in this callback function the externalButtons should be updated#
	if (hasExternalButtonsHandler)
		(*externalButtonsHandler)();
	
  return buttons;
}

// delay in ms with 16 bit duration
void ESPBoyCore::delayShort(uint16_t ms)
{
  delay((unsigned long) ms);
}

void ESPBoyCore::exitToBootloader()
{
	// TODO: Implement
}

// Replacement main() that eliminates the USB stack code.
// Used by the ESPBOY_NO_USB macro. This should not be called
// directly from a sketch.

void ESPBoyCore::mainNoUSB()
{
  for (;;) {
    loop();
//    if (serialEventRun) serialEventRun();
  }
}

