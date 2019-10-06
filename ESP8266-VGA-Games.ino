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

#include <ESPVGAX.h>
#include <math.h>
#include <fonts/arial12.h>

#define WHEEL_ONE_PIN A0 //analog
#define WHEEL_TWO_PIN A0 //analog
#define BUTTON_1_PIN D3 //digital
#define BUTTON_2_PIN D4  //digital - do not use ESPVGAX_EXTRA_COLORS!
#define BUTTON_3_PIN D0 //digital

ESPVGAX vga;

static const char str30[] PROGMEM="Choose Arduino Game"; 
static const char str31[] PROGMEM="Pong"; 
static const char str32[] PROGMEM="Breakout"; 
static const char str33[] PROGMEM="Bomber"; 
static const char str34[] PROGMEM="Etch-a-Sketch"; 
static const char str35[] PROGMEM="Tetris"; 

void setup() {
  //Serial.begin(9600); 
  vga.begin();
  randomSeed(analogRead(WHEEL_ONE_PIN));
}

boolean buttonOneStatus; 
boolean buttonTwoStatus; 
boolean buttonThreeStatus;
byte wheelOnePosition; 
byte wheelTwoPosition;

ICACHE_RAM_ATTR void processInputs() {
  pinMode(BUTTON_1_PIN,OUTPUT);
  pinMode(BUTTON_2_PIN,OUTPUT);
  digitalWrite(BUTTON_1_PIN,HIGH);
  digitalWrite(BUTTON_2_PIN,LOW);
  wheelOnePosition = 127 - byte(analogRead(WHEEL_ONE_PIN)/8); //to change direction of the wheel remove "127 -" ---------------------
  digitalWrite(BUTTON_1_PIN,LOW);
  digitalWrite(BUTTON_2_PIN,HIGH);
  wheelTwoPosition = 127 - byte(analogRead(WHEEL_TWO_PIN)/8); 
  pinMode(BUTTON_1_PIN,INPUT_PULLUP);
  pinMode(BUTTON_2_PIN,INPUT_PULLUP);
  pinMode(BUTTON_3_PIN,INPUT);
  buttonOneStatus = 1-digitalRead(BUTTON_1_PIN);
  buttonTwoStatus = 1-digitalRead(BUTTON_2_PIN);
  buttonThreeStatus = 1-digitalRead(BUTTON_3_PIN);
}

//------------------------------------------- the next three voids are shared ----------------------------------------------

void vgaPrint(const char* str, byte x, byte y, byte color){
   vga.setFont((uint8_t*)fnt_arial12_data, FNT_ARIAL12_SYMBOLS_COUNT, FNT_ARIAL12_HEIGHT, FNT_ARIAL12_GLYPH_WIDTH);
   vga.print_P(str, x, y, true, -1, color == 0 ? ESPVGAX_OP_XOR : ESPVGAX_OP_OR, true);
}

void vgaPrintNumber(byte number, byte x, byte y, byte color){
   char scoreChar[2];
   sprintf(scoreChar,"%d",number);
   vga.setFont((uint8_t*)fnt_arial12_data, FNT_ARIAL12_SYMBOLS_COUNT, FNT_ARIAL12_HEIGHT, FNT_ARIAL12_GLYPH_WIDTH);
   vga.print_P(scoreChar, x, y, true, -1, color == 0 ? ESPVGAX_OP_XOR : ESPVGAX_OP_OR, true);
}

// -------------------------------------- This is the main loop of the game -----------------------------------------------------------------------------------------------------
// ------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
byte state = 0; 
ICACHE_RAM_ATTR void loop() {
 while(1) {
  processInputs();
  if(buttonOneStatus == 1 && buttonTwoStatus == 1 && buttonThreeStatus == 1) {
    while(buttonOneStatus == 1 || buttonTwoStatus == 1 || buttonThreeStatus == 1) {
      processInputs();
    }
    state = 0;
  }
  if(state == 1 || state == 0) { drawStartMenu(); } 
  if(state == 2) { loopPong(); }
/*
  if(state == 3) { loopBreakout(); }
  if(state == 4) { loopBomber(); }
  if(state == 5) { loopDrawingToy(); }
  if(state == 6) { loopTetris(); }
 */
 }
} 
// ------------------------------------------- end of main loop -----------------------------------------------------------------------------------------------------------------
// ------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

//------------------------------------------------------- start screen -------------------------------------------------------
int8_t ticPosition = 0;
const int8_t ticPositionEnd = 5;
void drawStartMenu(){
   if (state == 0) {
      vga.clear(0);
      vgaPrint(str30, 20, 5, 3);  // Choose a game
      vgaPrint(str31, 50, 30, 2); // pong
      vgaPrint(str32, 50, 50, 2); // breakout
      vgaPrint(str33, 50, 70, 2); // bomber 
      vgaPrint(str34, 50, 90, 2); // drawing toy 
      vgaPrint(str35, 50, 110, 2); // tetris 
      vga.drawCircle(35, ticPosition*20+35, 5, 1, true, ESPVGAX_OP_XOR); 
      state = 1;
   }
   vga.delay(50);
   processInputs();
   int8_t tickMove = 0;
   if (buttonOneStatus == 1){ //game selector (X) moves down ---------------
      tickMove = 1;
   }
   if (buttonThreeStatus == 1){ //game selector (X) moves up ---------------
      tickMove = -1;
   }
   if (tickMove != 0){
      vga.drawCircle(35, ticPosition*20+35, 5, 1, true, ESPVGAX_OP_XOR); 
      ticPosition = (ticPosition+tickMove) % ticPositionEnd; 
      vga.drawCircle(35, ticPosition*20+35, 5, 1, true, ESPVGAX_OP_XOR); 
      vga.delay(200);
   }
   if (buttonTwoStatus == 1){ //game choice ---------------------------
      while(buttonOneStatus == 1 || buttonTwoStatus == 1 || buttonThreeStatus == 1) {
        processInputs();
      }
      vga.clear(0);
      if (ticPosition == 0) { // Pong
         state = 2;
         setupPong(); 
      } 
      if (ticPosition == 1) { // Breakout 
         state = 3;
         //setupBreakout();
      } 
      if (ticPosition == 2) { // Bomber
         state = 4;
         //setupBomber();
      } 
      if (ticPosition == 3) { // drawing Toy 
         state = 5;
         //setupDrawingToy();
      } 
      if (ticPosition == 4) { // Tetris
         state = 6;
         //setupTetris();
      } 
   }
}
//----------------------------------------------- start screen end -----------------------
