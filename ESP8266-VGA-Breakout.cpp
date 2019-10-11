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
static ESPVGAXUtils vgaU(ESPVGAX_SCALEX,ESPVGAX_SCALEY);

static const char str10[] PROGMEM="#"; 
static const char str18[] PROGMEM="Game Over!!!";

static void gameIniBreakout();
static void parameterUpdate();
static void drawLives();
static void drawBorderBreakout();
static void drawBricksGrid(int n);
static void gameOverBreakout();
static void ballStart();
static void ballCoordinatesBreakout();
static void drawBallBreakout(byte x, byte y, byte col);
static void drawPadBreakout(byte padPosition, byte color);
static void nextlivesBreakout();

static byte state; 
static byte ballX; 
static byte ballY; 

static int beginning; // deve restare int poichè può assumere valori negativi, anche in breakout e bombard 
static float ballPX; // pong, breakout. No bomber
static int ballPY; // pong, breakout, bomber //************** era un int
static float ballVx; // già condivisa tra pong e bomber
static float ballVy; // già condivisa tra pong e bomber

// ------------------------------------------ Variable definition Breakout ---------------------------- 
static int speedX; //bomber e breakout 
static int speedY; 
static int speedT; //pong, breakout e bomber 
static float variableFloat_2; //bomber, breakout 
static float variableFloat_1; //bomber, breakout  
static byte padPosition; 
static byte padPositionOld; //bombard, breakout, tetris 
static byte ballXold;
static byte ballYold; 
static byte gameStep; 
static boolean cancelSafe; 
static byte hitScore; 
static byte lives = 8; //---------- qui lascio lives = 8 per pong. E' definito anche in fondo a parameterPongIni() ----------------
static byte ballX0;
static byte ballY0; 
static byte iDel;
static byte jDel; 
static byte ballXhit; 
static byte ballYhit; 
static const float pi PROGMEM = 3.1415296;
static byte nBricks; 
static byte color; 
static const float speedIncrement PROGMEM = 1.259921; //cubic root of two. Every three levels speed doubles --------------------
//----------------------------------------------------Variables definition end --------------------------------

void setupBreakout() {
  state = 4;
  gameIniBreakout();
}

static void gameIniBreakout(){ // Breakout
  ballPX = 70.;
  ballPY = 3000;
  cancelSafe = 0;
  hitScore = 0; 
  nBricks = 0;
  color = 1; 
  lives = 9; 
  speedT = 80; //------------ Breakout ball speed at the beginning ---------------- 
  gameStep = 0; 
  beginning = 0; 
  variableFloat_1 = 180./4; 
  padPositionOld = 115;
  padPosition = 114;
  parameterUpdate(); 
  drawLives(); 
  drawBorderBreakout();
  drawBricksGrid(gameStep); 
}

static void parameterUpdate() { // Breakout
  variableFloat_2 = variableFloat_1/180.* pi; 
  if (speedX > 0) {speedX = int(sin(variableFloat_2)*speedT);} else {speedX = int(-sin(variableFloat_2)*speedT);} 
  if (speedY > 0) {speedY = int(cos(variableFloat_2)*speedT);} else {speedY = int(-cos(variableFloat_2)*speedT);} 
}

static void drawLives() { // Breakout
    //vgaPrint(str10, 110, 10, 0);
    if (lives > 0) {vgaPrintNumber(lives, 110, 10, 3);}
    if(lives == 0) {gameOverBreakout();} 
}

static void gameOverBreakout() { // Breakout
  vgaTone(660, 200); 
  vgaTone(330, 200);
  vgaTone(165, 200); 
  vgaTone(82, 200);
  vgaPrint(str18, 25, 40, 1);
  ballStart(); 
  //if (state != 1){ 
  if (state == 4){ 
     vga.clear(0); 
     gameIniBreakout(); 
  }
  vga.delay(300);
}

static void ballStart(){ // Breakout
   ballPX = padPosition; 
   ballPY = 5500; 
   ballCoordinatesBreakout(); 
   ballY = 55; 
   drawBallBreakout(ballX, ballY, 2); 
   while (buttonOneStatus == 0 && state == 4){
      padPositionOld = padPosition; 
      processInputs();
      padPosition = map(wheelOnePosition, 127, 0, 7, 93); 
      if (padPosition != padPositionOld){
         drawPadBreakout(padPositionOld, 0); 
         drawPadBreakout(padPosition, 1);
         drawBallBreakout(ballX, ballY, 0); 
         ballPX = padPosition; 
         ballPY = 5500; 
         ballCoordinatesBreakout(); 
         ballY = 55; 
         drawBallBreakout(ballX, ballY, 2); 
      }
      if (buttonThreeStatus == 1){ // --------------------- ritorna al menu iniziale -------------------------
         state = 0; 
         vga.clear(0);
         vga.delay(300);
      }
      vga.delay(50);
   }
}

static void drawBorderBreakout() { // Breakout 
   vgaU.draw_line(0, 0, 0, 59, 3);
   vgaU.draw_line(0, 0, 99, 0, 3);
   vgaU.draw_line(0, 59, 99, 59, 3);
   vgaU.draw_line(99, 0, 99, 60, 3);
}

static void drawPadBreakout(byte padPosition, byte color) { // Breakout
   vgaU.draw_line(padPosition - 6, 58, padPosition + 6, 58, color);
   vgaU.draw_line(padPosition - 6, 57, padPosition + 6, 57, color);
}

static void brick(byte x, byte y, byte col) { // Breakout
   vgaU.draw_line(x - 5, y + 1, x + 5, y + 1 , col);
   vgaU.draw_line(x - 5, y - 1, x + 5, y - 1 , col);
   vgaU.putpixel(x - 5, y, col); 
   vgaU.putpixel(x + 4, y, col); 
}

static void drawBricksGrid(int n) { // Breakout
  nBricks = 0; 
  if (n%3 == 0) {   
    for (int i = 1; i <= int(100/13 - 1); i++) {
      for (int j = 1; j < 5; j++) {
         color = (i + j)%3 + 1; 
         brick(8 + i*12, 3 + j*4, color); 
         if (color |= 0) {nBricks++;}
      }
    }
  }
  if (n%3 == 1) {   
    for (int i = 0; i <= int(100/13); i++) {
      for (int j = 0; j < 7; j++) {
         color = (i + j)%4 + 0; 
         brick(8 + i*12, 3 + j*4, color); 
         if (color |= 0) {nBricks++;}
      }
    }
  }
  if (n%3 == 2) {   
    for (int i = 0; i <= int(100/13); i++) {
      for (int j = 0; j < 7; j++) {
         color = (i + j)%3 + 1; 
         brick(8 + i*12, 3 + j*4, color); 
         if (color |= 0) {nBricks++;}
      }
    }
  }
}

static void searchHitBrick(int bX, int bY) { // Breakout
   ballX0 = 120;
   ballY0 = 60;
   for (int i = 0; i <= int(100/13); i++) {
      for (int j = 0; j < 7; j++) {
         if (abs(8 + i*12 - bX) < ballX0){
            ballX0 = abs(8 + i*12 - bX); 
            iDel = i;
         }
         if (abs(1 + 2 + j*4 - bY) < ballY0){
            ballY0 = abs(1 + 2 + j*4 - bY); 
            jDel = j;
         }
      }
   }
   brick(8 + iDel*12, 3 + jDel*4, 0); 
   nBricks--; 
   if (nBricks == 0){nextlivesBreakout();}
}

static void drawBallBreakout(byte x, byte y, byte col) { // Breakout
   vgaU.putpixel(x, y, col); 
   vgaU.putpixel(x + 1, y, col); 
   vgaU.putpixel(x, y + 1, col); 
   vgaU.putpixel(x + 1, y + 1, col);
}

static void ballCoordinatesBreakout() { // Breakout
  ballXold = ballX; 
  ballYold = ballY; 
  ballPX += float(speedX/1000.);
  ballPY += speedY/10.;
  ballX = byte(ballPX); 
  ballY = byte(ballPY/100); 
}

static void nextlivesBreakout() { // Breakout
  gameStep++; 
  speedT = speedIncrement*speedT; 
  drawBricksGrid(gameStep); 
}

// ----------------------------------------------- breakout -----------------------------------
void hasHit() {
        if (hitScore == 3 | hitScore == 12) {
            speedY = -speedY;
        }
        else {
           if (hitScore == 5 | hitScore == 10) {
              speedX = -speedX;
           }
           else {
              speedX = -speedX;
              speedY = -speedY;
           }
        }
        if (ballY > 55) {
           if (vgaU.getpixel(ballX, ballY + 1) == 1 | vgaU.getpixel(ballX + 1, ballY + 1) == 1) { //-------------- breakout ball hits the paddle ----------------------- 
              variableFloat_1 = variableFloat_1 - 2*speedX/abs(speedX)*(padPosition - ballX); 
              if (variableFloat_1 < 20) {variableFloat_1 = 20;}
              if (variableFloat_1 > 70) {variableFloat_1 = 70;}
              parameterUpdate(); 
           } 
           else { //------------ breakout ball hits the bottom ----------------------- 
              lives = lives - 1; 
              if (lives |= 0) { vgaTone(110, 100); }
              drawLives(); 
              if (state == 4 ) { ballStart(); } 
              cancelSafe = 0; 
              speedY = - abs(speedY); 
           }
        }
        else if (ballX > 1 && ballX + 1 < 99 && ballY > 1) { //-------------- ball hits a brick otherwise it has it the border -------------- 
           vgaTone(440,30);
           if (hitScore == 1 | hitScore == 3 ) {ballXhit = ballX; ballYhit = ballY;}
           if (hitScore == 2 | hitScore == 10) {ballXhit = ballX + 1; ballYhit = ballY;}
           if (hitScore == 8 | hitScore == 12) {ballXhit = ballX + 1; ballYhit = ballY + 1;}
           if (hitScore == 4 | hitScore == 5) {ballXhit = ballX; ballYhit = ballY + 1;}
           searchHitBrick(ballXhit, ballYhit); 
        }
        if (ballPY <= 0) { //per baco ball che scompare da sopra ------------------------------------
           speedY = -speedY;
        }
}

ICACHE_RAM_ATTR void loopBreakout(){
  if (beginning == 0){ 
     padPositionOld = padPosition; 
     padPosition = map(wheelOnePosition, 127, 0, 7, 93); 
     gameIniBreakout(); 
     ballStart(); 
     beginning = 1; 
  }
  padPositionOld = padPosition; 
  //buttonOneStatus = 1; // for debugging ----------------------
  padPosition = map(wheelOnePosition, 127, 0, 7, 93); 

  if (padPosition != padPositionOld) {
     drawPadBreakout(padPositionOld, 0); 
     drawPadBreakout(padPosition, 1);
  }
  
  ballCoordinatesBreakout(); 
  
  if ( ballX != ballXold || ballY != ballYold) { 
     if (cancelSafe == 0) {
        drawBallBreakout(ballXold, ballYold, 0); 
        cancelSafe = 1; 
     }
     if (vgaU.getpixel(ballX, ballY) != 0) {hitScore += 1;}
     if (vgaU.getpixel(ballX + 1, ballY) != 0) {hitScore += 2;}
     if (vgaU.getpixel(ballX, ballY + 1) != 0) {hitScore += 4;}
     if (vgaU.getpixel(ballX + 1, ballY + 1) != 0) {hitScore += 8;}
     if (hitScore == 0) { //------------------------------- ball did not hit anything and can go ahead --------------
        drawBallBreakout(ballX, ballY, 2); 
        cancelSafe = 0; 
     }
     else { //--------------------------------- ball in breakout has hit something -------------------------- 
        hasHit();
        hitScore = 0; 
      }
   }
   vga.delay(2);
}
// ---------------------------------------- void breakout() end ---------------------------------------------
