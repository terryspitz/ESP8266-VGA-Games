/******************************************************************* 
*  ArduinoVGAgame - Version 2 - September the 29th 2017            * 
*                                                                  *
*  Arduino VGA Game is a game consolle for VGA monitors,           * 
*  written by Roberto Melzi, running for Arduino software          * 
*  IDE version 1.6.4                                               * 
*  VGAx library does not work for newer or elder software          *
*  versions.                                                       * 
*  The consolle includes the games:                                *
*  Pong, Breakout, Bomber, Etch-a-Sketch and Tetris.               *
*                                                                  *
*  VGAx Library written By Sandro Maffiodo aka Smaffer.            * 
*  https://github.com/smaffer/vgax                                 * 
*  See also the Arduino forum:                                     * 
*  http://forum.arduino.cc/index.php?topic=328631.0                * 
*  and on Instructables:                                           *
*  http://www.instructables.com/member/Rob%20Cai/instructables/    *
*                                                                  * 
*******************************************************************/ 

#include <math.h>
#include "ESP8266-VGA-Games.h"
#include "ESPVGAXUtils.h"
static const int ESPVGAX_SCALEX=4;
static const int ESPVGAX_SCALEY=4;
static const int VGAX_WIDTH=(ESPVGAX_WIDTH/4);
static const int VGAX_HEIGHT=(ESPVGAX_HEIGHT/4);
static ESPVGAXUtils vgaU(ESPVGAX_SCALEX,ESPVGAX_SCALEY);

static byte ballX; 
static byte ballY; 
static byte color; 

extern void setupDrawingToy() {
  vga.clear(0);
  color = 1;
}

void loopDrawingToy() { //--------------------------- drawing toy --------------------------------------------- 
  processInputs(); 
  ballX = byte(map(wheelOnePosition, 0, 128, VGAX_WIDTH - 1, 1));
  ballY = byte(map(wheelTwoPosition, 128, 0, VGAX_HEIGHT - 1, 1));
  vgaU.putpixel(ballX, ballY, color);
  if (buttonTwoStatus == 1) {
     setupDrawingToy();
     vgaTone(262, 80);
     vgaTone(330, 80);
     vgaTone(392, 80);
     vgaTone(524, 80);
  }
  if (buttonOneStatus == 1) {
    color += 1; 
    if (color == 2) 
       {color = 0;} 
    vgaTone(440, 60);  
    vga.delay(200); 
  }
  vga.delay(20);
}
