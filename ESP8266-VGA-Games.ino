/*******************************************************************
   ArduinoVGAgame - Version 2.1

   Arduino VGA Game is a game consolle for VGA monitors,
   written by Roberto Melzi, running for Arduino software
   IDE version 1.6.4
   VGAx library does not work for newer or elder software
   versions.
   The console includes the games:
   Pong, Breakout, Bomber, Etch-a-Sketch and Tetris.

   VGAx Library written By Sandro Maffiodo aka Smaffer.
   https://github.com/smaffer/vgax
   See also the Arduino forum:
   http://forum.arduino.cc/index.php?topic=328631.0
   and on Instructables:
   http://www.instructables.com/member/Rob%20Cai/instructables/

   ESP32 port by terryspitz, Xmas 2020
   using ESP32 VGA library http://www.fabglib.org/ by Fabrizio Di Vittorio
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
const gpio_num_t VGA_RED_PIN = GPIO_NUM_15;
const gpio_num_t VGA_GREEN_PIN = GPIO_NUM_18;
const gpio_num_t VGA_BLUE_PIN = GPIO_NUM_23;
const gpio_num_t VGA_HSYNC_PIN = GPIO_NUM_13;
const gpio_num_t VGA_VSYNC_PIN = GPIO_NUM_19;

//switch these depending on your input type
#define JOYSTICK 1;  //e.g. https://exploreembedded.com/wiki/Analog_JoyStick_with_Arduino
//#define POTENTIOMETER 1; //e.g. https://www.arduino.cc/en/tutorial/potentiometer

const gpio_num_t WHEEL_ONE_PIN = GPIO_NUM_36; //JOYSTICK_X
//GPIO_NUM_39; JOYSTICK_Y
const gpio_num_t WHEEL_TWO_PIN = (gpio_num_t)NULL; //not installed on my hardware
const gpio_num_t BUTTON_1_PIN = GPIO_NUM_16;
const gpio_num_t BUTTON_2_PIN = GPIO_NUM_17;
const gpio_num_t BUTTON_3_PIN = GPIO_NUM_1; //TODO - add this switch or adapt code

VGAAdapter vga;

fabgl::VGAController displayController;
fabgl::Canvas        canvas(&displayController);
fabgl::SoundGenerator soundGenerator;
fabgl::SquareWaveformGenerator swg;


#endif


static const char str30[] PROGMEM = "Choose Your Game:";
static const char str31[] PROGMEM = "Pong 2-player";
static const char str32[] PROGMEM = "Pong 1-player";
static const char str33[] PROGMEM = "Breakout";
static const char str34[] PROGMEM = "Bomber";
static const char str35[] PROGMEM = "Etch-a-Sketch";
static const char str36[] PROGMEM = "Tetris";
static const char str37[] PROGMEM = "Snake";
static const char str38[] PROGMEM = "ESP VGA Games by Roberto Melzi, rob_cai, matgoebl, terryspitz";

void setup() {
  pinMode(BUTTON_1_PIN, INPUT_PULLUP);
  pinMode(BUTTON_2_PIN, INPUT_PULLUP);
  pinMode(BUTTON_3_PIN, INPUT_PULLUP);

  Serial.begin(115200);
  Serial.println("Started");

  vga.begin();

  randomSeed(analogRead(WHEEL_ONE_PIN));
}

bool buttonOneStatus;
bool buttonTwoStatus;
bool buttonThreeStatus;
byte wheelOnePosition = 64;
byte wheelTwoPosition = 64;
bool enableWheels = true;
byte wheelOnePosition0 = 0;
byte wheelTwoPosition0 = 0;

void drawStartMenu();

ICACHE_RAM_ATTR void processInputs() {
#if !defined(ESPVGAX_READ_INPUTS) || defined(ESP32)
  byte newWheelOnePosition = 0;
  byte newWheelTwoPosition = 0;
  if (enableWheels) {
#ifdef ESP8266
    /*ESP8266 only has one Analog Digital Convertor (ADC),
      so the two wheels/pots/joysticks are wired via two transistors or
      two diodes to allow both to be read on the single ADC pin.
      for more info google: "esp8266 adc multiplexing diode"
    */
    const int MAX_ANALOG_READ = 1024;
    const int SCALE = MAX_ANALOG_READ / 128;
    pinMode(BUTTON_1_PIN, OUTPUT);
    pinMode(BUTTON_2_PIN, OUTPUT);
    digitalWrite(BUTTON_1_PIN, HIGH);
    digitalWrite(BUTTON_2_PIN, LOW);
    newWheelOnePosition = 127 - byte(analogRead(WHEEL_ONE_PIN) / SCALE); //to change direction of the wheel remove "127 -" ---------------------
    digitalWrite(BUTTON_1_PIN, LOW);
    digitalWrite(BUTTON_2_PIN, HIGH);
    newWheelTwoPosition = 127 - byte(analogRead(WHEEL_TWO_PIN) / SCALE);
    pinMode(BUTTON_1_PIN, INPUT_PULLUP);
    pinMode(BUTTON_2_PIN, INPUT_PULLUP);
    pinMode(BUTTON_3_PIN, INPUT_PULLUP);
#ifdef JOYSTICK
    if (buttonOneStatus == 0)
      wheelOnePosition = constrain(wheelOnePosition + (newWheelOnePosition - wheelOnePosition0) / 16, 0, 127);
    if (buttonTwoStatus == 0)
      wheelTwoPosition = constrain(wheelTwoPosition + (newWheelTwoPosition - wheelTwoPosition0) / 16, 0, 127);
#else
    if (buttonOneStatus == 0)
      wheelOnePosition = newWheelOnePosition;
    if (buttonTwoStatus == 0)
      wheelTwoPosition = newWheelTwoPosition;
#endif
#elif ESP32
    const int MAX_ANALOG_READ = 4096;
    const int SCALE = MAX_ANALOG_READ / 128;
    newWheelOnePosition = 127 - byte(analogRead(WHEEL_ONE_PIN) / SCALE); //to change direction of the wheel remove "127 -" ---------------------
    newWheelTwoPosition = /*127 - */ byte(analogRead(WHEEL_TWO_PIN) / SCALE);
#ifdef JOYSTICK
    if (wheelOnePosition0 == 0) {
      wheelOnePosition0 = newWheelOnePosition;
      wheelTwoPosition0 = newWheelTwoPosition;
#if 0
      Serial.println("newWheelOnePosition " +
                     String(newWheelOnePosition) + " " +
                     "newWheelTwoPosition " +
                     String(newWheelTwoPosition));
#endif
    }
    wheelOnePosition = constrain(wheelOnePosition + (newWheelOnePosition - wheelOnePosition0) / 16, 0, 127);
    wheelTwoPosition = constrain(wheelTwoPosition + (newWheelTwoPosition - wheelTwoPosition0) / 16, 0, 127);
#else
    wheelOnePosition = newWheelOnePosition;
    wheelTwoPosition = newWheelTwoPosition;
#endif
#if 0
    Serial.println(String(newWheelOnePosition) + " " +
                   String(wheelOnePosition) + " " +
                   String(newWheelTwoPosition) + " " +
                   String(wheelTwoPosition));
#endif
#endif
  }
  buttonOneStatus = 1 - digitalRead(BUTTON_1_PIN);
  buttonTwoStatus = 1 - digitalRead(BUTTON_2_PIN);
  buttonThreeStatus = 1 - digitalRead(BUTTON_3_PIN);
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
  displayController.begin(VGA_RED_PIN, VGA_GREEN_PIN, VGA_BLUE_PIN, VGA_HSYNC_PIN, VGA_VSYNC_PIN);
  displayController.setResolution(VGA_512x448_60Hz);
  // adjust this to center screen in your monitor
  displayController.moveScreen(100, 100);

  canvas.selectFont(fabgl::getPresetFontInfo(ESPVGAX_WIDTH, ESPVGAX_HEIGHT, 40, 32));

  soundGenerator.setVolume(126);
  soundGenerator.play(true);
  soundGenerator.attach(&swg);
}

void VGAAdapter::clear(uint8_t color) {
  canvas.setBrushColor(color_ESPVGAX_to_FabGL(color));
  canvas.clear();
}

void VGAAdapter::delay(int ms) {
  ::delay(ms);
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
  if (fill) {
    canvas.setBrushColor(color_ESPVGAX_to_FabGL(c));
    canvas.fillRectangle(x, y, x + w, y + h);
  }
  else {
    canvas.setPenColor(color_ESPVGAX_to_FabGL(c));
    canvas.drawRectangle(x, y, x + w, y + h);
  }
}

void VGAAdapter::drawLine(int x0, int y0, int x1, int y1, uint8_t c) {
  canvas.setPenColor(color_ESPVGAX_to_FabGL(c));
  canvas.drawLine(x0, y0, x1, y1);
}

uint8_t VGAAdapter::getpixel(int x, int y) {
  return color_FabGL_to_ESPVGAX(canvas.getPixel(x, y));
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
    if (y < 0) {
      h -= y;
      y = 0;
    }
    vga.drawRect(x, y, w, h, 0, true, ESPVGAX_OP_SET);
  }
#elif ESP32
  canvas.setPenColor(color_ESPVGAX_to_FabGL(color));
  canvas.drawText(x, y, str);
  Serial.println(str);
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
    // Press all three buttons to reset
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
    if (state == 4) {
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
const int dy = 20;
void drawStartMenu() {
  enableWheels = false;
  if (state == 0) {
    Serial.println("drawStartMenu()");
    vga.clear(0);
    //draw a border, helps to check screen is centred/all visible
    for (char i = 1; i < 4; ++i) {
      vga.drawRect(i * 2, i * 2, ESPVGAX_WIDTH - 1 - i * 4, ESPVGAX_HEIGHT - 1 - i * 4, i);
    }
    int y = 0;
    vgaPrint(str30, 20, ++y * dy, 3); // Choose a game
    vgaPrint(str31, 50, ++y * dy, 2); // pong
    vgaPrint(str32, 50, ++y * dy, 2); // pong single
    vgaPrint(str33, 50, ++y * dy, 2); // breakout
    vgaPrint(str34, 50, ++y * dy, 2); // bomber
    vgaPrint(str35, 50, ++y * dy, 2); // drawing toy
    vgaPrint(str36, 50, ++y * dy, 2); // tetris
    vgaPrint(str37, 50, ++y * dy, 2); // snake
    vgaPrint(str38, 20, ESPVGAX_HEIGHT - 50, 1); // credits
    vga.drawCircle(35, ticPosition * dy + dy * 2 + 5, 5, 1, true);
    state = 1;
  }
  vga.delay(50);
  processInputs();
  int8_t tickMove = 0;
  if (buttonOneStatus == 1) { //game selector (X) moves down ---------------
    Serial.println("move down");
    tickMove = 1;
  }
  if (buttonThreeStatus == 1) { //game selector (X) moves up ---------------
    Serial.println("move up");
    tickMove = -1;
  }
  if (tickMove != 0) {
    vga.drawCircle(35, ticPosition * dy + dy * 2 + 5, 5, 0, true);
    ticPosition = (ticPosition + tickMove + ticPositionEnd) % ticPositionEnd;
    vga.drawCircle(35, ticPosition * dy + dy * 2 + 5, 5, 1, true);
    vga.delay(200);
  }
  if (buttonTwoStatus == 1) { //game choice ---------------------------
    Serial.println("Selected game");
    while (buttonOneStatus == 1 || buttonTwoStatus == 1 || buttonThreeStatus == 1) {
      processInputs();
    }
    vga.clear(0);
    if (ticPosition == 0) { // Pong 2-player
      state = 2;
      enableWheels = true;
      Serial.println("start pong(2)");
      setupPong(2);
    }
    if (ticPosition == 1) { // Pong 1-player
      state = 2;
      enableWheels = true;
      Serial.println("start pong(1)");
      setupPong(1);
    }
    if (ticPosition == 2) { // Breakout
      state = 3;
      enableWheels = true;
      Serial.println("start breakout");
      setupBreakout();
    }
    if (ticPosition == 3) { // Bomber
      state = 4;
      Serial.println("start bomber");
      setupBomber();
    }
    if (ticPosition == 4) { // drawing Toy
      state = 5;
      enableWheels = true;
      Serial.println("start drawing toy");
      setupDrawingToy();
    }
    if (ticPosition == 5) { // Tetris
      state = 6;
      enableWheels = false;
      Serial.println("start tetris");
      setupTetris();
    }
    if (ticPosition == 6) { // Snake
      state = 7;
      enableWheels = false;
      Serial.println("start snake");
      setupSnake();
    }
  }
}
//----------------------------------------------- start screen end -----------------------
