#ifndef TFT_FONT_H
#define TFT_FONT_H

#include <Arduino.h>

// Arial-style 5x7 bitmap font, ASCII 32-126
// Each character = 5 bytes (columns), LSB = top row
static const uint8_t font5x7[] PROGMEM = {
  0x00,0x00,0x00,0x00,0x00, // 32 space
  0x00,0x00,0x5F,0x00,0x00, // 33 !
  0x00,0x07,0x00,0x07,0x00, // 34 "
  0x14,0x7F,0x14,0x7F,0x14, // 35 #
  0x24,0x2A,0x7F,0x2A,0x12, // 36 $
  0x23,0x13,0x08,0x64,0x62, // 37 %
  0x36,0x49,0x55,0x22,0x50, // 38 &
  0x00,0x00,0x03,0x00,0x00, // 39 '
  0x00,0x1C,0x22,0x41,0x00, // 40 (
  0x00,0x41,0x22,0x1C,0x00, // 41 )
  0x08,0x2A,0x1C,0x2A,0x08, // 42 *
  0x08,0x08,0x3E,0x08,0x08, // 43 +
  0x00,0x50,0x30,0x00,0x00, // 44 ,
  0x08,0x08,0x08,0x08,0x08, // 45 -
  0x00,0x60,0x60,0x00,0x00, // 46 .
  0x20,0x10,0x08,0x04,0x02, // 47 /
  0x3E,0x51,0x49,0x45,0x3E, // 48 0
  0x00,0x42,0x7F,0x40,0x00, // 49 1
  0x42,0x61,0x51,0x49,0x46, // 50 2
  0x21,0x41,0x45,0x4B,0x31, // 51 3
  0x18,0x14,0x12,0x7F,0x10, // 52 4
  0x27,0x45,0x45,0x45,0x39, // 53 5
  0x3C,0x4A,0x49,0x49,0x30, // 54 6
  0x01,0x71,0x09,0x05,0x03, // 55 7
  0x36,0x49,0x49,0x49,0x36, // 56 8
  0x06,0x49,0x49,0x29,0x1E, // 57 9
  0x00,0x36,0x36,0x00,0x00, // 58 :
  0x00,0x56,0x36,0x00,0x00, // 59 ;
  0x00,0x08,0x14,0x22,0x41, // 60 <
  0x14,0x14,0x14,0x14,0x14, // 61 =
  0x41,0x22,0x14,0x08,0x00, // 62 >
  0x02,0x01,0x51,0x09,0x06, // 63 ?
  0x32,0x49,0x79,0x41,0x3E, // 64 @
  0x7E,0x11,0x11,0x11,0x7E, // 65 A
  0x7F,0x49,0x49,0x49,0x36, // 66 B
  0x3E,0x41,0x41,0x41,0x22, // 67 C
  0x7F,0x41,0x41,0x41,0x3E, // 68 D
  0x7F,0x49,0x49,0x49,0x41, // 69 E
  0x7F,0x09,0x09,0x01,0x01, // 70 F
  0x3E,0x41,0x49,0x49,0x3A, // 71 G
  0x7F,0x08,0x08,0x08,0x7F, // 72 H
  0x00,0x41,0x7F,0x41,0x00, // 73 I
  0x20,0x40,0x41,0x3F,0x01, // 74 J
  0x7F,0x08,0x14,0x22,0x41, // 75 K
  0x7F,0x40,0x40,0x40,0x40, // 76 L
  0x7F,0x02,0x04,0x02,0x7F, // 77 M
  0x7F,0x04,0x08,0x10,0x7F, // 78 N
  0x3E,0x41,0x41,0x41,0x3E, // 79 O
  0x7F,0x09,0x09,0x09,0x06, // 80 P
  0x3E,0x41,0x51,0x21,0x5E, // 81 Q
  0x7F,0x09,0x19,0x29,0x46, // 82 R
  0x26,0x49,0x49,0x49,0x32, // 83 S
  0x01,0x01,0x7F,0x01,0x01, // 84 T
  0x3F,0x40,0x40,0x40,0x3F, // 85 U
  0x1F,0x20,0x40,0x20,0x1F, // 86 V
  0x7F,0x20,0x18,0x20,0x7F, // 87 W
  0x63,0x14,0x08,0x14,0x63, // 88 X
  0x03,0x04,0x78,0x04,0x03, // 89 Y
  0x61,0x51,0x49,0x45,0x43, // 90 Z
  0x00,0x00,0x7F,0x41,0x41, // 91 [
  0x02,0x04,0x08,0x10,0x20, // 92 backslash
  0x41,0x41,0x7F,0x00,0x00, // 93 ]
  0x04,0x02,0x01,0x02,0x04, // 94 ^
  0x40,0x40,0x40,0x40,0x40, // 95 _
  0x00,0x01,0x02,0x04,0x00, // 96 `
  0x20,0x54,0x54,0x54,0x78, // 97 a
  0x7F,0x44,0x44,0x44,0x38, // 98 b
  0x38,0x44,0x44,0x44,0x20, // 99 c
  0x38,0x44,0x44,0x44,0x7F, // 100 d
  0x38,0x54,0x54,0x54,0x18, // 101 e
  0x08,0x7E,0x09,0x01,0x00, // 102 f
  0x08,0x54,0x54,0x54,0x3C, // 103 g
  0x7F,0x04,0x04,0x04,0x78, // 104 h
  0x00,0x40,0x7D,0x40,0x00, // 105 i
  0x20,0x40,0x40,0x3D,0x00, // 106 j
  0x7F,0x10,0x28,0x44,0x00, // 107 k
  0x00,0x40,0x7F,0x40,0x00, // 108 l
  0x7C,0x04,0x1C,0x04,0x78, // 109 m
  0x7C,0x04,0x04,0x04,0x78, // 110 n
  0x38,0x44,0x44,0x44,0x38, // 111 o
  0x7C,0x24,0x24,0x24,0x18, // 112 p
  0x18,0x24,0x24,0x24,0x7C, // 113 q
  0x7C,0x08,0x04,0x04,0x08, // 114 r
  0x48,0x54,0x54,0x54,0x20, // 115 s
  0x04,0x3F,0x44,0x40,0x20, // 116 t
  0x3C,0x40,0x40,0x40,0x7C, // 117 u
  0x1C,0x20,0x40,0x20,0x1C, // 118 v
  0x3C,0x40,0x30,0x40,0x3C, // 119 w
  0x44,0x28,0x10,0x28,0x44, // 120 x
  0x0C,0x50,0x50,0x50,0x3C, // 121 y
  0x44,0x64,0x54,0x4C,0x44, // 122 z
  0x00,0x08,0x36,0x41,0x00, // 123 {
  0x00,0x00,0x7F,0x00,0x00, // 124 |
  0x00,0x41,0x36,0x08,0x00, // 125 }
  0x08,0x08,0x2A,0x1C,0x08, // 126 ~
};

// Render one scanline of a string into rowBuf (legacy 5x7 bitmap font)
// x: starting x pixel, str: text, charRow: 0..(7*scale-1), fg/bg: RGB565, scale: 1 or 2
static void tft_drawStringRow(uint16_t *rowBuf, int x, const char *str, int charRow,
                              uint16_t fg, uint16_t bg, int scale) {
  int fontRow = charRow / scale;
  if (fontRow > 6) return;

  while (*str) {
    char ch = *str++;
    if (ch < 32 || ch > 126) ch = '?';
    int idx = (ch - 32) * 5;

    for (int col = 0; col < 5; col++) {
      uint8_t colData = pgm_read_byte(&font5x7[idx + col]);
      bool pixel = (colData >> fontRow) & 1;
      uint16_t color = pixel ? fg : bg;
      for (int s = 0; s < scale; s++) {
        int px = x + col * scale + s;
        if (px >= 0 && px < 240) rowBuf[px] = color;
      }
    }
    // 1px spacing (scaled)
    for (int s = 0; s < scale; s++) {
      int px = x + 5 * scale + s;
      if (px >= 0 && px < 240) rowBuf[px] = bg;
    }
    x += 6 * scale;
  }
}

// ── GFX Font support (Adafruit GFX format) ──

typedef struct {
  uint32_t bitmapOffset;
  uint8_t  width, height;
  uint8_t  xAdvance;
  int8_t   xOffset, yOffset;
} GFXglyph;

typedef struct {
  uint8_t  *bitmap;
  GFXglyph *glyph;
  uint16_t  first, last;
  uint8_t   yAdvance;
} GFXfont;

#include "FreeSans9pt7b.h"
#include "FreeSansBold9pt7b.h"
#include "FreeSans12pt7b.h"

// Compute baseline (max ascent) for a GFX font
static int gfxFontBaseline(const GFXfont *font) {
  int baseline = 0;
  for (uint16_t c = font->first; c <= font->last; c++) {
    int8_t yo = pgm_read_byte(&font->glyph[c - font->first].yOffset);
    if (-yo > baseline) baseline = -yo;
  }
  return baseline;
}

// Render one scanline of a string using a GFX font into rowBuf
// x: starting x pixel, str: text, charRow: 0..(yAdvance-1)
// fg/bg: RGB565 colors, font: GFX font, baseline: from gfxFontBaseline()
static void tft_drawStringRowGFX(uint16_t *rowBuf, int x, const char *str, int charRow,
                                  uint16_t fg, uint16_t bg, const GFXfont *font, int baseline) {
  while (*str) {
    char ch = *str++;
    if (ch < pgm_read_word(&font->first) || ch > pgm_read_word(&font->last)) {
      if (ch == ' ' && ' ' >= pgm_read_word(&font->first)) {
        ch = ' ';
      } else {
        ch = '?';
      }
    }

    uint16_t idx = ch - pgm_read_word(&font->first);
    // Read glyph data from PROGMEM
    uint32_t bitmapOffset;
    uint8_t gw, gh, xAdv;
    int8_t xOff, yOff;
    memcpy_P(&bitmapOffset, &font->glyph[idx].bitmapOffset, sizeof(uint32_t));
    gw   = pgm_read_byte(&font->glyph[idx].width);
    gh   = pgm_read_byte(&font->glyph[idx].height);
    xAdv = pgm_read_byte(&font->glyph[idx].xAdvance);
    xOff = (int8_t)pgm_read_byte(&font->glyph[idx].xOffset);
    yOff = (int8_t)pgm_read_byte(&font->glyph[idx].yOffset);

    // Fill background for this character's advance width
    for (int s = 0; s < xAdv; s++) {
      int px = x + s;
      if (px >= 0 && px < 240) rowBuf[px] = bg;
    }

    // Which row of the glyph bitmap does this scanline correspond to?
    int glyphRow = charRow - baseline - yOff;
    if (glyphRow >= 0 && glyphRow < gh) {
      int bitOffset = glyphRow * gw;
      for (int col = 0; col < gw; col++) {
        int bit = bitOffset + col;
        uint8_t byteVal = pgm_read_byte(&font->bitmap[bitmapOffset + bit / 8]);
        bool pixel = (byteVal >> (7 - (bit & 7))) & 1;
        if (pixel) {
          int px = x + xOff + col;
          if (px >= 0 && px < 240) rowBuf[px] = fg;
        }
      }
    }

    x += xAdv;
  }
}

// ── Draw GFX font string on any display with setPixel(x,y) ──
// Use for OLED (SSD1306Wire) or any pixel-addressable display.
// x,y: top-left position of text line. Renders full string.
template <typename Display>
static void oled_drawStringGFX(Display &disp, int x, int y, const char *str, const GFXfont *font) {
  int baseline = gfxFontBaseline(font);
  int cursorX = x;

  while (*str) {
    char ch = *str++;
    if (ch < pgm_read_word(&font->first) || ch > pgm_read_word(&font->last)) {
      if (ch == ' ' && ' ' >= pgm_read_word(&font->first)) ch = ' ';
      else ch = '?';
    }

    uint16_t idx = ch - pgm_read_word(&font->first);
    uint32_t bitmapOffset;
    uint8_t gw, gh, xAdv;
    int8_t xOff, yOff;
    memcpy_P(&bitmapOffset, &font->glyph[idx].bitmapOffset, sizeof(uint32_t));
    gw   = pgm_read_byte(&font->glyph[idx].width);
    gh   = pgm_read_byte(&font->glyph[idx].height);
    xAdv = pgm_read_byte(&font->glyph[idx].xAdvance);
    xOff = (int8_t)pgm_read_byte(&font->glyph[idx].xOffset);
    yOff = (int8_t)pgm_read_byte(&font->glyph[idx].yOffset);

    for (int row = 0; row < gh; row++) {
      int bitOffset = row * gw;
      for (int col = 0; col < gw; col++) {
        int bit = bitOffset + col;
        uint8_t byteVal = pgm_read_byte(&font->bitmap[bitmapOffset + bit / 8]);
        if ((byteVal >> (7 - (bit & 7))) & 1) {
          disp.setPixel(cursorX + xOff + col, y + baseline + yOff + row);
        }
      }
    }
    cursorX += xAdv;
  }
}

// Compute string width in pixels for a GFX font
static int gfxStringWidth(const char *str, const GFXfont *font) {
  int w = 0;
  while (*str) {
    char ch = *str++;
    if (ch < pgm_read_word(&font->first) || ch > pgm_read_word(&font->last)) {
      if (ch == ' ' && ' ' >= pgm_read_word(&font->first)) ch = ' ';
      else ch = '?';
    }
    uint16_t idx = ch - pgm_read_word(&font->first);
    w += pgm_read_byte(&font->glyph[idx].xAdvance);
  }
  return w;
}

// Draw GFX string centered horizontally on display of given width
template <typename Display>
static void oled_drawStringGFXCenter(Display &disp, int centerX, int y, const char *str, const GFXfont *font) {
  int w = gfxStringWidth(str, font);
  oled_drawStringGFX(disp, centerX - w / 2, y, str, font);
}

#endif // TFT_FONT_H
