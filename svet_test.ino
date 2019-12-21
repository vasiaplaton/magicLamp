#include <FastLED.h>
#include "GyverButton.h"
#include "GyverTimer.h"
// settings
#define TRACK_STEP 1 // less - more steps in animation
#define FIRE_PALETTE 0  // types of fire
#define AUTOPLAY_TIME 30 // time of change mode
#define AUTOBRIGHT_TIME 300 // time of autobright
#define COEF 0.6 // coef for autobright 0-1000(0,001-1,000);
#define NUM_LEDS 12 // number of leds
#define NUM_LEDS1 6 // 1/2 number of leds for fire
#define DATA_PIN 9 // pin of ws2812b
#define UP_PIN 3 // pin of up button
#define DOWN_PIN 2   // pin of down button   
#define BRIGHTNESS 250 // std brightness in start
#define MIN_BRIGHTNESS 2 // min brightness for hand setting
#define MODES_AMOUNT 8 // number of modes
#define STD_SPEED 6 // speed of animation bigger - slowly
// end
// leds routins
CRGB leds[NUM_LEDS];
CRGBPalette16 gPal;
// buttons
GButton UP(UP_PIN, LOW_PULL, NORM_OPEN);
GButton DOWN(DOWN_PIN, LOW_PULL, NORM_OPEN);
// timer
GTimer_ms effectTimer(10); //60
GTimer_ms autoplayTimer((long)AUTOPLAY_TIME * 1000);
GTimer_ms autobrightTimer(AUTOBRIGHT_TIME);
// variables
boolean loadingFlag = true;
boolean UP_CL; 
float temp_br;
boolean DOWN_CL;
bool gReverseDirection = false;
boolean power=1;
boolean autoplay=0;
boolean autobright=0;
int sped=STD_SPEED;
int brightness=BRIGHTNESS;
int mode;
int a=0;
int prevmode;
void setup() { 
      FastLED.addLeds<NEOPIXEL, DATA_PIN>(leds, NUM_LEDS);
  UP.setTimeout(300);
  UP.setStepTimeout(10);
  DOWN.setTimeout(300);
  DOWN.setStepTimeout(10);
  
  randomSeed(analogRead(0));
  
  if (FIRE_PALETTE == 0) gPal = HeatColors_p;
  else if (FIRE_PALETTE == 1) gPal = CRGBPalette16( CRGB::Black, CRGB::Red, CRGB::Yellow, CRGB::White);
  else if (FIRE_PALETTE == 2) gPal = CRGBPalette16( CRGB::Black, CRGB::Blue, CRGB::Aqua,  CRGB::White);
  else if (FIRE_PALETTE == 3) gPal = CRGBPalette16( CRGB::Black, CRGB::Red, CRGB::White);

}


// залить все
void fillAll(CRGB newcolor) {
  for (int i = 0; i < NUM_LEDS; i++) {
    leds[i] = newcolor;
  }
}

// функция получения цвета пикселя по его номеру
uint32_t getPixColor(int thisPixel) {
  return (((uint32_t)leds[thisPixel].r << 16) | ((long)leds[thisPixel].g << 8 ) | (long)leds[thisPixel].b);
}

void Next_mode(){
   if(mode<MODES_AMOUNT) mode++;
   loadingFlag = true;
   FastLED.clear();
  /// leds[1] = CRGB::Red;
}
void Prev_mode(){
  if(mode>0) mode--;
  loadingFlag = true;
  FastLED.clear();
 /// leds[1] = CRGB::Blue;
}
void ButtonTick(){
   UP.tick();
      DOWN.tick();
      UP_CL=UP.isStep();
      DOWN_CL=DOWN.isStep();
      if( UP_CL && DOWN_CL){
        power=0;
        //delay(1000);
      }
      
      if(UP.hasClicks() && !DOWN_CL  ){
        byte clicks = UP.getClicks();
       switch(clicks){
          case 1:
          if(!power) power=1;
          else Next_mode();
          break;
          case 2:
          if(sped>1) sped--;
          break;
          case 3:
          // autobright on
          autobright_on_anim(5, 50);
          autobright=1;
          break;
          case 4:
          autoplay_on_anim(50);
          autoplay=1;
                    // autoplay on
          break;
        }
      }
      if(UP_CL  && !DOWN_CL ){
        if(brightness+1 < 255) brightness += 1;
      }
      if(DOWN.hasClicks() && !UP_CL ){
        byte clicks = DOWN.getClicks();
       switch(clicks){
          case 1:
          if(!power) power=1;
          else Prev_mode();
          break;
          case 2:
          if(sped<8) sped++;
          break;
          case 3:
          //autobright off
          autobright_off_anim(5 ,50);
          autobright=0;
          break;
          case 4:
          autoplay_off_anim(50);
          autoplay=0;
   // autoplay off       
          break;
        }
      }
      if(DOWN_CL && !UP_CL ){
        if(brightness-1>MIN_BRIGHTNESS) brightness -= 1;
      }
 

  if(power && !autobright)    FastLED.setBrightness(brightness);
   if(!power && !autobright) FastLED.setBrightness(0);
  FastLED.show();
}
void ModeTick(){
  if (effectTimer.isReady() && power) {
      a++;
  if(mode<6 && ((a%sped)==0)){
    switch (mode) {
      case 0: lighter();
        break;
      case 1: lightBugs();
        break;
      case 2: colors();
        break;
      case 3: rainbow();
        break;
      case 4: sparkles();
        break;
      case 5: fire();
        break;
    }
 }
  if( mode >= 6 && (a%sped)==0){
    switch (mode) {
      case 6:
      for(int i=0; i<NUM_LEDS; i++) leds[i]=CRGB::White;
      break;
      case 7:
      phototocirc();
      break;
    }
  }
  FastLED.show();
  }
//  FastLED.show();
}
void autoPlayTick(){
    if (autoplayTimer.isReady() && autoplay) {// таймер смены режима
 mode++;
 if ( mode > MODES_AMOUNT) mode =0;
  }
}
void autoBrightTick(){ // dont work, need test
  if(autobrightTimer.isReady() && autobright){
    // work with photoresist
    int val;
   val=map(analogRead(A6), 100, 1024, 2, 250);
   temp_br=val*COEF;
   if(temp_br>MIN_BRIGHTNESS) FastLED.setBrightness(temp_br);
   else FastLED.setBrightness(MIN_BRIGHTNESS);
  }
}
void loop() {
  ButtonTick();
  ModeTick(); 
autoPlayTick();
autoBrightTick();
  }
