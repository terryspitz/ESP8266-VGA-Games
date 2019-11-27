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
static const int VGAX_WIDTH=(ESPVGAX_WIDTH/ESPVGAX_SCALEX);
static const int VGAX_HEIGHT=(ESPVGAX_HEIGHT/ESPVGAX_SCALEY);
static ESPVGAXUtils vgaU(ESPVGAX_SCALEX,ESPVGAX_SCALEY);

static const char str51[] PROGMEM="~"; //planeR
static const char str52[] PROGMEM="%"; //planeL
static const char str53[] PROGMEM="$"; //bomb
static const char str54[] PROGMEM="'"; //rectangle 5 x 6
static const char str59[] PROGMEM="'''''''"; //clear 'Hit' banner 
static const char str510[] PROGMEM="Hit!!!"; //'Hit' banner
static const char str511[] PROGMEM="Arduino VGA Bomber"; 
static const char str515[] PROGMEM="Game Over!!!"; 

//-------------------------------- variables definition Pong ----------------------------
static byte state = 1; 
static byte shot; // pong, bomber, breakout 
static byte ballX; 
static byte ballY; 
static int beginning; // deve restare int poichè può assumere valori negativi, anche in breakout e bombard 
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
static byte lives = 8; //---------- qui lascio lives = 8 per pong. E' definito anche in fondo a parameterPongIni() ----------------
static byte ballX0;
static byte ballY0; 
static byte iDel;
static byte jDel; 
static byte ballXhit; 
static byte ballYhit; 
//----------------------------------------- Variable definition Bomber ---------------------------------------
static int bombX; 
static byte bomberVariableA; //only in bomber
static byte score; 
static byte bomberVariableB; //only in bomber
//----------------------------------------------------Variables definition end --------------------------------

//------------------------------------------------------------------------------------------------------

void setupBomber() { // Bomber 
  shot = 12; 
  score = 80; 
  speedX = 256*(10 + random(VGAX_WIDTH - 20));
  speedY = 10 + random(VGAX_HEIGHT/4);
  bomberVariableA = byte(speedX/256);
  ballVx = speedY + 4;
  speedT = 25 + random(50); 
  variableFloat_2 = speedT/256. - random(-10,10)/100.; 
  variableFloat_1 = random(10)/200.; 
  lives = 0;
  buttonOneStatus = 0; 
}

static void drawPlaneBomber() { // --------------- draw plane ----------------------------------------------  
   if(speedT > 0 && ballX != ballX0) {
      vgaPrint(str54, ballX0, ballY0, 0); 
      vgaPrint(str51, ballX, ballY, 1);
   }
   if(speedT < 0 && ballX != ballX0) {
      vgaPrint(str54, ballX0, ballY0, 0); 
      vgaPrint(str52, ballX, ballY, 1);
   }
}

static void drawBombBomber() { // -------------------- draw bomb -----------------------------------------------
   vgaPrint(str53, bomberVariableB, ballVy, 0);
   vgaPrint(str53, bombX, byte(ballPY/100), 2);
}

static void drawBombBomberard() { // ----------------- draw bombard --------------------------------------------
    if (padPosition != padPositionOld) {
      vgaU.draw_column(padPositionOld, VGAX_HEIGHT-4, VGAX_HEIGHT-7, 0); 
      vgaU.draw_row(VGAX_HEIGHT-4, padPositionOld-2, padPositionOld+3, 0); 
      vgaU.draw_row(VGAX_HEIGHT-3, padPositionOld-5, padPositionOld+6, 0); 
      vgaU.draw_row(VGAX_HEIGHT-2, padPositionOld-5, padPositionOld+6, 0); 
     
      vgaU.draw_column(padPosition, VGAX_HEIGHT-4, VGAX_HEIGHT-7, 3); 
      vgaU.draw_row(VGAX_HEIGHT-4, padPosition-2, padPosition+3, 3); 
      vgaU.draw_row(VGAX_HEIGHT-3, padPosition-5, padPosition+6, 3); 
      vgaU.draw_row(VGAX_HEIGHT-2, padPosition-5, padPosition+6, 3); 
   }
}

static void drawShotBomber() { // --------------------- draw shot ----------------------------------------------
   if ( beginning == shot - 1) {
      vgaU.draw_column(padPositionOld, 2, VGAX_HEIGHT - 7, 0);
   }
   if ( beginning == shot) {
      vgaU.draw_column(padPosition, 2, VGAX_HEIGHT - 7, 1);
      vga.delay(5); 
   }
}

static void drawScoreBomber() { // Bomber
   vgaU.draw_line(VGAX_WIDTH - (116 - 1), 1, VGAX_WIDTH - score/2 - 1, 1, 0);  
   vga.putpixel(VGAX_WIDTH - 116, 1, (lives + 2)%3 + 1);
   vgaU.draw_line(VGAX_WIDTH - 1 , 1, VGAX_WIDTH - score/2, 1, 3);  
}

static void drawGameScreenBomber() { // ----------------- main function to draw Bomber game screen ---------------------------
   drawPlaneBomber();
   drawBombBomber();
   drawScoreBomber(); 
   vgaU.draw_row(VGAX_HEIGHT-1, VGAX_WIDTH, 1, 2); 
   drawBombBomberard(); 
   drawShotBomber(); 
}

static void gameOverSoundBomber() { // Bomber
   vga.delay(300);
   vgaTone(660, 200); 
   vgaTone(330, 200);
   vgaTone(165, 200); 
   vgaTone(82, 200);
}

static void bomberHitSoundBomber() { //Bomber
   vgaTone(880, 200); 
   vgaTone(440, 200); 
   vgaTone(220, 200);
   vgaTone(110, 200); 
}

static void nextlivesSoundBomber() { //Bomber
   vgaTone(440, 100);
   vgaTone(880, 100);
}

// ---------------------------------------- Bomber---------------------------------------------------------------------------------------------------------------
void loopBomber() { 
     if (beginning == shot) {  // shot button has been hit ----------------------------------
        if (padPosition > int(speedX/256) - 2 - lives && padPosition < int(speedX/256) + 2 + lives) { // plane is hit -------------- 
           speedX = -2560; 
           score += 16;
           vgaTone(110, 60);
         }
         if (padPosition > (bombX - 2 ) && padPosition < (bombX + 2)) { // bomb is hit ----------------
            ballVx = speedY + 4;
            bomberVariableA = byte(speedX/256);
            variableFloat_2 = speedT/256. - 0.1*lives + random(-100,100)/1000.; 
            variableFloat_1 = 0.1*lives + random(100)/1000.; 
            iDel = 0;
            score += 16; 
            vgaTone(165, 60);
          }
     }     
 
     if( int(speedX/256) > VGAX_WIDTH || speedX < 0) {  // -------------- plane has escaped -----------------------------
        if (score > 2*(3 + lives)) { score += -2*(3 + lives); } else {score = 0;}
        speedT = random(50) + 50 + 50*lives; 
        speedY = random(VGAX_HEIGHT*2/3*100)/100 + 2; 
        if(random(1000) > 500)
           {speedX = 2560;} 
        else {
           speedX = 256*(VGAX_WIDTH - 10); 
           speedT = -speedT; 
        }
     }
     
     speedX += speedT; // plane next position -------------------------------------------------------
                                              
     bombX = bomberVariableA + variableFloat_2*iDel; // bomb position and trajectory ----------------------------------------------
     ballPY = 100.*ballVx + 60.*variableFloat_1*iDel + 8.*iDel*iDel/(60. - 20.*lives); //NB do not forget the "." otherwise ballPY will be an integer
     if(ballPY > 100*(VGAX_HEIGHT - 5) || bombX < 0 || bombX > VGAX_WIDTH){ //---------- the bomb hits the ground ------------------------------------
        if(bombX >= padPosition - 6 && bombX < padPosition + 6){ //---------------- the bomber has been hit ------------------------------------------
           vgaPrint(str510, 50, 10, 1); 
           bomberHitSoundBomber(); 
           vgaPrint(str59, 50, 10, 0); 
           vgaPrint(str59, 51, 10, 0); 
           if (score > 60) { score += -60; } else {score = 0;}
        }
        ballVx = speedY + 4;
        bomberVariableA = byte(speedX/256);
        variableFloat_2 = speedT/256 - 0.1*lives + random(-100,100)/1000.; 
        variableFloat_1 = 0.1*lives + random(100)/1000.; 
        iDel = 0;
     }
     else {iDel = iDel + 1;}
     // check the shot ---------------------------------------------------------
     if(beginning != 0 ) {
        beginning += -1; 
     }
       if ( buttonOneStatus == 1 && beginning == 0) {
          if (score > 1) { score += -1; } else {score = 0;} //******************************** essendo score raddoppiato, qui va lasciato -1 ****************************
          beginning = shot; 
          padPositionOld = padPosition;
          buttonOneStatus = 0; 
        }
         
        // check score state and Game Over ----------------------------------------------------- 
        if(score == 0) { // --------------- bomber Game Over !!! -------------------------------  
           gameOverSoundBomber(); 
           vgaPrint(str515, 40, 20, 1); 
           setupBomber(); 
           padPositionOld = padPosition + 1;   
           do
           {
              processInputs(); 
           } while (buttonOneStatus == 0 && buttonTwoStatus == 0 && buttonThreeStatus == 0);  
           vga.delay(100);
           vga.clear(0); 
           if (buttonThreeStatus == 1){
              state = 0; 
              vga.delay(200);
           }
        }
        if (score > 232) {
          score = 116;
          nextlivesSoundBomber(); 
          lives++; 
     }
     ballX = byte(speedX/256); 
     ballY = byte(speedY); 
     padPosition = byte((((128 - wheelOnePosition)) * (VGAX_WIDTH - 12))/ 128 + 6);
     if(state == 1) {
        vga.clear(0); 
     }
     else{
        drawGameScreenBomber(); 
     }
     bomberVariableB = byte(bombX); //bomb X - int 
     ballVy = ballPY/100.; //bomb Y - both float
     ballX0 = ballX; //plane X - byte 
     ballY0 = ballY; //plane Y - byte 
     padPositionOld = padPosition; // pad position 
     if (buttonThreeStatus == 1) { 
        state = 0; 
        vga.delay(200);
     } 
}
// ---------------------------------------- void bomber() end ---------------------------------------------
