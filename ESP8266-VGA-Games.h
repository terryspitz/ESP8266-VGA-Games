#ifndef __ESP8266-VGA-Games_h
#define __ESP8266-VGA-Games_h

#ifdef ESP8266

  #include <ESPVGAX.h>
  extern ESPVGAX vga;

#elif ESP32

  #include "Arduino.h"
  #define PROGMEM
  #define ICACHE_RAM_ATTR
  #define ESPVGAX_WIDTH 512
  #define ESPVGAX_HEIGHT 480
  
  class VGAAdapter {
    public:
      void begin();
      void clear(uint8_t color);
      void delay(int ms);
      void drawCircle(int x0, int y0, int radius, uint8_t c, bool fill);
      void drawRect(int x, int y, int w, int h, uint8_t c, bool fill=false);
      void drawLine(int x0, int y0, int x1, int y1, uint8_t c);
      uint8_t getpixel(int x, int y);
      void tone(int freq);
      void noTone();
  };
  extern VGAAdapter vga;

#endif

extern bool buttonOneStatus;
extern bool buttonTwoStatus;
extern bool buttonThreeStatus;
extern byte wheelOnePosition;
extern byte wheelTwoPosition;

extern void processInputs();
void vgaTone(int freq, int time);
extern void vgaPrint(const char* str, int x, int y, byte color=0);
extern void vgaPrintNumber(byte number, int x, int y, byte color=0);

extern void setupPong(int players);
extern void setupBreakout();
extern void setupTetris();
extern void setupSnake();
extern void setupDrawingToy();
extern void loopPong();
extern void loopBreakout();
extern void loopTetris();
extern void loopSnake();
extern void loopDrawingToy();

#endif // __ESP8266-VGA-Games_h
