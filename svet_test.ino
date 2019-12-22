/*
Code for the project magiclamp.
Scheme, board https://github.com/vasiaplaton/magiclamp  (private repo)
Author: Vasiliy Platon - website vasblog.tk, youtube VasChanell.
*/
/*
Control 
1x UP - next mode
1x DOWN - previous mode
Long press UP - brightness UP
Long press DOWN - brightness DOWN
LOng press UP & DOWN - off system(for switch on system use 1x UP or DOWN)
2x UP - speed up
2x DOWN - speed down
3x UP - autobright on
3x DOWN - autobright off
4x UP - autoplay on
4x DOWN - autoplay off
*/
#include <FastLED.h>
#include "GyverButton.h"
#include "GyverTimer.h"
#include <EEPROM.h>
// settings
#define TRACK_STEP 1 // less - more steps in animation
#define FIRE_PALETTE 0  // types of fire
#define AUTOPLAY_TIME 30 // time of change mode
#define AUTOBRIGHT_TIME 300 // time of autobright
#define COEF 0.6 // coef for autobright 0-1000(0,001-1,000);
#define NUM_LEDS 12 // number of leds
#define EEPROM_TIME 1000 // in ms
#define EEPROM_STD_ST 1 // eeprom on/off
#define NUM_LEDS1 6 // 1/2 number of leds for fire
#define DATA_PIN 9 // pin of ws2812b
#define UP_PIN 3 // pin of up button
#define DOWN_PIN 2   // pin of down button   
#define BRIGHTNESS 250 // std brightness in start
#define MIN_BRIGHTNESS 2 // min brightness for hand setting
#define MODES_AMOUNT 7 // number of modes
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
GTimer_ms eepromTimer(EEPROM_TIME);
// variables
boolean loadingFlag = true; // need or not setup for effect
boolean UP_CL; // long click UP
float temp_br; // temp brightness for autobright
boolean DOWN_CL; // long click DOWN
bool gReverseDirection = false;
boolean power=1; // power in start
boolean autoplay=0; // autoplay start state
boolean autobright=0; // autobright start state 
boolean eeprom=EEPROM_STD_ST;
int sped=STD_SPEED;
int brightness=BRIGHTNESS;
int mode=0; // mode start
int a=0;
void LoadDataFromEeprom(){ // reed data from eeprom in setup 
  if(eeprom){
    autoplay=EEPROM.read(0);
    autobright=EEPROM.read(1);
    if(!autoplay) mode=EEPROM.read(2); // if autoplay on, don't need mode
    if(!autobright) brightness=EEPROM.read(3); // if autobrightness on, don't need brightness
    sped=EEPROM.read(4); // read speed
  }
}
void setup() { 
      FastLED.addLeds<NEOPIXEL, DATA_PIN>(leds, NUM_LEDS); // leds start
      //button settings
  UP.setTimeout(300);
  UP.setStepTimeout(10);
  DOWN.setTimeout(300);
  DOWN.setStepTimeout(10);
  // seed for random
  randomSeed(analogRead(0));
  //fire palette
  if (FIRE_PALETTE == 0) gPal = HeatColors_p;
  else if (FIRE_PALETTE == 1) gPal = CRGBPalette16( CRGB::Black, CRGB::Red, CRGB::Yellow, CRGB::White);
  else if (FIRE_PALETTE == 2) gPal = CRGBPalette16( CRGB::Black, CRGB::Blue, CRGB::Aqua,  CRGB::White);
  else if (FIRE_PALETTE == 3) gPal = CRGBPalette16( CRGB::Black, CRGB::Red, CRGB::White);
  // load data from eeprom 
 LoadDataFromEeprom();

}


// fill all leds 
void fillAll(CRGB newcolor) {
  for (int i = 0; i < NUM_LEDS; i++) {
    leds[i] = newcolor;
  }
}

// get pix color know his nubmer
uint32_t getPixColor(int thisPixel) {
  return (((uint32_t)leds[thisPixel].r << 16) | ((long)leds[thisPixel].g << 8 ) | (long)leds[thisPixel].b);
}
// next mode func
void Next_mode(){
   if(mode<MODES_AMOUNT) mode++;
   loadingFlag = true;
   FastLED.clear();
  /// leds[1] = CRGB::Red;
}
//previous mode func
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
      if( UP_CL && DOWN_CL){ // long press DOWN & UP
        power=0; // power off
      }
      
      if(UP.hasClicks() && !DOWN_CL  ){ // UP has clicked
        byte clicks = UP.getClicks();
       switch(clicks){
          case 1: // one click
          if(!power) power=1;
          else Next_mode();
          break;
          case 2: // two click
          if(sped>1) sped--;
          break;
          case 3: // three click
          // autobright on
          autobright_on_anim(5, 50);
          autobright=1;
          break;
          case 4:
          // autoplay on
          autoplay_on_anim(50);
          autoplay=1; 
          break;
        }
      }
      if(UP_CL  && !DOWN_CL ){ // long click 
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
          // autoplay off 
          autoplay_off_anim(50);
          autoplay=0;      
          break;
        }
      }
      if(DOWN_CL && !UP_CL ){ // long click 
        if(brightness-1>MIN_BRIGHTNESS) brightness -= 1;
      }
 

  if(power && !autobright)    FastLED.setBrightness(brightness); // power on, set normal brightness
   if(!power) FastLED.setBrightness(0); // power off, off leds
  FastLED.show();
}
void ModeTick(){ // draw mode
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
     // phototocirc();
      break;
    }
  }
  FastLED.show();
  }
//  FastLED.show();
}
void autoPlayTick(){ // autoplay tick
    if (autoplayTimer.isReady() && autoplay) {// таймер смены режима
 mode++; 
 if ( mode > MODES_AMOUNT) mode =0;
  }
}
void autoBrightTick(){  // autobright tick
  if(autobrightTimer.isReady() && autobright){
    // work with photoresist
    int val;
   val=map(analogRead(A6), 100, 1024, 2, 250);
   temp_br=val*COEF;
   if(temp_br>MIN_BRIGHTNESS) FastLED.setBrightness(temp_br);
   else FastLED.setBrightness(MIN_BRIGHTNESS);
  }
}
void eepromTick(){ // save to eeprom settings everu 1s
  if (eepromTimer.isReady() && eeprom){
    
    // need to save mode, autoplay, autobright, brightness if !autobright, sped;
    EEPROM.update(0, autoplay);
    EEPROM.update(1, autobright);
    if(!autoplay) EEPROM.update(2, mode);
    if(!autobright) EEPROM.update(3, brightness);
    EEPROM.update(4, sped);
  }
}
void loop() {
  //timer ticks
  ButtonTick();
  ModeTick(); 
autoPlayTick();
autoBrightTick();
eepromTick();
  }
