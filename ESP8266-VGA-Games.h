#ifndef __ESP8266-VGA-Games_h
#define __ESP8266-VGA-Games_h

#include <ESPVGAX.h>
extern ESPVGAX vga;

extern boolean buttonOneStatus; 
extern boolean buttonTwoStatus; 
extern boolean buttonThreeStatus;
extern byte wheelOnePosition; 
extern byte wheelTwoPosition;

extern void processInputs();
extern void vgaPrint(const char* str, byte x, byte y, byte color);
extern void vgaPrintNumber(byte number, byte x, byte y, byte color);

extern void setupPong(int players);
extern void setupTetris();
extern void setupSnake();
extern void loopPong();
extern void loopTetris();
extern void loopSnake();

#endif // __ESP8266-VGA-Games_h
