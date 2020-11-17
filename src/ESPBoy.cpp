/**
 * @file ESPBoy.cpp
 * \brief
 * The ESPBoyBase and ESPBoy classes and support objects and definitions.
 */

#include "ESPBoy.h"
#include "eb_logo.inc"
#include "glcdfont.c"

//========================================
//========== class ESPBoyBase ==========
//========================================

uint8_t ESPBoyBase::sBuffer[];
uint32_t Battery:: state;

ESPBoyBase::ESPBoyBase()
{
  currentButtonState = 0;
  previousButtonState = 0;
  // frame management
  setFrameDuration(16);
  frameCount = 0;
  justRendered = false;
}

// functions called here should be public so users can create their
// own init functions if they need different behavior than `begin`
// provides by default
void ESPBoyBase::begin()
{
	boot();
  flashlight(); // light the RGB LED and screen if UP button is being held.

  // check for and handle buttons held during start up for system control
  systemButtons(); 

  audio.begin();

  bootLogo();
  // alternative logo functions. Work the same as bootLogo() but may reduce
  // memory size if the sketch uses the same bitmap drawing function
//  bootLogoCompressed();
//  bootLogoSpritesSelfMasked();
//  bootLogoSpritesOverwrite();
//  bootLogoSpritesBSelfMasked();
//  bootLogoSpritesBOverwrite();

  // waitNoButtons(); // wait for all buttons to be release
}

void ESPBoyBase::flashlight()
{
  // FIXME:
}

void ESPBoyBase::systemButtons()
{
  // FIXME:
}

void ESPBoyBase::sysCtrlSound(uint8_t buttons, uint8_t led, uint8_t eeVal)
{
  // FIXME:
}

void ESPBoyBase::bootLogo()
{
  bootLogoShell(drawLogoBitmap);
}

void ESPBoyBase::drawLogoBitmap(int16_t y)
{
  drawBitmap(20, y, espboy_logo, 88, 16);
}

void ESPBoyBase::bootLogoCompressed()
{
  bootLogoShell(drawLogoCompressed);
}

void ESPBoyBase::drawLogoCompressed(int16_t y)
{
  drawCompressed(20, y, espboy_logo_compressed);
}

void ESPBoyBase::bootLogoSpritesSelfMasked()
{
  bootLogoShell(drawLogoSpritesSelfMasked);
}

void ESPBoyBase::drawLogoSpritesSelfMasked(int16_t y)
{
  Sprites::drawSelfMasked(20, y, espboy_logo_sprite, 0);
}

void ESPBoyBase::bootLogoSpritesOverwrite()
{
  bootLogoShell(drawLogoSpritesOverwrite);
}

void ESPBoyBase::drawLogoSpritesOverwrite(int16_t y)
{
  Sprites::drawOverwrite(20, y, espboy_logo_sprite, 0);
}

void ESPBoyBase::bootLogoSpritesBSelfMasked()
{
  bootLogoShell(drawLogoSpritesBSelfMasked);
}

void ESPBoyBase::drawLogoSpritesBSelfMasked(int16_t y)
{
  SpritesB::drawSelfMasked(20, y, espboy_logo_sprite, 0);
}

void ESPBoyBase::bootLogoSpritesBOverwrite()
{
  bootLogoShell(drawLogoSpritesBOverwrite);
}

void ESPBoyBase::drawLogoSpritesBOverwrite(int16_t y)
{
  SpritesB::drawOverwrite(20, y, espboy_logo_sprite, 0);
}

// bootLogoText() should be kept in sync with bootLogoShell()
// if changes are made to one, equivalent changes should be made to the other
void ESPBoyBase::bootLogoShell(void (*drawLogo)(int16_t))
{   
  bool showLEDs = readShowBootLogoLEDsFlag();
  bool showLogo = readShowBootLogoFlag();
  
  if (!showLogo) {
    return;
  }

  if (showLEDs) {
    digitalWriteRGB(RED_LED, RGB_ON);
  }

   for (int16_t y = -16; y <= 24; y++) {
    if (pressed(RIGHT_BUTTON)) {
      digitalWriteRGB(RGB_OFF, RGB_OFF, RGB_OFF); // all LEDs off
      return;
    }

    if (showLEDs && y == 4) {
      digitalWriteRGB(RED_LED, RGB_OFF);    // red LED off
      digitalWriteRGB(GREEN_LED, RGB_ON);   // green LED on
    }

    // Using display(CLEAR_BUFFER) instead of clear() may save code space.
    // The extra time it takes to repaint the previous logo isn't an issue.
    // display(CLEAR_BUFFER);
	clear();
    (*drawLogo)(y); // call the function that actually draws the logo
    display();      
    delayShort(15);
  }

  if (showLEDs) {
    digitalWriteRGB(GREEN_LED, RGB_OFF);  // green LED off
    digitalWriteRGB(BLUE_LED, RGB_ON);    // blue LED on
  }
  delayShort(400);
  digitalWriteRGB(BLUE_LED, RGB_OFF);

  bootLogoExtra();
}

// Virtual function overridden by derived class
void ESPBoyBase::bootLogoExtra() { }

// wait for all buttons to be released
void ESPBoyBase::waitNoButtons() {
  do {
    delayShort(50); // simple button debounce
  } while (buttonsState());
}

/* Frame management */

void ESPBoyBase::setFrameRate(uint8_t rate)
{
  eachFrameMillis = 1000 / rate;
}

void ESPBoyBase::setFrameDuration(uint8_t duration)
{
  eachFrameMillis = duration;
}

bool ESPBoyBase::everyXFrames(uint8_t frames)
{
  return frameCount % frames == 0;
}

bool ESPBoyBase::nextFrame()
{
  uint8_t now = (uint8_t) millis();
  uint8_t frameDurationMs = now - thisFrameStart;

  if (justRendered) {
    lastFrameDurationMs = frameDurationMs;
    justRendered = false;
    return false;
  }
  else if (frameDurationMs < eachFrameMillis) {
    // Only idle if at least a full millisecond remains, since idle() may
    // sleep the processor until the next millisecond timer interrupt.
    if (++frameDurationMs < eachFrameMillis) {
      idle();
    }

    return false;
  }

  // pre-render
  justRendered = true;
  thisFrameStart = now;
  frameCount++;

  return true;
}

bool ESPBoyBase::nextFrameDEV()
{
  // FIXME:
  return nextFrame();
}

int ESPBoyBase::cpuLoad()
{
  return lastFrameDurationMs*100 / eachFrameMillis;
}

void ESPBoyBase::initRandomSeed()
{
  // FIXME:
}

/* Graphics */

void ESPBoyBase::clear()
{
  fillScreen(BLACK);
}


// Used by drawPixel to help with left bitshifting since AVR has no
// multiple bit shift instruction.  We can bit shift from a lookup table
// in flash faster than we can calculate the bit shifts on the CPU.
const uint8_t bitshift_left[] PROGMEM = {
  _BV(0), _BV(1), _BV(2), _BV(3), _BV(4), _BV(5), _BV(6), _BV(7)
};

void ESPBoyBase::drawPixel(int16_t x, int16_t y, uint8_t color)
{
  #ifdef PIXEL_SAFE_MODE
  if (x < 0 || x > (WIDTH-1) || y < 0 || y > (HEIGHT-1))
  {
    return;
  }
  #endif

  uint8_t row = (uint8_t)y / 8;
  if (color)
  {
    sBuffer[(row*WIDTH) + (uint8_t)x] |=   _BV((uint8_t)y % 8);
  }
  else
  {
    sBuffer[(row*WIDTH) + (uint8_t)x] &= ~ _BV((uint8_t)y % 8);
  }
}

uint8_t ESPBoyBase::getPixel(uint8_t x, uint8_t y)
{
  uint8_t row = y / 8;
  uint8_t bit_position = y % 8;

  return (sBuffer[(row*WIDTH) + x] & _BV(bit_position)) >> bit_position;
}

void ESPBoyBase::drawCircle(int16_t x0, int16_t y0, uint8_t r, uint8_t color)
{
  int16_t f = 1 - r;
  int16_t ddF_x = 1;
  int16_t ddF_y = -2 * r;
  int16_t x = 0;
  int16_t y = r;

  drawPixel(x0, y0+r, color);
  drawPixel(x0, y0-r, color);
  drawPixel(x0+r, y0, color);
  drawPixel(x0-r, y0, color);

  while (x<y)
  {
    if (f >= 0)
    {
      y--;
      ddF_y += 2;
      f += ddF_y;
    }

    x++;
    ddF_x += 2;
    f += ddF_x;

    drawPixel(x0 + x, y0 + y, color);
    drawPixel(x0 - x, y0 + y, color);
    drawPixel(x0 + x, y0 - y, color);
    drawPixel(x0 - x, y0 - y, color);
    drawPixel(x0 + y, y0 + x, color);
    drawPixel(x0 - y, y0 + x, color);
    drawPixel(x0 + y, y0 - x, color);
    drawPixel(x0 - y, y0 - x, color);
  }
}

void ESPBoyBase::drawCircleHelper
(int16_t x0, int16_t y0, uint8_t r, uint8_t corners, uint8_t color)
{
  int16_t f = 1 - r;
  int16_t ddF_x = 1;
  int16_t ddF_y = -2 * r;
  int16_t x = 0;
  int16_t y = r;

  while (x<y)
  {
    if (f >= 0)
    {
      y--;
      ddF_y += 2;
      f += ddF_y;
    }

    x++;
    ddF_x += 2;
    f += ddF_x;

    if (corners & 0x4) // lower right
    {
      drawPixel(x0 + x, y0 + y, color);
      drawPixel(x0 + y, y0 + x, color);
    }
    if (corners & 0x2) // upper right
    {
      drawPixel(x0 + x, y0 - y, color);
      drawPixel(x0 + y, y0 - x, color);
    }
    if (corners & 0x8) // lower left
    {
      drawPixel(x0 - y, y0 + x, color);
      drawPixel(x0 - x, y0 + y, color);
    }
    if (corners & 0x1) // upper left
    {
      drawPixel(x0 - y, y0 - x, color);
      drawPixel(x0 - x, y0 - y, color);
    }
  }
}

void ESPBoyBase::fillCircle(int16_t x0, int16_t y0, uint8_t r, uint8_t color)
{
  drawFastVLine(x0, y0-r, 2*r+1, color);
  fillCircleHelper(x0, y0, r, 3, 0, color);
}

void ESPBoyBase::fillCircleHelper
(int16_t x0, int16_t y0, uint8_t r, uint8_t sides, int16_t delta,
 uint8_t color)
{
  int16_t f = 1 - r;
  int16_t ddF_x = 1;
  int16_t ddF_y = -2 * r;
  int16_t x = 0;
  int16_t y = r;

  while (x < y)
  {
    if (f >= 0)
    {
      y--;
      ddF_y += 2;
      f += ddF_y;
    }

    x++;
    ddF_x += 2;
    f += ddF_x;

    if (sides & 0x1) // right side
    {
      drawFastVLine(x0+x, y0-y, 2*y+1+delta, color);
      drawFastVLine(x0+y, y0-x, 2*x+1+delta, color);
    }

    if (sides & 0x2) // left side
    {
      drawFastVLine(x0-x, y0-y, 2*y+1+delta, color);
      drawFastVLine(x0-y, y0-x, 2*x+1+delta, color);
    }
  }
}

void ESPBoyBase::drawLine
(int16_t x0, int16_t y0, int16_t x1, int16_t y1, uint8_t color)
{
  // bresenham's algorithm - thx wikpedia
  bool steep = abs(y1 - y0) > abs(x1 - x0);
  if (steep) {
    swap(x0, y0);
    swap(x1, y1);
  }

  if (x0 > x1) {
    swap(x0, x1);
    swap(y0, y1);
  }

  int16_t dx, dy;
  dx = x1 - x0;
  dy = abs(y1 - y0);

  int16_t err = dx / 2;
  int8_t ystep;

  if (y0 < y1)
  {
    ystep = 1;
  }
  else
  {
    ystep = -1;
  }

  for (; x0 <= x1; x0++)
  {
    if (steep)
    {
      drawPixel(y0, x0, color);
    }
    else
    {
      drawPixel(x0, y0, color);
    }

    err -= dy;
    if (err < 0)
    {
      y0 += ystep;
      err += dx;
    }
  }
}

void ESPBoyBase::drawRect
(int16_t x, int16_t y, uint8_t w, uint8_t h, uint8_t color)
{
  drawFastHLine(x, y, w, color);
  drawFastHLine(x, y+h-1, w, color);
  drawFastVLine(x, y, h, color);
  drawFastVLine(x+w-1, y, h, color);
}

void ESPBoyBase::drawFastVLine
(int16_t x, int16_t y, uint8_t h, uint8_t color)
{
// this is called in fillRect, so it needs to be the fast version
//    const uint16_t fullColor = color?ST77XX_WHITE:ST77XX_BLUE;
//    theDisplay.drawFastVLine(x, y+32, h, fullColor);
  // FIXME:
  int16_t end = y+h;
  for (int16_t a = max(0,y); a < min(end,HEIGHT); a++)
  {
    drawPixel(x,a,color);
  }
}

void ESPBoyBase::drawFastHLine
(int16_t x, int16_t y, uint8_t w, uint8_t color)
{
  int16_t xEnd; // last x point + 1

  // Do y bounds checks
  if (y < 0 || y >= HEIGHT)
    return;

  xEnd = x + w;

  // Check if the entire line is not on the display
  if (xEnd <= 0 || x >= WIDTH)
    return;

  // Don't start before the left edge
  if (x < 0)
    x = 0;

  // Don't end past the right edge
  if (xEnd > WIDTH)
    xEnd = WIDTH;

  // calculate actual width (even if unchanged)
  w = xEnd - x;

  // buffer pointer plus row offset + x offset
  register uint8_t *pBuf = sBuffer + ((y / 8) * WIDTH) + x;   

  // pixel mask
  register uint8_t mask = 1 << (y & 7);

  switch (color)
  {
    case WHITE:
      while (w--)
      {
        *pBuf++ |= mask;
      }
      break;

    case BLACK:
      mask = ~mask;
      while (w--)
      {
        *pBuf++ &= mask;
      }
      break;
  }
}

void ESPBoyBase::fillRect
(int16_t x, int16_t y, uint8_t w, uint8_t h, uint8_t color)
{
  // stupidest version - update in subclasses if desired!
  for (int16_t i=x; i<x+w; i++)
  {
    drawFastVLine(i, y, h, color);
  }
}

void ESPBoyBase::fillScreen(uint8_t color)
{
    if (color == BLACK) {
        clearDisplay();
    } else {
        memset(sBuffer, 0xff, sBufferSize);
    }
}

void ESPBoyBase::drawRoundRect
(int16_t x, int16_t y, uint8_t w, uint8_t h, uint8_t r, uint8_t color)
{
  // smarter version
  drawFastHLine(x+r, y, w-2*r, color); // Top
  drawFastHLine(x+r, y+h-1, w-2*r, color); // Bottom
  drawFastVLine(x, y+r, h-2*r, color); // Left
  drawFastVLine(x+w-1, y+r, h-2*r, color); // Right
  // draw four corners
  drawCircleHelper(x+r, y+r, r, 1, color);
  drawCircleHelper(x+w-r-1, y+r, r, 2, color);
  drawCircleHelper(x+w-r-1, y+h-r-1, r, 4, color);
  drawCircleHelper(x+r, y+h-r-1, r, 8, color);
}

void ESPBoyBase::fillRoundRect
(int16_t x, int16_t y, uint8_t w, uint8_t h, uint8_t r, uint8_t color)
{
  // smarter version
  fillRect(x+r, y, w-2*r, h, color);

  // draw four corners
  fillCircleHelper(x+w-r-1, y+r, r, 1, h-2*r-1, color);
  fillCircleHelper(x+r, y+r, r, 2, h-2*r-1, color);
}

void ESPBoyBase::drawTriangle
(int16_t x0, int16_t y0, int16_t x1, int16_t y1, int16_t x2, int16_t y2, uint8_t color)
{
  drawLine(x0, y0, x1, y1, color);
  drawLine(x1, y1, x2, y2, color);
  drawLine(x2, y2, x0, y0, color);
}

void ESPBoyBase::fillTriangle
(int16_t x0, int16_t y0, int16_t x1, int16_t y1, int16_t x2, int16_t y2, uint8_t color)
{

  int16_t a, b, y, last;
  // Sort coordinates by Y order (y2 >= y1 >= y0)
  if (y0 > y1)
  {
    swap(y0, y1); swap(x0, x1);
  }
  if (y1 > y2)
  {
    swap(y2, y1); swap(x2, x1);
  }
  if (y0 > y1)
  {
    swap(y0, y1); swap(x0, x1);
  }

  if(y0 == y2)
  { // Handle awkward all-on-same-line case as its own thing
    a = b = x0;
    if(x1 < a)
    {
      a = x1;
    }
    else if(x1 > b)
    {
      b = x1;
    }
    if(x2 < a)
    {
      a = x2;
    }
    else if(x2 > b)
    {
      b = x2;
    }
    drawFastHLine(a, y0, b-a+1, color);
    return;
  }

  int16_t dx01 = x1 - x0,
      dy01 = y1 - y0,
      dx02 = x2 - x0,
      dy02 = y2 - y0,
      dx12 = x2 - x1,
      dy12 = y2 - y1,
      sa = 0,
      sb = 0;

  // For upper part of triangle, find scanline crossings for segments
  // 0-1 and 0-2.  If y1=y2 (flat-bottomed triangle), the scanline y1
  // is included here (and second loop will be skipped, avoiding a /0
  // error there), otherwise scanline y1 is skipped here and handled
  // in the second loop...which also avoids a /0 error here if y0=y1
  // (flat-topped triangle).
  if (y1 == y2)
  {
    last = y1;   // Include y1 scanline
  }
  else
  {
    last = y1-1; // Skip it
  }


  for(y = y0; y <= last; y++)
  {
    a   = x0 + sa / dy01;
    b   = x0 + sb / dy02;
    sa += dx01;
    sb += dx02;

    if(a > b)
    {
      swap(a,b);
    }

    drawFastHLine(a, y, b-a+1, color);
  }

  // For lower part of triangle, find scanline crossings for segments
  // 0-2 and 1-2.  This loop is skipped if y1=y2.
  sa = dx12 * (y - y1);
  sb = dx02 * (y - y0);

  for(; y <= y2; y++)
  {
    a   = x1 + sa / dy12;
    b   = x0 + sb / dy02;
    sa += dx12;
    sb += dx02;

    if(a > b)
    {
      swap(a,b);
    }

    drawFastHLine(a, y, b-a+1, color);
  }
}

void ESPBoyBase::drawBitmap
(int16_t x, int16_t y, const uint8_t *bitmap, uint8_t w, uint8_t h,
 uint8_t color)
{
  // no need to draw at all if we're offscreen
  if (x+w < 0 || x > WIDTH-1 || y+h < 0 || y > HEIGHT-1)
    return;

  int yOffset = abs(y) % 8;
  int sRow = y / 8;
  if (y < 0) {
    sRow--;
    yOffset = 8 - yOffset;
  }
  int rows = h/8;
  if (h%8!=0) rows++;
  for (int a = 0; a < rows; a++) {
    int bRow = sRow + a;
    if (bRow > (HEIGHT/8)-1) break;
    if (bRow > -2) {
      for (int iCol = 0; iCol<w; iCol++) {
        if (iCol + x > (WIDTH-1)) break;
        if (iCol + x >= 0) {
          if (bRow >= 0) {    
            if (color == WHITE)
              sBuffer[(bRow*WIDTH) + x + iCol] |= pgm_read_byte(bitmap+(a*w)+iCol) << yOffset;
            else if (color == BLACK)
              sBuffer[(bRow*WIDTH) + x + iCol] &= ~(pgm_read_byte(bitmap+(a*w)+iCol) << yOffset);
            else
              sBuffer[(bRow*WIDTH) + x + iCol] ^= pgm_read_byte(bitmap+(a*w)+iCol) << yOffset;
          }
          if (yOffset && bRow<(HEIGHT/8)-1 && bRow > -2) {
            if (color == WHITE)
              sBuffer[((bRow+1)*WIDTH) + x + iCol] |= pgm_read_byte(bitmap+(a*w)+iCol) >> (8-yOffset);
            else if (color == BLACK)
              sBuffer[((bRow+1)*WIDTH) + x + iCol] &= ~(pgm_read_byte(bitmap+(a*w)+iCol) >> (8-yOffset));
            else
              sBuffer[((bRow+1)*WIDTH) + x + iCol] ^= pgm_read_byte(bitmap+(a*w)+iCol) >> (8-yOffset);
          }
        }
      }
    }
  }
}


void ESPBoyBase::drawSlowXYBitmap
(int16_t x, int16_t y, const uint8_t *bitmap, uint8_t w, uint8_t h, uint8_t color)
{
  // no need to draw at all of we're offscreen
  if (x+w < 0 || x > WIDTH-1 || y+h < 0 || y > HEIGHT-1)
    return;
  int16_t xi, yi, byteWidth = (w + 7) / 8;

  for(yi = 0; yi < h; yi++) {
    for(xi = 0; xi < w; xi++ ) {
      if(!(pgm_read_byte(bitmap + yi * byteWidth + xi / 8) & (128 >> (xi & 7)))) {      
        drawPixel(x + xi, y + yi, color);
      } else {
        drawPixel(x + xi, y + yi, !color);
			}
    }
  }
}


// Helper for drawCompressed()
struct BitStreamReader
{
  const uint8_t *source;
  uint16_t sourceIndex;
  uint8_t bitBuffer;
  uint8_t byteBuffer;

  BitStreamReader(const uint8_t *source)
    : source(source), sourceIndex(), bitBuffer(), byteBuffer()
  {
  }

  uint16_t readBits(uint16_t bitCount)
  {
    uint16_t result = 0;
    for (uint16_t i = 0; i < bitCount; i++)
    {
      if (this->bitBuffer == 0)
      {
        this->bitBuffer = 0x1;
        this->byteBuffer = pgm_read_byte(&this->source[this->sourceIndex]);
        ++this->sourceIndex;
      }

      if ((this->byteBuffer & this->bitBuffer) != 0)
        result |= (1 << i); // result |= bitshift_left[i];

      this->bitBuffer <<= 1;
    }
    return result;
  }
};

void ESPBoyBase::drawCompressed(int16_t sx, int16_t sy, const uint8_t *bitmap, uint8_t color)
{
  // set up decompress state
  BitStreamReader cs = BitStreamReader(bitmap);

  // read header
  int width = (int)cs.readBits(8) + 1;
  int height = (int)cs.readBits(8) + 1;
  uint8_t spanColour = (uint8_t)cs.readBits(1); // starting colour

  // no need to draw at all if we're offscreen
  if ((sx + width < 0) || (sx > WIDTH - 1) || (sy + height < 0) || (sy > HEIGHT - 1))
    return;

  // sy = sy - (frame * height);
  int yOffset = abs(sy) % 8;
  int startRow = sy / 8;
  if (sy < 0) {
    startRow--;
    yOffset = 8 - yOffset;
  }
  int rows = height / 8;
  if ((height % 8) != 0)
    ++rows;

  int rowOffset = 0; // +(frame*rows);
  int columnOffset = 0;

  uint8_t byte = 0x00;
  uint8_t bit = 0x01;
  while (rowOffset < rows) // + (frame*rows))
  {
    uint16_t bitLength = 1;
    while (cs.readBits(1) == 0)
      bitLength += 2;

    uint16_t len = cs.readBits(bitLength) + 1; // span length

    // draw the span
    for (uint16_t i = 0; i < len; ++i)
    {
      if (spanColour != 0)
        byte |= bit;
      bit <<= 1;

      if (bit == 0) // reached end of byte
      {
        // draw
        int bRow = startRow + rowOffset;

        //if (byte) // possible optimisation
        if ((bRow <= (HEIGHT / 8) - 1) && (bRow > -2) &&
            (columnOffset + sx <= (WIDTH - 1)) && (columnOffset + sx >= 0))
        {
          int16_t offset = (bRow * WIDTH) + sx + columnOffset;
          if (bRow >= 0)
          {
            int16_t index = offset;
            uint8_t value = byte << yOffset;
            if (color != 0)
              sBuffer[index] |= value;
            else
              sBuffer[index] &= ~value;
          }
          if ((yOffset != 0) && (bRow < (HEIGHT / 8) - 1))
          {
            int16_t index = offset + WIDTH;
            uint8_t value = byte >> (8 - yOffset);

            if (color != 0)
              sBuffer[index] |= value;
            else
              sBuffer[index] &= ~value;
          }
        }

        // iterate
        ++columnOffset;
        if (columnOffset >= width)
        {
          columnOffset = 0;
          ++rowOffset;
        }

        // reset byte
        byte = 0x00;
        bit = 0x01;
      }
    }

    spanColour ^= 0x01; // toggle colour bit (bit 0) for next span
  }
}

void ESPBoyBase::display()
{ 
  const uint32_t emulatedHeight = HEIGHT;
  const uint32_t linesPerStrip = 8;
  const uint32_t width = WIDTH;
  const size_t bufferSize = (width * linesPerStrip);
  uint16_t buffer[bufferSize];
  const uint32_t stripCount = emulatedHeight / linesPerStrip;
	uint32_t strip, column, line;
	uint32_t srcIndex, destIndex;
  const uint32_t orgX = (theDisplay.width() - width) >> 1;
  const uint32_t orgY = 24; //(theDisplay.height() - emulatedHeight) >> 1;
	
  /* Strip-per-strip drawing */
  for(strip=0; strip < stripCount; strip++) {
		for(column=0; column < width; column++) {
			srcIndex = (strip * width) + column;
			uint8_t byte = sBuffer[srcIndex];
			for(line=0; line < linesPerStrip; line++) {
				const bool set = byte & 0x80;
				destIndex = line * width + (width - column - 1);
				buffer[destIndex] = set?FOREGROUND_COLOR:BACKGROUND_COLOR;
				byte <<= 1;
			}
		}
		// const uint16_t y = (stripCount - strip) * linesPerStrip - 8; // + 24;
		const uint16_t y = (stripCount - strip) * linesPerStrip;
		theDisplay.drawRGBBitmap(orgX, orgY + y, buffer, width, linesPerStrip);
  }
}

void ESPBoyBase::display(bool clear)
{
  display();
  if(clear) {
    clearDisplay();
  }
}

uint8_t* ESPBoyBase::getBuffer()
{
  return sBuffer;
}

bool ESPBoyBase::pressed(uint8_t buttons)
{
  return (currentButtonState & buttons) == buttons;
}

bool ESPBoyBase::notPressed(uint8_t buttons)
{
  return (buttonsState() & buttons) == 0;
}

void ESPBoyBase::pollButtons()
{
  previousButtonState = currentButtonState;
  currentButtonState = buttonsState();
}

bool ESPBoyBase::justPressed(uint8_t button)
{
  return (!(previousButtonState & button) && (currentButtonState & button));
}

bool ESPBoyBase::justReleased(uint8_t button)
{
  return ((previousButtonState & button) && !(currentButtonState & button));
}

bool ESPBoyBase::collide(Point point, Rect rect)
{
  return ((point.x >= rect.x) && (point.x < rect.x + rect.width) &&
      (point.y >= rect.y) && (point.y < rect.y + rect.height));
}

bool ESPBoyBase::collide(Rect rect1, Rect rect2)
{
  return !(rect2.x                >= rect1.x + rect1.width  ||
           rect2.x + rect2.width  <= rect1.x                ||
           rect2.y                >= rect1.y + rect1.height ||
           rect2.y + rect2.height <= rect1.y);
}

uint16_t ESPBoyBase::readUnitID()
{
    EEPROM.begin(EEPROM_STORAGE_SPACE_START);
  uint8_t result =  EEPROM.read(EEPROM_UNIT_ID) |
         (((uint16_t)(EEPROM.read(EEPROM_UNIT_ID + 1))) << 8);
    EEPROM.end();
    return result;
}

void ESPBoyBase::writeUnitID(uint16_t id)
{
  EEPROM.begin(EEPROM_STORAGE_SPACE_START);
  EEPROM.update(EEPROM_UNIT_ID, (uint8_t)(id & 0xff));
  EEPROM.update(EEPROM_UNIT_ID + 1, (uint8_t)(id >> 8));
  EEPROM.end();
}

uint8_t ESPBoyBase::readUnitName(char* name)
{
  char val;
  uint8_t dest;
  uint8_t src = EEPROM_UNIT_NAME;

    EEPROM.begin(EEPROM_STORAGE_SPACE_START);
  for (dest = 0; dest < ESPBOY_UNIT_NAME_LEN; dest++)
  {
    val = EEPROM.read(src);
    name[dest] = val;
    src++;
    if (val == 0x00 || (byte)val == 0xFF) {
      break;
    }
  }
    EEPROM.end();

  name[dest] = 0x00;
  return dest;
}

void ESPBoyBase::writeUnitName(char* name)
{
  bool done = false;
  uint8_t dest = EEPROM_UNIT_NAME;

  EEPROM.begin(EEPROM_STORAGE_SPACE_START);
  for (uint8_t src = 0; src < ESPBOY_UNIT_NAME_LEN; src++)
  {
    if (name[src] == 0x00) {
      done = true;
    }
    // write character or 0 pad if finished
    EEPROM.update(dest, done ? 0x00 : name[src]);
    dest++;
  }
  EEPROM.end();
}

bool ESPBoyBase::readShowBootLogoFlag()
{
    EEPROM.begin(EEPROM_STORAGE_SPACE_START);
  uint8_t result = (EEPROM.read(EEPROM_SYS_FLAGS) & SYS_FLAG_SHOW_LOGO_MASK);
    EEPROM.end();
    
    return result;
}

void ESPBoyBase::writeShowBootLogoFlag(bool val)
{
    EEPROM.begin(EEPROM_STORAGE_SPACE_START);
  uint8_t flags = EEPROM.read(EEPROM_SYS_FLAGS);
    
  bitWrite(flags, SYS_FLAG_SHOW_LOGO, val);
  EEPROM.update(EEPROM_SYS_FLAGS, flags);
  EEPROM.end();
}

bool ESPBoyBase::readShowUnitNameFlag()
{
    EEPROM.begin(EEPROM_STORAGE_SPACE_START);
  uint8_t result = (EEPROM.read(EEPROM_SYS_FLAGS) & SYS_FLAG_UNAME_MASK);
    EEPROM.end();
    
    return result;
}

void ESPBoyBase::writeShowUnitNameFlag(bool val)
{
    EEPROM.begin(EEPROM_STORAGE_SPACE_START);
  uint8_t flags = EEPROM.read(EEPROM_SYS_FLAGS);
    
  bitWrite(flags, SYS_FLAG_UNAME, val);
  EEPROM.update(EEPROM_SYS_FLAGS, flags);
  EEPROM.end();
}

bool ESPBoyBase::readShowBootLogoLEDsFlag()
{
  EEPROM.begin(EEPROM_STORAGE_SPACE_START);
  uint8_t result = (EEPROM.read(EEPROM_SYS_FLAGS) & SYS_FLAG_SHOW_LOGO_LEDS_MASK);
    EEPROM.end();
    
    return result;
}

void ESPBoyBase::writeShowBootLogoLEDsFlag(bool val)
{
  EEPROM.begin(EEPROM_STORAGE_SPACE_START);
  uint8_t flags = EEPROM.read(EEPROM_SYS_FLAGS);
    
  bitWrite(flags, SYS_FLAG_SHOW_LOGO_LEDS, val);
  EEPROM.update(EEPROM_SYS_FLAGS, flags);
  EEPROM.end();
}

void ESPBoyBase::swap(int16_t& a, int16_t& b)
{
  int16_t temp = a;
  a = b;
  b = temp;
}


//====================================
//========== class ESPBoy ==========
//====================================

ESPBoy::ESPBoy()
{
  cursor_x = 0;
  cursor_y = 0;
  textColor = 1;
  textBackground = 0;
  textSize = 1;
  textWrap = 0;
}

// bootLogoText() should be kept in sync with bootLogoShell()
// if changes are made to one, equivalent changes should be made to the other
void ESPBoy::bootLogoText()
{
  bool showLEDs = readShowBootLogoLEDsFlag();

  if (!readShowBootLogoFlag()) {
    return;
  }

  if (showLEDs) {
    digitalWriteRGB(RED_LED, RGB_ON);
  }

  for (int16_t y = -16; y <= 24; y++) {
    if (pressed(RIGHT_BUTTON)) {
      digitalWriteRGB(RGB_OFF, RGB_OFF, RGB_OFF); // all LEDs off
      return;
    }

    if (showLEDs && y == 4) {
      digitalWriteRGB(RED_LED, RGB_OFF);    // red LED off
      digitalWriteRGB(GREEN_LED, RGB_ON);   // green LED on
    }

    // Using display(CLEAR_BUFFER) instead of clear() may save code space.
    // The extra time it takes to repaint the previous logo isn't an issue.
        
        
    display(CLEAR_BUFFER);
    cursor_x = 23;
    cursor_y = y;
    textSize = 2;
    print(F("ESPBOY"));
    textSize = 1;
    display();
    delayShort(11);
  }

  if (showLEDs) {
    digitalWriteRGB(GREEN_LED, RGB_OFF);  // green LED off
    digitalWriteRGB(BLUE_LED, RGB_ON);    // blue LED on
  }
  delayShort(400);
  digitalWriteRGB(BLUE_LED, RGB_OFF);

  bootLogoExtra();
}

void ESPBoy::bootLogoExtra()
{
  uint8_t c;
  bool showName = true; //readShowUnitNameFlag();
  
  if (!showName)
  {
    return;
  }

	EEPROM.begin(EEPROM_STORAGE_SPACE_START);
	c = EEPROM.read(EEPROM_UNIT_NAME) ;
	EEPROM.end();

  if (c != 0xFF && c != 0x00)
  {
    uint8_t i = EEPROM_UNIT_NAME;
    // cursor_x = 50;
    cursor_y = 56;

    EEPROM.begin(EEPROM_STORAGE_SPACE_START);
    do
    {
      write(c);
      c = EEPROM.read(++i);
    }  while (i < EEPROM_UNIT_NAME + ESPBOY_UNIT_NAME_LEN);
    EEPROM.end();

    display();
    delayShort(1000);
  }
}

size_t ESPBoy::write(uint8_t c)
{
  if (c == '\n')
  {
    cursor_y += textSize * 8;
    cursor_x = 0;
  }
  else if (c == '\r')
  {
    // skip em
  }
  else
  {
    drawChar(cursor_x, cursor_y, c, textColor, textBackground, textSize);
    cursor_x += textSize * 6;
    if (textWrap && (cursor_x > (WIDTH - textSize * 6)))
    {
      // calling ourselves recursively for 'newline' is
      // 12 bytes smaller than doing the same math here
      write('\n');
    }
  }
  return 1;
}

void ESPBoy::drawChar
  (int16_t x, int16_t y, unsigned char c, uint8_t color, uint8_t bg, uint8_t size)
{
  uint8_t line;
  bool draw_background = bg != color;
  const unsigned char* bitmap = font + c * 5;

  if ((x >= WIDTH) ||              // Clip right
      (y >= HEIGHT) ||             // Clip bottom
      ((x + 5 * size - 1) < 0) ||  // Clip left
      ((y + 8 * size - 1) < 0)     // Clip top
     )
  {
    return;
  }

  for (uint8_t i = 0; i < 6; i++ )
  {
    line = pgm_read_byte(bitmap++);
    if (i == 5) {
      line = 0x0;
    }

    for (uint8_t j = 0; j < 8; j++)
    {
      uint8_t draw_color = (line & 0x1) ? color : bg;

      if (draw_color || draw_background) {
        for (uint8_t a = 0; a < size; a++ ) {
          for (uint8_t b = 0; b < size; b++ ) {
            drawPixel(x + (i * size) + a, y + (j * size) + b, draw_color);
          }
        }
      }
      line >>= 1;
    }
  }
}

void ESPBoy::setCursor(int16_t x, int16_t y)
{
  cursor_x = x;
  cursor_y = y;
}

int16_t ESPBoy::getCursorX()
{
  return cursor_x;
}

int16_t ESPBoy::getCursorY()
{
  return cursor_y;
}

void ESPBoy::setTextColor(uint8_t color)
{
  textColor = color;
}

uint8_t ESPBoy::getTextColor()
{
  return textColor;
}

void ESPBoy::setTextBackground(uint8_t bg)
{
  textBackground = bg;
}

uint8_t ESPBoy::getTextBackground()
{
  return textBackground;
}

void ESPBoy::setTextSize(uint8_t s)
{
  // size must always be 1 or higher
  textSize = max(1, s);
}

uint8_t ESPBoy::getTextSize()
{
  return textSize;
}

void ESPBoy::setTextWrap(bool w)
{
  textWrap = w;
}

bool ESPBoy::getTextWrap()
{
  return textWrap;
}

void ESPBoy::clear()
{
    ESPBoyBase::clear();
    cursor_x = cursor_y = 0;
}

void ESPBoyBase::clearDisplay() {
    memset(sBuffer, 0, sBufferSize);
}