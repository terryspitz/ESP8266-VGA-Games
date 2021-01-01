/*******************************************************************
   ESP8266 Pong - Version Beta - December the 28th 2018

   ESP8266 Pong is a game for VGA monitors, written by
   Roberto Melzi, running for Arduino IDE version 1.8.1, and
   based on the ESPVGAX library written By
   Sandro Maffiodo aka Smaffer.

   https://github.com/smaffer/espvgax

   For more projects, see on Instructables:
   http://www.instructables.com/member/Rob%20Cai/instructables/

   ESP32 port by terryspitz, Xmas 2020
   Inspired by https://upload.wikimedia.org/wikipedia/commons/2/26/Pong.svg

*******************************************************************/

#include "ESP8266-VGA-Games.h"

static const int Pad_Length = 40 * 2;
static const int waitCycles = 34 * 2;
static const int radius = 5;
static const int left_pad_x = 1;
static const int right_pad_x = (ESPVGAX_WIDTH - 1) - 2 * radius;

static float ballStatus = 1;
static int deltaX = 4;
static int deltaY = 3;
static int cx = ESPVGAX_WIDTH / 2;
static int cy = 0;
static int cx0 = cx;
static int cy0 = cy;
static int angle;
static byte scoreL;
static byte scoreR;
static int potOnePosition;
static int potTwoPosition;
static int potOnePositionOld;
static int potTwoPositionOld;
static int players;

static void gameOver();
static void processInputsPong();
static void drawPad(int xPos, int potPosition, bool myColor);
static void drawBall(int cx, int cy, byte myColor);
static void ballPosition();
static void ballIni();
static void printScore();

void setupPong(int players_) {
  players = players_;
  vga.clear(0);
  vgaPrint("Pong", ESPVGAX_WIDTH/2, ESPVGAX_HEIGHT/2, 1);
  processInputsPong();
  drawPad(left_pad_x, potOnePosition, 1);
  drawPad(right_pad_x, potTwoPosition, 1);
  potOnePositionOld = potOnePosition;
  potTwoPositionOld = potTwoPosition;
}

//________________________________ Main Loop __________________________________________________
//_____________________________________________________________________________________________
ICACHE_RAM_ATTR void loopPong() {
  processInputsPong ();
  if (!(cx == cx0 && cy == cy0)) {
    drawBall(cx, cy, 1);
  }
  if (potOnePosition != potOnePositionOld ) {
    drawPad(left_pad_x, potOnePositionOld, 0);
    //vga.delay(2);
    drawPad(left_pad_x, potOnePosition, 1);
    potOnePositionOld = potOnePosition;
  }
  if (potTwoPosition != potTwoPositionOld ) {
    drawPad(right_pad_x, potTwoPositionOld, 0);
    //vga.delay(2);
    drawPad(right_pad_x, potTwoPosition, 1);
    potTwoPositionOld = potTwoPosition;
  }
  ballPosition();
  if (buttonOneStatus && (ballStatus > 0)) {
    vga.clear(0);
    ballIni();
    ballStatus = 0;
    drawPad(left_pad_x, potOnePosition, 1);
    drawPad(right_pad_x, potTwoPosition, 1);
  }
  vga.delay(waitCycles);
  if (!(cx == cx0 && cy == cy0)) {
    drawBall(cx0, cy0, 0);
  }
}
//_______________________________________________________________________________________________________
//_______________________________________________________________________________________________________
/* My board has one joystick and two buttons.
 * In Pong i use the two buttons for left player and joystick for right player.
 */
#define JOYSTICK 1
#ifdef JOYSTICK
#define BUTTON_1_PIN D3
#define BUTTON_2_PIN D4
#endif

static ICACHE_RAM_ATTR void processInputsPong () {
#ifdef JOYSTICK
  if (players == 1) {
    potOnePosition = map(wheelOnePosition, 0, 128, Pad_Length / 2, ESPVGAX_HEIGHT - Pad_Length / 2);
    potTwoPosition = potOnePosition;
  }
  else {
    potOnePosition = constrain(potOnePosition - buttonOneStatus*8 + buttonTwoStatus*8, Pad_Length / 2, ESPVGAX_HEIGHT - Pad_Length / 2);
    potTwoPosition = map(wheelOnePosition, 0, 128, Pad_Length / 2, ESPVGAX_HEIGHT - Pad_Length / 2);
  }
#else
  potOnePosition = map(wheelOnePosition, 0, 128, Pad_Length / 2, ESPVGAX_HEIGHT - Pad_Length / 2);
  if (players == 1)
    potTwoPosition = potOnePosition;
  else
    potTwoPosition = map(wheelTwoPosition, 0, 128, Pad_Length / 2, ESPVGAX_HEIGHT - Pad_Length / 2);
#endif
}

//_______________________________________________________________________________________________________
static void drawBall(int cx, int cy, byte myColor) {
  //vga.drawCircle(cx, cy, radius, myColor, true);                                //not very retro :(
  vga.drawRect(cx - radius/2, cy - radius/2, 2*radius, 2*radius, myColor, true);  //that's better!
}

//_______________________________________________________________________________________________________
static void drawPad(int xPos, int potPosition, bool color) {
  vga.drawRect(xPos, potPosition - Pad_Length / 2, 2 * radius, Pad_Length, color, true);
}

//_______________________________________________________________________________________________________
static void ballPosition() {
  cx0 = cx;
  cy0 = cy;
  cx = cx + deltaX;
  cy = cy + deltaY;
  //hit right paddle
  if ((cx > right_pad_x - radius - 2) && (cy > potTwoPosition - Pad_Length / 2) && (cy < potTwoPosition + Pad_Length / 2)) {
    cx = cx - 2 * deltaX;
    deltaX = - deltaX;
    deltaY += int((potTwoPosition - cy0) / 4);
  }
  //hit left paddle
  if ((cx < 8 + 3 * radius / 2) && (cy > potOnePosition - Pad_Length / 2) && (cy < potOnePosition + Pad_Length / 2)) {
    cx = cx - 2 * deltaX;
    deltaX = - deltaX;
    deltaY += int((potOnePosition - cy0) / 4);
  }
  //hit left wall
  if (cx < 4 + radius / 2) {
    ballStatus = 1;
    scoreR += 1;
    printScore();
    deltaX = 0;
    deltaY = 0;
  }
  //hit right wall
  if (cx > ESPVGAX_WIDTH - radius / 2 - 2) {
    ballStatus = 2;
    scoreL += 1;
    printScore();
    deltaX = 0;
    deltaY = 0;
  }
  if (ballStatus == 1) {
    cx = left_pad_x + 3 * radius;
    cy = potOnePosition;
  }
  if (ballStatus == 2) {
    cx = (right_pad_x - 2) - radius;
    cy = potTwoPosition;
  }
  if ((scoreL > 9) || (scoreR > 9)) {
    gameOver();
  }
  if ((cy > ESPVGAX_HEIGHT - radius / 2 - 4) | (cy < 3 + radius / 2)) {
    cy = cy - 2 * deltaY;
    deltaY = - deltaY;
  }
}

//_______________________________________________________________________________________________________
static void ballIni() {
  drawBall(cx0, cy0, 0);
  vga.drawRect(200, 198, 130, 20, 0, true);
  if (ballStatus == 1) {
    angle = potOnePosition - cy0;
  }
  if (ballStatus == 2) {
    angle = potTwoPosition - cy0;
  }
  deltaX = 3 + random(2, 9);
  deltaY = random(-5, 5) + int(angle / 4);
  if (ballStatus == 2) {
    deltaX = -deltaX;
  }
}

//_______________________________________________________________________________________________________
static void printScore() {
  String score = "Left " + String(scoreL) + "   Right " + String(scoreR);
  vgaPrint(score.c_str(), (ESPVGAX_WIDTH - ESPVGAX_WIDTH/40 * score.length()) / 2, ESPVGAX_HEIGHT/2 - 20, 1);
  vga.delay(2000);
}

//_______________________________________________________________________________________________________
static void gameOver() {
  vga.drawRect(200, 198, 130, 20, 0, true);
  //if (scoreL > 9){vgaPrint("Left Wins!  GAME OVER ", 200, 200);}
  //if (scoreR > 9){vgaPrint("Right Wins!  GAME OVER ", 200, 200);}
  vga.drawRect(0, 0, ESPVGAX_WIDTH - 16, ESPVGAX_HEIGHT - 1, 1);
  scoreL = 0;
  scoreR = 0;
  vgaPrint(" Game Over ", (ESPVGAX_WIDTH - ESPVGAX_WIDTH/40 * 10) / 2, ESPVGAX_HEIGHT/2 - 20, 0);
}
