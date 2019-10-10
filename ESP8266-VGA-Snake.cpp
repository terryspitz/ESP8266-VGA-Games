/*
  Arduino Snake for VGA monitor - by Roberto Melzi. 
  Version 1.1 - October the 10th, 2015
  with sound
  Arduino IDE 1.6.4 
  Version 1.4 (December the 13th, 2016)
  snake array in byte, more memory for variables available, 
  allowing lenght increment of 5 pixels instead of 3. 
  Version 1.5 (November the 12th, 2017) 
  other variables in byte, score up to 10, adding vgaPrint() and vgaPrintNumber, 
*/

#include "ESP8266-VGA-Games.h"
#include <math.h>
#include "ESPVGAXUtils.h"
static const int ESPVGAX_SCALEX=8;
static const int ESPVGAX_SCALEY=8;
static const int VGAX_WIDTH=(ESPVGAX_WIDTH/8);
static const int VGAX_HEIGHT=(ESPVGAX_HEIGHT/8);
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
static const char str20[] PROGMEM="Arduino VGA Snake"; 
static const char str21[] PROGMEM="by Roberto Melzi"; 
static const char str22[] PROGMEM="Game Over"; 
static const char str23[] PROGMEM="Score"; 
static const char str24[] PROGMEM="You Win!"; 

static byte counterMenu = 0;
static byte counterMenu2 = 0; 
static byte state = 1;
static byte score = 0; 
static byte scoreMax = 10; 
static byte foodX = 60; 
static byte foodY = 30;
static byte snakeMaxLength = 55; 
static byte sx[55];      // > slength + scoreMax*delta + 1 = 40
static byte sy[55];
static byte slength = 3; // snake starting length 
static byte delta = 5;   // snake length increment 
static byte wleft = 8; 
static int i;
static byte x; 
static byte y; 
static byte direct = 3; 
static int speedDelay = 200; 

static void drawSnakeIni();
static void toneSafe(int freq, int duration);

void setupSnake() {
  state = 1;
}

static void foodIni() {
  do{
     foodX = random(VGAX_WIDTH - 4 - wleft) + 2 + wleft;  
     foodY = random(VGAX_HEIGHT - 4) + 2;
     // ------------ choose the following for food up to the border ----------------------------------------- 
     // foodX = random(VGAX_WIDTH - 2 - wleft) + 1 + wleft;  
     // foodY = random(VGAX_HEIGHT - 2) + 1;
  } while ( vga.getpixel(foodX, foodY) > 1 ); 
}

static void drawMenu() {
  counterMenu2++; 
  vga.delay(10); 
  if (counterMenu2 > 50){
    counterMenu++; 
    vgaPrint(str20, 26, 16, (counterMenu%3) + 1);
    vgaPrint(str21, 28, 38, (counterMenu%3) + 1);
    counterMenu2 = 0; 
  }
}

static void drawBorder() {
    vgaU.draw_line(wleft, 0, VGAX_WIDTH-1, 0, 3);
    vgaU.draw_line(wleft, VGAX_HEIGHT-1, VGAX_WIDTH-1, VGAX_HEIGHT-1, 3);
    vgaU.draw_line(wleft, 0, wleft, VGAX_HEIGHT-1, 3);
    vgaU.draw_line(VGAX_WIDTH-1, 0, VGAX_WIDTH-1, VGAX_HEIGHT, 3);
}

static void drawScore() {
  vgaPrint(str23, 10, 20, 2);
  ESPVGAX::drawRect(20, 40, 40, 20, 0, true, ESPVGAX_OP_SET);
  vgaPrintNumber(score, 20, 40, 2);
}

// this is for the beginning game window ---------------------------------------------------------------------------------------
static void drawStartScreen() {
   vga.clear(0);
   drawBorder(); 
   drawSnakeIni(); 
   drawScore(); 
   vga.delay(200);
} 

static void drawSnakeIni() {
   for (byte i = 0; i < slength ; i++) {
      sx[i] = 80 + i;
      sy[i] = 30; 
      vgaU.putpixel(int(sx[i]), int(sy[i]), 2);
   }
   for (byte i = slength; i < snakeMaxLength ; i++) {
     sx[i] = 1;
     sy[i] = 1; 
  }
   vgaU.putpixel(foodX, foodY, 1);
}

// re-inizialize new match -----------------------------------------------------------------------
static void newMatch(){
  score = 0;
  slength = 3; 
  i = slength - 1;  
  vga.clear(0);  
  drawBorder();
  drawScore(); 
  vgaU.putpixel(foodX, foodY, 1);
}

// This is the main loop of the game ------------------------------------------------------------
void loopSnake() {
  
  if(state == 1) { 
        vga.clear(0);
        foodIni();
        newMatch();
        drawStartScreen(); 
        state = 2; 
  }
  
 if(state == 2){
     //if (buttonOneStatus == 1){ 
        score = 0;
        drawScore(); 
        direct = 3; 
        x = -1;
        y = 0; 
        i = slength - 1; 
        state = 3; 
     //}
  }
  
  if(state == 3) { 
     //-------------------- change direction --------------------------------------------
     if (direct == 1){ // right
        if (buttonOneStatus == 1){ x = 0; y = -1; direct = 2;}
        if (buttonTwoStatus == 1){ x = 0; y = +1; direct = 4;}
     }
     else {
        if (direct == 2){ // up
           if (buttonTwoStatus == 1){ x = +1; y = 0; direct = 1;}
           if (buttonOneStatus == 1){ x = -1; y = 0; direct = 3;}
        }
        else {
           if (direct == 3){ // left
              if (buttonTwoStatus == 1){ x = 0; y = -1; direct = 2;}
              if (buttonOneStatus == 1){ x = 0; y = +1; direct = 4;}
           }
           else { 
              if (direct == 4){ // down
                 if (buttonOneStatus == 1){ x = +1; y = 0; direct = 1;}
                 if (buttonTwoStatus == 1){ x = -1; y = 0; direct = 3;}
              }
           }
        }
     }

//----------------------- delete tail --------------------------------------     
     vgaU.putpixel(int(sx[i]), int(sy[i]), 0);
     if ( i == slength - 1){
        sx[i] = sx[0] + x; 
        sy[i] = sy[0] + y; 
     }
     else {
        sx[i] = sx[i + 1] + x; 
        sy[i] = sy[i + 1] + y; 
     }

//--------------------- out from border ------------------------------------    

     if(sx[i] < wleft + 1) {sx[i] = VGAX_WIDTH - 2;}
     if(sx[i] > VGAX_WIDTH - 2) {sx[i] = wleft + 1;}
     if(sy[i] < 1) {sy[i] = VGAX_HEIGHT - 2;}
     if(sy[i] > VGAX_HEIGHT - 2) {sy[i] = 1;}
     
//--------------------- check eating food -----------------------------------
     if ( sx[i] == foodX && sy[i] == foodY){ 
     //if (vgaU.getpixel(int(sx[i]), int(sy[i])) == 1 ){
        toneSafe(660,30);  
        foodIni(); 
        drawBorder(); 
        vgaU.putpixel(foodX, foodY, 1);
        slength = slength + delta; 
        score++; 
        if (score > scoreMax) {
           speedDelay = speedDelay - 20;
           toneSafe(880,30);  
           vgaPrint(str24, 5, 80, 2);
           vga.delay(1000);
           while(buttonOneStatus == 0){processInputs();vga.delay(100);}
           newMatch();
           state = 2;
        }
        drawScore(); 
     }

//----------------------- increase head and Game Over -------------------------------------
     if (vgaU.getpixel(int(sx[i]), int(sy[i])) == 0 || vgaU.getpixel(int(sx[i]), int(sy[i])) == 1 ){
        vgaU.putpixel(int(sx[i]), int(sy[i]), 2); 
     }
     else //------------------ Game Over --------------------------------------------------
     {
        vgaPrint(str22, 58, 24, 1);
        vga.delay(1000);
        toneSafe(660, 200); 
        toneSafe(330, 200);
        toneSafe(165, 200); 
        toneSafe(82, 200);
        while(buttonOneStatus == 0){processInputs();vga.delay(100);}
        speedDelay = 100; 
        newMatch();
        drawSnakeIni();
     }
   
     i--;
     if ( i < 0){i = slength - 1;}
 
     vga.delay(buttonThreeStatus == 1 ? speedDelay / 5 : speedDelay); 

  }
} 

static void toneSafe(int freq, int duration) {
   vga.tone(freq);  
   vga.delay(duration); 
   vga.noTone(); 
}
