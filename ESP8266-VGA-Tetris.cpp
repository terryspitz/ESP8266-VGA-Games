// ****************************
// *                          *
// *       Tetris.ino         *
// *                          *
// ****************************
//
// version for new wood box (hexagonal) 
// + some bugs fixed 
//
// Arduino Tetris for VGA output - beta Version
// written by Roberto Melzi - December the 21th, 2015 
// Arduino software version: 1.6.4
// Added sound: June the 22nd, 2016 
// some bug solved: November the 13th, 2016
//
// Version for 4:3 VGA monitor
//
// See it running on YouTube: "Arduino VGA Pong (with colors)" 
// https://www.youtube.com/watch?v=mrRGosn48gw
// VGAx Library By Sandro Maffiodo (version downloaded on August the 17th 2015) here:
// https://github.com/smaffer/vgax
// Post on the Arduino forum:
// http://forum.arduino.cc/index.php?topic=328631.0

#include <math.h>
#include "ESP8266-VGA-Games.h"
#include "ESPVGAXUtils.h"
static const int ESPVGAX_SCALEX=4;
static const int ESPVGAX_SCALEY=8;
static ESPVGAXUtils vgaU(ESPVGAX_SCALEX,ESPVGAX_SCALEY);

static const char str0[] PROGMEM="0"; 
static const char str1[] PROGMEM="1"; 
static const char str2[] PROGMEM="2"; 
static const char str3[] PROGMEM="3"; 
static const char str4[] PROGMEM="4"; 
static const char str5[] PROGMEM="5"; 
static const char str6[] PROGMEM="6"; 
static const char str7[] PROGMEM="7"; 
static const char str8[] PROGMEM="8"; 
static const char str9[] PROGMEM="9"; 
static const char str10[] PROGMEM="#"; 
static const char str11[] PROGMEM="Arduino VGA Tetris"; 
static const char str12[] PROGMEM="by Roberto Melzi"; 
static const char str13[] PROGMEM="Game"; 
static const char str14[] PROGMEM="Over!"; 

static int block[4][2]={{0,0},{0,0},{0,0},{0,0}};
static int blockExt[4][2]={{0,0},{0,0},{0,0},{0,0}};
static int blockOld[4][2]={{0,0},{0,0},{0,0},{0,0}};
static int blockTmp[4][2]={{0,0},{0,0},{0,0},{0,0}};
static int blockTr[4][2]={{0,0},{0,0},{0,0},{0,0}};
static int yLine[4] = {0,0,0,0}; 
static int yLineTmp[4] = {0,0,0,0}; 
static int yCounter = 0; 
static int x = 60; 
static int y = 6; 
static int z = 10; 
static int score; 
static int noLoop = -1; 
static int clock = 1; 
static int delta = 0;  
static int color = 1; 
static int colorOld; 
static int blockN; 
static int blockNext; 
static int busy; 
static int noDelete = 0; 
static int k = 0; 
static int a = 40;
static int b = 10; 
static int counterMenu = 0; 
static unsigned long ticks = 0;
static int fast = 14; //14; 
static int buttonThreePressedTime = 0;

static void drawGameScreen();
static void vgaTone(int freq, int ticks);
static void gameOver();
static void checkForFullLine();
static void drawScore(int i);

void setupTetris() {
  noLoop = -1;
  score = 0;
  fast = 14;
  ticks = 0;
}

static ICACHE_RAM_ATTR void processInputsTetris() {
  vga.delay(buttonOneStatus == 1 || buttonTwoStatus == 1 || buttonThreeStatus == 1 ? 100 : 25);
}

static void drawMenu() {
  vga.clear(0);
  drawGameScreen();
  drawScore(score);
}

static void drawScore(int i) {
    if (i > 39){
       score = 0;
       i = 0; 
       if (fast > 5 ) {fast = fast - 3;}
       else {fast = fast - 1;}
       if (fast < 3) {fast = 2;}
    }
    vgaU.draw_line(20, 10, 20, 50, 3); 
    vgaU.draw_line(20, 50, 20, 50 - i, 1); 
    vgaU.draw_line(19, 10, 22, 10, 3); 
    vgaU.draw_line(19, 50, 22, 50, 3); 
}
    
static void drawBorder() {
   // total screen size = 120/60
   // tetris game board: width = 30; heigh = 60
   vgaU.draw_line(44,0,78,0,3);
   vgaU.draw_line(44,59,78,59,3);
   vgaU.draw_line(44,0,44,59,3);
   vgaU.draw_line(78,0,78,60,3);
}

// --------------------- this is for the beginning game window ------------------------ 
static void drawStartScreen() {
   drawBorder(); 
   drawGameScreen();
   vga.delay(200);
} 

// ---------------------- this is the main function to draw the game screen -----------
static void drawGameScreen() {
  drawBorder(); 
}

// ----------------------- Tetriminos definition --------------------------------------
static void blockDef(int i) {
  if (i == 1){
  // O
  block[0][0] = 0;
  block[0][1] = 0;
  block[1][0] = 1;
  block[1][1] = 0;
  block[2][0] = 0;
  block[2][1] = 1;
  block[3][0] = 1;
  block[3][1] = 1;
  color = 1; 
  }
  if (i == 2){
  // L
  block[0][0] = -1;
  block[0][1] = 0;
  block[1][0] = 0;
  block[1][1] = 0;
  block[2][0] = 1;
  block[2][1] = 0;
  block[3][0] = -1;
  block[3][1] = 1;
  color = 2; 
  }
  if (i == 3){
  // J
  block[0][0] = -1;
  block[0][1] = 0;
  block[1][0] = 0;
  block[1][1] = 0;
  block[2][0] = 1;
  block[2][1] = 0;
  block[3][0] = 1;
  block[3][1] = 1;
  color = 3; 
  }
  if (i == 4){
  // I
  block[0][0] = -1;
  block[0][1] = 0;
  block[1][0] = 0;
  block[1][1] = 0;
  block[2][0] = 1;
  block[2][1] = 0;
  block[3][0] = 2;
  block[3][1] = 0;
    color = 1; 
  }
  if (i == 5){
  // S
  block[0][0] = -1;
  block[0][1] = 0;
  block[1][0] = 0;
  block[1][1] = 0;
  block[2][0] = 0;
  block[2][1] = 1;
  block[3][0] = 1;
  block[3][1] = 1;
  color = 2; 
  }
  if (i == 6){
  // Z
  block[0][0] = -1;
  block[0][1] = 1;
  block[1][0] = 0;
  block[1][1] = 1;
  block[2][0] = 0;
  block[2][1] = 0;
  block[3][0] = 1;
  block[3][1] = 0;
  color = 3; 
  }
  if (i == 7){
  // T
  block[0][0] = -1;
  block[0][1] = 0;
  block[1][0] = 0;
  block[1][1] = 0;
  block[2][0] = 0;
  block[2][1] = 1;
  block[3][0] = 1;
  block[3][1] = 0;
  color = 1; 
  }
}

// -------------------------- expansion for 4:3 monitors ------------------------------ 
static void blockExtension() {
   for (int i = 0; i < 4; i++){
      blockExt[0][0] = block[0][0]*3;
      blockExt[0][1] = block[0][1]*2;
      blockExt[1][0] = block[1][0]*3;
      blockExt[1][1] = block[1][1]*2;
      blockExt[2][0] = block[2][0]*3;
      blockExt[2][1] = block[2][1]*2;
      blockExt[3][0] = block[3][0]*3;
      blockExt[3][1] = block[3][1]*2;
   }
}
 
static void blockRotation(int clock){
  for (int i = 0; i < 4; i++){
     blockOld[0][0] = block[0][0];
     blockOld[0][1] = block[0][1];
     blockOld[1][0] = block[1][0];
     blockOld[1][1] = block[1][1];
     blockOld[2][0] = block[2][0];
     blockOld[2][1] = block[2][1];
     blockOld[3][0] = block[3][0];
     blockOld[3][1] = block[3][1];
  }
  for (int i = 0; i < 4; i++){
     block[0][0] = blockOld[0][1]*clock;
     block[0][1] = -blockOld[0][0]*clock;
     block[1][0] = blockOld[1][1]*clock;
     block[1][1] = -blockOld[1][0]*clock;
     block[2][0] = blockOld[2][1]*clock;
     block[2][1] = -blockOld[2][0]*clock;
     block[3][0] = blockOld[3][1]*clock;
     block[3][1] = -blockOld[3][0]*clock;
  }
}
static void blockTranslation(int x, int y) {
   for (int i = 0; i < 4; i++){
      blockTr[0][0] = blockExt[0][0] + x;
      blockTr[0][1] = blockExt[0][1] + y;
      blockTr[1][0] = blockExt[1][0] + x;
      blockTr[1][1] = blockExt[1][1] + y;
      blockTr[2][0] = blockExt[2][0] + x;
      blockTr[2][1] = blockExt[2][1] + y;
      blockTr[3][0] = blockExt[3][0] + x;
      blockTr[3][1] = blockExt[3][1] + y;
   }
}

static void delBlock(){
  if (noDelete == 1) {noDelete = 0;} 
  else {
      for (int i = 0; i < 4; i++){
         vgaU.draw_line(blockTr[i][0],blockTr[i][1],blockTr[i][0] + 3,blockTr[i][1],0);
         vgaU.draw_line(blockTr[i][0],blockTr[i][1] + 1,blockTr[i][0] + 3,blockTr[i][1] + 1,0);   
      }
   }
}

static void drawBlock(){
  for (int i = 0; i < 4; i++){
     vgaU.draw_line(blockTr[i][0],blockTr[i][1],blockTr[i][0] + 3,blockTr[i][1], color);
     vgaU.draw_line(blockTr[i][0],blockTr[i][1] + 1,blockTr[i][0] + 3,blockTr[i][1] + 1, color);   
  }
  for (int i = 0; i < 4; i++){
     blockTmp[0][0] = blockTr[0][0];
     blockTmp[0][1] = blockTr[0][1];
     blockTmp[1][0] = blockTr[1][0];
     blockTmp[1][1] = blockTr[1][1];
     blockTmp[2][0] = blockTr[2][0];
     blockTmp[2][1] = blockTr[2][1];
     blockTmp[3][0] = blockTr[3][0];
     blockTmp[3][1] = blockTr[3][1];
  }
}

static void drawBlockTmp(){
  for (int i = 0; i < 4; i++){
     vgaU.draw_line(blockTmp[i][0],blockTmp[i][1],blockTmp[i][0] + 3,blockTmp[i][1], color);
     vgaU.draw_line(blockTmp[i][0],blockTmp[i][1] + 1,blockTmp[i][0] + 3,blockTmp[i][1] + 1, color);   
  }
}

static void checkBlock(){ 
  busy = 0;  
  for (int i = 0; i < 4; i++){
     busy = busy + vgaU.getpixel(blockTr[i][0], blockTr[i][1]) + vgaU.getpixel(blockTr[i][0] + 2, blockTr[i][1]);
  }
}

static void replaceBlock(){
     blockExtension(); 
     blockTranslation(x, y); 
     checkBlock();
     if (busy == 0){
        drawBlock(); 
     }
     else // ---------- else is run if the block cannot get down  ----------------- 
     {
        drawBlockTmp(); 
        checkForFullLine(); // ---- check il the line is filled when the block cennot get down anymore ---------------------- 
        noLoop = 0; 
        noDelete = 1; 
        if (y < 8) {
           gameOver();
        }
     }
     vga.delay(1);
}

static void gameOver(){ // ------------------------------------------- Game Over ! --------------------------------------------------
   noLoop = -1; 
   score = 0;
   fast = 14;
   ticks = 0; 
   vgaPrint(str13, 92, 30, 1);
   vgaPrint(str14, 92, 38, 1);
   vga.delay(300);
   vgaTone(660, 200); 
   vgaTone(330, 200);
   vgaTone(165, 200); 
   vgaTone(82, 200);
   while (buttonOneStatus == 0 && buttonTwoStatus == 0 && buttonThreeStatus == 0) {
      processInputs(); processInputsTetris();
      vga.delay(200);
   }
   vga.clear(0);  
}

static void drawBlockNext(){ // ----- draw next block on the right side -------------------------------- 
     blockExtension(); 
     blockTranslation(100, 10); 
     vgaU.draw_line(95, 8, 112, 8, 0);
     vgaU.draw_line(95, 9, 112, 9, 0);
     vgaU.draw_line(95, 10, 112, 10, 0);
     vgaU.draw_line(95, 11, 112, 11, 0);
     vgaU.draw_line(95, 12, 112, 12, 0);
     vgaU.draw_line(95, 13, 112, 13, 0);
     vgaU.draw_line(95, 14, 112, 14, 0);
     vgaU.draw_line(95, 15, 112, 15, 0);
     drawBlock(); 
}

static void checkBlockTranslation(){
     x = x + delta; 
     blockExtension(); 
     blockTranslation(x, y); 
     checkBlock();
     if (busy == 0){
        drawBlock(); 
     }
     else
     {
        x = x - delta; 
        blockExtension(); 
        blockTranslation(x, y); 
        drawBlock(); 
     }
     vga.delay(50);
}

static void checkBlockRotation(){
     //x = x + delta; 
     blockExtension(); 
     blockTranslation(x, y); 
     checkBlock();
     if (busy == 0){
        drawBlock(); 
     }
     else
     {
        clock = -clock; 
        blockRotation(clock); 
        blockExtension(); 
        blockTranslation(x, y); 
        drawBlock(); 
     }
     vga.delay(50);
}

static void checkForFullLine() { // --------------------- check if the line is full and must be deleted --------------
   for (int i = 0; i < 4; i++){
      for (int j = 45; j < 76; j += 3) {
         if (vgaU.getpixel(j, blockTmp[i][1]) >0){k++; }
      }
      if (k == 11) { // ---- line is full and must be deleted ----------------------------------------------------------
         vgaU.draw_line(45, blockTmp[i][1], 78, blockTmp[i][1], 0); 
         vgaU.draw_line(45, blockTmp[i][1] + 1, 78, blockTmp[i][1] + 1, 0);   
         yLineTmp[yCounter] = blockTmp[i][1]; 
         yLine[yCounter] = blockTmp[i][1]; 
         yCounter++; 
         vgaTone(660,30); 
      }
      k = 0; 
    }
    if (yLineTmp[yCounter - 1] < yLine[0]) { // ------------ qui va capito se va < o > (penso >) ----------------------
       for (int i = 0; i < yCounter; i++) { // ------------- inversion ---------------------------------------
          yLine[i] = yLineTmp[yCounter - i - 1]; 
       }
    }
    for (int i = 0; i < yCounter; i++){   // ----------- block translation to lower position -------------- 
      for (int y = yLine[i] - 2; y > 0; y = y - 2) {
         for (int x = 45; x < 76; x += 3) {
            colorOld = vgaU.getpixel(x, y); 
            if (colorOld > 0) {
               vgaU.draw_line(x, y , x + 3, y , 0);
               vgaU.draw_line(x, y + 1, x + 3, y + 1, 0);
               vgaU.draw_line(x, y + 2, x + 3, y + 2, colorOld);
               vgaU.draw_line(x, y + 3, x + 3, y + 3, colorOld);
            } 
         }
      }
   }
   if (yCounter != 0) {
      score = score + 2*int(pow(2, yCounter));
      vgaTone(990,30); 
   } 
   drawScore(score); 
   yCounter = 0; 
}

//-----------------------------------------------------------------------------------------------
//--------------------- This is the main loop of the game ---------------------------------------
//-----------------------------------------------------------------------------------------------

ICACHE_RAM_ATTR void loopTetris() {
  processInputsTetris();
  if (noLoop < 1){ // --------------- to generate new Tetraminos --------------------------------
     blockN = blockNext; 
     if (noLoop == -1 ) { // -------------- only at the game beginning  -------------------------
        drawMenu(); 
     }
     drawGameScreen();
     drawScore(score);
     blockNext = 1 + int(random(7));
     blockDef(blockNext);
     drawBlockNext();
     blockDef(blockN);
     x = 57; 
     y = 5; 
     noLoop = 1; 
  }

  int buttonThreeStatusShortLong=0;
  if (buttonThreeStatus == 1 && buttonThreePressedTime == 0){
    buttonThreePressedTime=millis();
  }
  if (buttonThreeStatus == 1 && buttonThreePressedTime != 0){
    if (millis()-buttonThreePressedTime > 500) buttonThreeStatusShortLong=2;
  }
  if (buttonThreeStatus == 0 && buttonThreePressedTime != 0){
    if (millis()-buttonThreePressedTime <= 500) buttonThreeStatusShortLong=1;
    buttonThreePressedTime=0;
  }

  if (buttonThreeStatusShortLong == 1){ // ------------------------ rotation -------------------------
     //if (button_5 == 1){clock = -1;}
     if (buttonThreeStatus == 1){clock = 1;}
     delBlock();
     blockRotation(clock); 
     checkBlockRotation(); 
  }
  if (buttonOneStatus == 1 || buttonTwoStatus == 1){ // ------- translation ----------------------
     if (buttonTwoStatus == 1){delta = 3;}
     if (buttonOneStatus == 1){delta = -3;}
     delBlock();
     checkBlockTranslation();
  }
  ticks++; 
  if (ticks % fast > fast - 2 || buttonThreeStatusShortLong == 2){ // --- Tetraminos falling ----------
     if (fast < 3) {fast = 2;}
     y = y + 2;
     delBlock(); 
     replaceBlock(); 
  }
  else
  {vga.delay(10 + 2*fast);}
}
//-----------------------------------------------------------------------------------------------
//--------------------- This is the end of the main loop ----------------------------------------
//-----------------------------------------------------------------------------------------------

static void vgaTone(int freq, int ticks){
   vga.tone(freq);
   vga.delay(ticks); 
   vga.noTone(); 
}
