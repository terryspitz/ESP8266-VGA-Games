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
void vgaTone(int freq, byte time);
extern void vgaPrint(const char* str, int x, int y, byte color);
extern void vgaPrintNumber(byte number, int x, int y, byte color);

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
