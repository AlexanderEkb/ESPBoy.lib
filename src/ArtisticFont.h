#ifndef ARTISTIC_FONT_H
#define ARTISTIC_FONT_H

#include <stdint.h>

typedef uint16_t Pixel_t;

class OffscreenBuffer
{
  public:
    OffscreenBuffer(uint32_t bufferWidth, uint32_t bufferHeight) : width(bufferWidth), height(bufferHeight) {};
    Pixel_t * getPointAddress(uint32_t x, uint32_t y);
  protected:
    const uint32_t width;
    const uint32_t height;
};

class ArtisticFont
{
  public:
    ArtisticFont(char const * const pFontData);
    uint32_t getHeight();
    uint32_t getWidth();
    void setTextColor(uint16_t newColor);
    void setTextBackground(uint16_t newBackground);
    void drawChar(uint32_t x, uint32_t y, char c);
  private:
    char * pGlyphs;
    uint32_t charWidth;
    uint32_t charHeight;
    uint32_t textColor;
    uint32_t textBackground;
};

#endif /* ARTISTIC_FONT_H */