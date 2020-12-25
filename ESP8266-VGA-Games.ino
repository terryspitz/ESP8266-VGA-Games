/*******************************************************************
   ArduinoVGAgame - Version 2 - September the 29th 2017
*                                                                  *
   Arduino VGA Game is a game consolle for VGA monitors,
   written by Roberto Melzi, running for Arduino software
   IDE version 1.6.4
   VGAx library does not work for newer or elder software
   versions.
   The consolle includes the games:
   Pong, Breakout, Bomber, Etch-a-Sketch and Tetris.
*                                                                  *
   VGAx Library written By Sandro Maffiodo aka Smaffer.
   https://github.com/smaffer/vgax
   See also the Arduino forum:
   http://forum.arduino.cc/index.php?topic=328631.0
   and on Instructables:
   http://www.instructables.com/member/Rob%20Cai/instructables/
*                                                                  *
*******************************************************************/

#include "ESP8266-VGA-Games.h"
#include <math.h>

#ifdef ESP8266
#include <fonts/arial12.h>

#define WHEEL_ONE_PIN A0 //analog
#define WHEEL_TWO_PIN A0 //analog
#define BUTTON_1_PIN D3 //digital
#define BUTTON_2_PIN D4  //digital - do not use ESPVGAX_EXTRA_COLORS!
#define BUTTON_3_PIN D0 //digital

ESPVGAX vga;

#elif ESP32

#include "fabgl.h"
#include "fabutils.h"
// VGA PINS
const gpio_num_t redGPIO = GPIO_NUM_15;
const gpio_num_t greenGPIO = GPIO_NUM_18;
const gpio_num_t blueGPIO = GPIO_NUM_23;
const gpio_num_t HSyncGPIO = GPIO_NUM_13;
const gpio_num_t VSyncGPIO = GPIO_NUM_19;

const gpio_num_t WHEEL_ONE_PIN = GPIO_NUM_36;
const gpio_num_t WHEEL_TWO_PIN = GPIO_NUM_36;
//JOYSTICK_Y_GPIO = GPIO_NUM_39;
const gpio_num_t BUTTON_1_PIN = GPIO_NUM_16;
const gpio_num_t BUTTON_2_PIN = GPIO_NUM_17;
const gpio_num_t BUTTON_3_PIN = GPIO_NUM_1; //TODO - add this

VGAAdapter vga;

fabgl::VGAController displayController;
fabgl::Canvas        canvas(&displayController);
fabgl::SoundGenerator soundGenerator;
fabgl::SquareWaveformGenerator swg;

#endif


static const char str30[] PROGMEM = "Choose ESP8266 VGAx Game";
static const char str31[] PROGMEM = "Pong 2-player";
static const char str32[] PROGMEM = "Pong 1-player";
static const char str33[] PROGMEM = "Breakout";
static const char str34[] PROGMEM = "Bomber";
static const char str35[] PROGMEM = "Etch-a-Sketch";
static const char str36[] PROGMEM = "Tetris";
static const char str37[] PROGMEM = "Snake";

void setup() {
  //Serial.begin(9600);
  vga.begin();
  randomSeed(analogRead(WHEEL_ONE_PIN));
}

bool buttonOneStatus;
bool buttonTwoStatus;
bool buttonThreeStatus;
byte wheelOnePosition;
byte wheelTwoPosition;
bool enableWheels = true;

void drawStartMenu();

ICACHE_RAM_ATTR void processInputs() {
#ifndef ESPVGAX_READ_INPUTS
  byte newWheelOnePosition = 0;
  byte newWheelTwoPosition = 0;
  if (enableWheels) {
    pinMode(BUTTON_1_PIN, OUTPUT);
    pinMode(BUTTON_2_PIN, OUTPUT);
    digitalWrite(BUTTON_1_PIN, HIGH);
    digitalWrite(BUTTON_2_PIN, LOW);
    newWheelOnePosition = 127 - byte(analogRead(WHEEL_ONE_PIN) / 8); //to change direction of the wheel remove "127 -" ---------------------
    digitalWrite(BUTTON_1_PIN, LOW);
    digitalWrite(BUTTON_2_PIN, HIGH);
    newWheelTwoPosition = 127 - byte(analogRead(WHEEL_TWO_PIN) / 8);
    pinMode(BUTTON_1_PIN, INPUT_PULLUP);
    pinMode(BUTTON_2_PIN, INPUT_PULLUP);
    pinMode(BUTTON_3_PIN, INPUT);
  }
  buttonOneStatus = 1 - digitalRead(BUTTON_1_PIN);
  buttonTwoStatus = 1 - digitalRead(BUTTON_2_PIN);
  buttonThreeStatus = 1 - digitalRead(BUTTON_3_PIN);
  if (buttonOneStatus == 0) wheelOnePosition = newWheelOnePosition;
  if (buttonTwoStatus == 0) wheelTwoPosition = newWheelTwoPosition;
#else
// Prefered: Use extended ESPVGAX:
// Using analogRead() in loop() causes flickering.
// Therefore it is integrated into ESPVGAX withing vga_handler() to
// spread reading the inputs over multiple scan lines and
// hide it after the vertical blank interval.
  buttonOneStatus = vga.buttonOneStatus;
  buttonTwoStatus = vga.buttonTwoStatus;
  buttonThreeStatus = vga.buttonThreeStatus;
  wheelOnePosition = vga.wheelOnePosition;
  wheelTwoPosition = vga.wheelTwoPosition;
#endif
}

//------------------------------------------- shared functions ----------------------------------------------
#ifdef ESP32
RGB888 color_ESPVGAX_to_FabGL(uint8_t col) {
  return col == 0 ? Color::Black : col == 3 ? Color::White : col == 2 ? Color::Blue : Color::Green;
}
uint8_t color_FabGL_to_ESPVGAX(RGB888 col) {
  return col == Color::Black ? 0 : col == Color::White ? 3 : col == Color::Blue ? 2 : Color::Green ? 1 : 255;
}

void VGAAdapter::begin() {
  displayController.begin(redGPIO, greenGPIO, blueGPIO, HSyncGPIO, VSyncGPIO);
  displayController.setResolution(VGA_320x200_75Hz);

  // adjust this to center screen in your monitor
  displayController.moveScreen(0, 0);

  canvas.selectFont(fabgl::getPresetFontInfo(320, 300, 80, 32));

  soundGenerator.setVolume(126);
  soundGenerator.play(true);
  soundGenerator.attach(&swg);
}

void VGAAdapter::clear(uint8_t color) {
  canvas.setBrushColor(color_ESPVGAX_to_FabGL(color));
  canvas.clear();
}

void VGAAdapter::delay(int ms) {
  delay(ms);
}

void VGAAdapter::drawCircle(int x0, int y0, int radius, uint8_t color, bool fill) {
  canvas.setBrushColor(color_ESPVGAX_to_FabGL(color));
  canvas.setPenColor(color_ESPVGAX_to_FabGL(color));
  if (fill)
    canvas.fillEllipse(x0, y0, radius, radius);
  else
    canvas.drawEllipse(x0, y0, radius, radius);
}

void VGAAdapter::drawRect(int x, int y, int w, int h, uint8_t c, bool fill) {
  if(fill) {
    canvas.setBrushColor(color_ESPVGAX_to_FabGL(c));
    canvas.fillRectangle(x, y, x+w, y+h);
  }
  else {
    canvas.setPenColor(color_ESPVGAX_to_FabGL(c));
    canvas.drawRectangle(x, y, x+w, y+h);
  }
}

void VGAAdapter::drawLine(int x0, int y0, int x1, int y1, uint8_t c) {
  canvas.setPenColor(color_ESPVGAX_to_FabGL(c));
  canvas.drawLine(x0, y0, x1, y1);
}

uint8_t VGAAdapter::getpixel(int x, int y){
  return color_FabGL_to_ESPVGAX(canvas.getPixel(x,y));
}

void VGAAdapter::tone(int freq) {
  swg.setFrequency(freq);
  swg.enable(true);
}

void VGAAdapter::noTone() {
  swg.enable(false);  
}

#endif

void vgaTone(int freq, int time) {
  vga.tone(freq);
  vga.delay(time);
  vga.noTone();
}

void vgaPrint(const char* str, int x, int y, uint8_t color) {
#ifdef ESP8266
   if (color > 0) {
     vga.setFont((uint8_t*)fnt_arial12_data, FNT_ARIAL12_SYMBOLS_COUNT, FNT_ARIAL12_HEIGHT, FNT_ARIAL12_GLYPH_WIDTH);
     vga.print_P(str, x, y, true, -1, color == 0 ? ESPVGAX_OP_XOR : ESPVGAX_OP_OR, true);
   } else {
     int w = strlen(str) * 12;
     int h = 14;
     y -= 4;
     if (y<0) {
       h -= y;
       y = 0;
     }
     vga.drawRect(x, y, w, h, 0, true, ESPVGAX_OP_SET);
   }
#elif ESP32
  canvas.setPenColor(color_ESPVGAX_to_FabGL(color));
  canvas.drawText(x, y, str);
#endif
}

void vgaPrintNumber(byte number, int x, int y, uint8_t color) {
  char scoreChar[2];
  sprintf(scoreChar, "%d", number);
#ifdef ESP8266
  vga.setFont((uint8_t*)fnt_arial12_data, FNT_ARIAL12_SYMBOLS_COUNT, FNT_ARIAL12_HEIGHT, FNT_ARIAL12_GLYPH_WIDTH);
  vga.print_P(scoreChar, x, y, true, -1, color == 0 ? ESPVGAX_OP_XOR : ESPVGAX_OP_OR, true);
#elif ESP32
  vgaPrint(scoreChar, x, y, color);
#endif
}

// -------------------------------------- This is the main loop of the game -----------------------------------------------------------------------------------------------------
// ------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
byte state = 0;
ICACHE_RAM_ATTR void loop() {
  while (1) {
    processInputs();
    if (buttonOneStatus == 1 && buttonTwoStatus == 1 && buttonThreeStatus == 1) {
      while (buttonOneStatus == 1 || buttonTwoStatus == 1 || buttonThreeStatus == 1) {
        processInputs();
      }
      enableWheels = true;
      state = 0;
    }
    if (state == 1 || state == 0) {
      drawStartMenu();
    }
    if (state == 2) {
      loopPong();
    }
    if (state == 3) {
      loopBreakout();
    }
    if(state == 4) { 
      loopBomber(); 
    }
    if (state == 5) {
      loopDrawingToy();
    }
    if (state == 6) {
      loopTetris();
    }
    if (state == 7) {
      loopSnake();
    }
  }
}
// ------------------------------------------- end of main loop -----------------------------------------------------------------------------------------------------------------
// ------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

//------------------------------------------------------- start screen -------------------------------------------------------
int8_t ticPosition = 0;
const int8_t ticPositionEnd = 7;
void drawStartMenu() {
  enableWheels = false;
  if (state == 0) {
    vga.clear(0);
    vgaPrint(str30, 20, 5, 3);  // Choose a game
    vgaPrint(str31, 50, 30, 2); // pong
    vgaPrint(str32, 50, 50, 2); // pong single
    vgaPrint(str33, 50, 70, 2); // breakout
    vgaPrint(str34, 50, 90, 2); // bomber
    vgaPrint(str35, 50, 110, 2); // drawing toy
    vgaPrint(str36, 50, 130, 2); // tetris
    vgaPrint(str37, 50, 150, 2); // snake
    vga.drawCircle(35, ticPosition * 20 + 35, 5, 1, true);
    state = 1;
  }
  vga.delay(50);
  processInputs();
  int8_t tickMove = 0;
  if (buttonOneStatus == 1) { //game selector (X) moves down ---------------
    tickMove = 1;
  }
  if (buttonThreeStatus == 1) { //game selector (X) moves up ---------------
    tickMove = -1;
  }
  if (tickMove != 0) {
    vga.drawCircle(35, ticPosition * 20 + 35, 5, 0, true);
    ticPosition = (ticPosition + tickMove + ticPositionEnd) % ticPositionEnd;
    vga.drawCircle(35, ticPosition * 20 + 35, 5, 1, true);
    vga.delay(200);
  }
  if (buttonTwoStatus == 1) { //game choice ---------------------------
    while (buttonOneStatus == 1 || buttonTwoStatus == 1 || buttonThreeStatus == 1) {
      processInputs();
    }
    vga.clear(0);
    if (ticPosition == 0) { // Pong 2-player
      state = 2;
      enableWheels = true;
      setupPong(2);
    }
    if (ticPosition == 1) { // Pong 1-player
      state = 2;
      enableWheels = true;
      setupPong(1);
    }
    if (ticPosition == 2) { // Breakout
      state = 3;
      enableWheels = true;
      setupBreakout();
    }
    if (ticPosition == 3) { // Bomber
      state = 4;
         setupBomber();
    }
    if (ticPosition == 4) { // drawing Toy
      state = 5;
      enableWheels = true;
      setupDrawingToy();
    }
    if (ticPosition == 5) { // Tetris
      state = 6;
      enableWheels = false;
      setupTetris();
    }
    if (ticPosition == 6) { // Snake
      state = 7;
      enableWheels = false;
      setupSnake();
    }
  }
}
//----------------------------------------------- start screen end -----------------------
