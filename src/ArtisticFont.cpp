#include "ArtisticFont.h"
#include "ESPBoyCore.h"

ArtisticFont::ArtisticFont(char const * const pFontData)
{
  charWidth = pFontData[0];
  charHeight = pFontData[1];
  pGlyphs = (char *)pFontData + 2;
}

uint32_t ArtisticFont::getHeight()
{
  return charHeight;
}

uint32_t ArtisticFont::getWidth()
{
  return charWidth;
}

void ArtisticFont::setTextColor(uint16_t newColor)
{
  textColor = newColor;
}

void ArtisticFont::setTextBackground(uint16_t newBackground)
{
  textBackground = newBackground;
}

void ArtisticFont::drawChar(uint32_t x, uint32_t y, char c)
{
  const uint32_t bitsPerByte = 8;
  uint32_t bytesPerCharacter = charWidth * charHeight / bitsPerByte;
  uint32_t _remainder = charHeight % bitsPerByte;
  uint32_t row;

  if(_remainder != 0)
  {
    bytesPerCharacter += charWidth;
  }
  uint32_t bufferSize = charWidth * charHeight;
  uint16_t buffer[bufferSize];
  uint32_t offset = c * bytesPerCharacter;
  char * pointer = pGlyphs + offset;

  for(uint32_t col=0; col<charWidth; col++)
  {
    row = 0;
    uint32_t remainder = charHeight;
    while(remainder > 0)
    {
      char byte = *pointer;
      uint32_t bitCount = (remainder>bitsPerByte)?bitsPerByte:remainder;
      for(uint32_t bit=0; bit<bitCount; bit++)
      {
        bool pixel = (byte & 0x80);
        byte <<= 1;
        uint32_t index = row * charWidth + col;
        buffer[index] = pixel?textColor:textBackground;
        row++;
        remainder--;
      }
      pointer++;
    }
  }
  theDisplay.drawRGBBitmap(x, y, buffer, charWidth, charHeight);
}

Pixel_t * OffscreenBuffer::getPointAddress(uint32_t x, uint32_t y)
{
  return nullptr;
}