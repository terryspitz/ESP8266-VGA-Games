/******************************************************************* 
*  ESP8266 Pong - Version Beta - December the 28th 2018            * 
*                                                                  *
*  ESP8266 Pong is a game for VGA monitors, written by             *
*  Roberto Melzi, running for Arduino IDE version 1.8.1, and       * 
*  based on the ESPVGAX library written By                         *
*  Sandro Maffiodo aka Smaffer.                                    * 
*                                                                  * 
*  https://github.com/smaffer/espvgax                              * 
*                                                                  * 
*  For more projects, see on Instructables:                        *
*  http://www.instructables.com/member/Rob%20Cai/instructables/    *
*                                                                  * 
*******************************************************************/ 

#include "ESP8266-VGA-Games.h"
#include <fonts/arial12.h>

static const int Horiz_Resolution=480;
static const int Vert_Resolution=512;
static const int Pad_Length=48*2;
static const int waitCycles=34*2;
static const int radius=6;

static int vgaWidth = ESPVGAX_WIDTH - 16;
static int vgaHeight = ESPVGAX_HEIGHT;
static float ballStatus = 1; 
static int deltaX = 4;
static int deltaY = 3; 
static int cx = vgaWidth /2; 
static int cy = vgaHeight/2; 
static int cx0 = cx; 
static int cy0 = cy; 
static int angle; 
static byte scoreL;
static byte scoreR; 
static char bufferL[8];
static char bufferR[8];
static int potOnePosition; 
static int potTwoPosition;
static int potOnePositionOld; 
static int potTwoPositionOld;
static int players;

static void gameOver();
static void processInputsPong ();
static void DrawPad(int xPos, int potPosition, boolean myColor);
static void drawBall(int cx, int cy, byte myColor);
static void ballPosition();
static void ballIni();
static void printScore();

void setupPong(int players_) {
  players=players_;
  vga.setFont((uint8_t*)fnt_arial12_data, FNT_ARIAL12_SYMBOLS_COUNT, FNT_ARIAL12_HEIGHT, FNT_ARIAL12_GLYPH_WIDTH);
  vga.drawRect(0, 0, ESPVGAX_WIDTH - 16, ESPVGAX_HEIGHT - 1, 1); 
  vga.print_P("ESP8266 VGAx Pong", 200, 200, true, -1, ESPVGAX_OP_OR, true);
  processInputsPong ();
  DrawPad(1, potOnePosition, 1); 
  DrawPad(vgaWidth - 2*radius, potTwoPosition, 1); 
  potOnePositionOld = potOnePosition; 
  potTwoPositionOld = potTwoPosition; 
}

//________________________________ Main Loop __________________________________________________
//_____________________________________________________________________________________________
ICACHE_RAM_ATTR void loopPong() {
  processInputsPong ();
  if (!(cx == cx0 && cy == cy0)) { drawBall(cx, cy, 1); }
  if (potOnePosition != potOnePositionOld ){ 
    DrawPad(1, potOnePositionOld, 0);
    vga.delay(2);
    DrawPad(1, potOnePosition, 1); 
    potOnePositionOld = potOnePosition; 
  }
  if (potTwoPosition != potTwoPositionOld ){ 
    DrawPad(vgaWidth - 2*radius, potTwoPositionOld, 0);
    vga.delay(2);
    DrawPad(vgaWidth - 2*radius, potTwoPosition, 1); 
    potTwoPositionOld = potTwoPosition; 
  }
  ballPosition(); 
  if ((buttonOneStatus == HIGH) && (ballStatus > 0)) {
     ballIni(); 
     ballStatus = 0; 
  }
  vga.delay(waitCycles);
  if (!(cx == cx0 && cy == cy0)) { drawBall(cx0, cy0, 0); }
}
//_______________________________________________________________________________________________________
//_______________________________________________________________________________________________________

static ICACHE_RAM_ATTR void processInputsPong (){
  int potOne = wheelOnePosition * 8;
  int potTwo = wheelTwoPosition * 8;

  potOnePosition = map(potOne, 40, 980, Pad_Length/2, 512 - Pad_Length/2); 
  potOnePosition = constrain(potOnePosition, Pad_Length/2 + 1, ESPVGAX_HEIGHT - Pad_Length/2 - 1); 
  potTwoPosition = map(potTwo, 40, 980, Pad_Length/2, 512 - Pad_Length/2); 
  potTwoPosition = constrain(potTwoPosition, Pad_Length/2 + 1, ESPVGAX_HEIGHT - Pad_Length/2 - 1);
  if (players == 1) potTwoPosition=potOnePosition;
}

//_______________________________________________________________________________________________________
static void drawBall(int cx, int cy, byte myColor){
  vga.drawCircle(cx, cy, radius, myColor, true); 
}

//_______________________________________________________________________________________________________
static void DrawPad(int xPos, int potPosition, boolean myColor){
  vga.drawRect(xPos, potPosition - Pad_Length/2, 2*radius, Pad_Length, myColor, true); 
} 

//_______________________________________________________________________________________________________
static void ballPosition(){
  cx0 = cx;
  cy0 = cy; 
  cx = cx + deltaX;
  cy = cy + deltaY; 
  if ((cx > vgaWidth - 3*radius/2 - 8) && (cy > potTwoPosition - Pad_Length/2) && (cy < potTwoPosition + Pad_Length/2)){
     cx = cx - 2*deltaX;
     deltaX = - deltaX;
     deltaY += int((potTwoPosition - cy0)/4);
  }
  if ((cx < 8 + 3*radius/2) && (cy > potOnePosition - Pad_Length/2) && (cy < potOnePosition + Pad_Length/2)){
     cx = cx - 2*deltaX; 
     deltaX = - deltaX;
     deltaY += int((potOnePosition - cy0)/4);
  }
  if (cx < 4 + radius/2) { 
     ballStatus = 1; 
     scoreR += 1; 
     printScore(); 
     deltaX = 0;
     deltaY = 0; 
  }
  if (cx > vgaWidth - radius/2 - 2){ 
     ballStatus = 2; 
     scoreL += 1; 
     printScore(); 
     deltaX = 0;
     deltaY = 0; 
  }
  if (ballStatus == 1){
     cx = 3*radius + 1; 
     cy = potOnePosition; 
  }
  if (ballStatus == 2){
     cx = vgaWidth - 3*radius; 
     cy = potTwoPosition; 
  }
  if ((scoreL > 9) | (scoreR > 9)){
    gameOver(); 
  }
  if ((cy > vgaHeight - radius/2 - 4) | (cy < 3 + radius/2)){
     cy = cy - 2*deltaY;
     deltaY = - deltaY;
  }
}

//_______________________________________________________________________________________________________
static void ballIni(){
   drawBall(cx0, cy0, 0);
   vga.drawRect(200, 198, 130, 20, 0, true); 
   if (ballStatus == 1) {angle = potOnePosition - cy0;} 
   if (ballStatus == 2) {angle = potTwoPosition - cy0;} 
   deltaX = 3 + random(2, 9);
   deltaY = random(-5, 5) + int(angle/4); 
   if (ballStatus == 2) {deltaX = -deltaX;} 
}

//_______________________________________________________________________________________________________
static void printScore(){
  dtostrf(scoreL, 4, 0, bufferL); 
  dtostrf(scoreR, 4, 0, bufferR); 
  vga.print_P("Left = ", 200, 200, true, -1, ESPVGAX_OP_OR, true);
  vga.print_P(bufferL, 232, 200, true, -1, ESPVGAX_OP_OR, true);
  vga.print_P("   Right = ", 250, 200, true, -1, ESPVGAX_OP_OR, true);
  vga.print_P(bufferR, 300, 200, true, -1, ESPVGAX_OP_OR, true);
}

//_______________________________________________________________________________________________________
static void gameOver(){
  vga.drawRect(200, 198, 130, 20, 0, true); 
  //if (scoreL > 9){vga.print_P("Left Wins!  GAME OVER ", 200, 200, true, -1, ESPVGAX_OP_OR, true);}
  //if (scoreR > 9){vga.print_P("Right Wins!  GAME OVER ", 200, 200, true, -1, ESPVGAX_OP_OR, true);}
  vga.drawRect(0, 0, ESPVGAX_WIDTH - 16, ESPVGAX_HEIGHT - 1, 1); 
  scoreL = 0;
  scoreR = 0;
  vga.print_P(" Game Over ", 200, 200, true, -1, ESPVGAX_OP_OR, true);
}
