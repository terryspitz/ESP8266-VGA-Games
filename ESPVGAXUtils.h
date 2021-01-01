/*
Compatibility Library to use Arduino VGAX code with ESPVGAX
*/
#ifndef __ESPVGAX_utils__
#define __ESPVGAX_utils__

#include "ESP8266-VGA-Games.h"

class ESPVGAXUtils {
public:
  ESPVGAXUtils(int scalex, int scaley) {
    ESPVGAX_SCALEX=scalex;
    ESPVGAX_SCALEY=scaley;
  }
  int ESPVGAX_SCALEX;
  int ESPVGAX_SCALEY;
  inline void draw_line(uint8_t x0, uint8_t y0, 
                        uint8_t x1, uint8_t y1, uint8_t c) {
    uint8_t tmp;
    if(x0==x1 && ESPVGAX_SCALEX > 1 && ESPVGAX_SCALEY > 1) {
      if (y0 > y1) {tmp=y0; y0=y1; y1=tmp; }
      vga.drawRect(x0*ESPVGAX_SCALEX, y0*ESPVGAX_SCALEY, ESPVGAX_SCALEX-1, (y1-y0)*ESPVGAX_SCALEY-1, c, true);
    } else if(y0==y1 && ESPVGAX_SCALEX > 1 && ESPVGAX_SCALEY > 1) {
      if (x0 > x1) {tmp=x0; x0=x1; x1=tmp; }
      vga.drawRect(x0*ESPVGAX_SCALEX, y0*ESPVGAX_SCALEY, (x1-x0)*ESPVGAX_SCALEX-1, ESPVGAX_SCALEY-1, c, true);
    } else {
      vga.drawLine(x0*ESPVGAX_SCALEX, y0*ESPVGAX_SCALEY, x1*ESPVGAX_SCALEX, y1*ESPVGAX_SCALEY, c);
    }
  }
  inline void draw_circle(uint8_t x0, uint8_t y0, 
                                 uint8_t radius, uint8_t c, char fc) {
    vga.drawCircle(x0*ESPVGAX_SCALEX, y0*ESPVGAX_SCALEY, radius*ESPVGAX_SCALEX, c, false);
  }
  inline void draw_rect(uint8_t x0, uint8_t y0, uint8_t w, uint8_t h, 
                               uint8_t c, char fc) {
    vga.drawRect(x0*ESPVGAX_SCALEX, y0*ESPVGAX_SCALEY, w*ESPVGAX_SCALEX-1, h*ESPVGAX_SCALEY-1, c, false);
  }
  inline void draw_row(uint8_t line, uint8_t x0, uint8_t x1, uint8_t c) {
    draw_line(x0, line, x1, line, c);
  }
  inline void draw_column(uint8_t row, uint8_t y0, uint8_t y1, uint8_t c) {
    draw_line(row, y0, row, y1, c);
  }
  inline uint8_t getpixel(uint8_t x, uint8_t y) {
    return vga.getpixel(x*ESPVGAX_SCALEX, y*ESPVGAX_SCALEY);
  }
  inline uint8_t putpixel(uint8_t x, uint8_t y, uint8_t c) {
    vga.drawRect(x*ESPVGAX_SCALEX, y*ESPVGAX_SCALEY, ESPVGAX_SCALEX-1, ESPVGAX_SCALEY-1, c, true);
  }
};

#endif
