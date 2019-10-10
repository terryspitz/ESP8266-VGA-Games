/*
Compatibility Library to use Arduino VGAX code with ESPVGAX
*/
#ifndef __ESPVGAX_utils__
#define __ESPVGAX_utils__

#include <ESPVGAX.h>

#define VGAX ESPVGAX
#ifndef ESPVGAX_SCALEX
#define ESPVGAX_SCALEX 1
#endif
#ifndef ESPVGAX_SCALEY
#define ESPVGAX_SCALEY 1
#endif
#ifndef VGAX_WIDTH
#define VGAX_WIDTH (ESPVGAX_WIDTH/ESPVGAX_SCALEX)
#endif
#ifndef VGAX_HEIGHT
#define VGAX_HEIGHT (ESPVGAX_HEIGHT/ESPVGAX_SCALEY)
#endif

class ESPVGAXUtils {
public:
  static inline void draw_line(uint8_t x0, uint8_t y0, 
                               uint8_t x1, uint8_t y1, uint8_t c) {
    if(x0==x1 && ESPVGAX_SCALEX > 1 && ESPVGAX_SCALEY > 1) {
      ESPVGAX::drawRect(x0*ESPVGAX_SCALEX, y0*ESPVGAX_SCALEY, ESPVGAX_SCALEX, (y1-y0)*ESPVGAX_SCALEY, c, true, ESPVGAX_OP_SET);
    } else if(y0==y1 && ESPVGAX_SCALEX > 1 && ESPVGAX_SCALEY > 1) {
      ESPVGAX::drawRect(x0*ESPVGAX_SCALEX, y0*ESPVGAX_SCALEY, (x1-x0)*ESPVGAX_SCALEX, ESPVGAX_SCALEY, c, true, ESPVGAX_OP_SET);
    } else {
      ESPVGAX::drawLine(x0*ESPVGAX_SCALEX, y0*ESPVGAX_SCALEY, x1*ESPVGAX_SCALEX, y1*ESPVGAX_SCALEY, c, ESPVGAX_OP_SET);
    }
  }
  static inline void draw_circle(uint8_t x0, uint8_t y0, 
                                 uint8_t radius, uint8_t c, char fc) {
    ESPVGAX::drawCircle(x0*ESPVGAX_SCALEX, y0*ESPVGAX_SCALEY, radius*ESPVGAX_SCALEX, c, false, ESPVGAX_OP_SET);
  }
  static inline void draw_rect(uint8_t x0, uint8_t y0, uint8_t w, uint8_t h, 
                               uint8_t c, char fc) {
    ESPVGAX::drawRect(x0*ESPVGAX_SCALEX, y0*ESPVGAX_SCALEY, w*ESPVGAX_SCALEX, h*ESPVGAX_SCALEY, c, false, ESPVGAX_OP_SET);
  }
  static inline void draw_row(uint8_t line, uint8_t x0, uint8_t x1, uint8_t c) {
    ESPVGAXUtils::draw_line(x0, line, x1, line, c);
  }
  static inline void draw_column(uint8_t row, uint8_t y0, uint8_t y1, uint8_t c) {
    ESPVGAXUtils::draw_line(row, y0, row, y1, c);
  }
  static inline uint8_t getpixel(uint8_t x, uint8_t y) {
    return ESPVGAX::getpixel(x*ESPVGAX_SCALEX+1, y*ESPVGAX_SCALEY+1);
  }
  static inline uint8_t putpixel(uint8_t x, uint8_t y, uint8_t c) {
    ESPVGAX::drawRect(x*ESPVGAX_SCALEX, y*ESPVGAX_SCALEY, ESPVGAX_SCALEX, ESPVGAX_SCALEY, c, true, ESPVGAX_OP_SET);
  }
};

#endif
