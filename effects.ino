int aa=0;
// ****************************** ОГОНЁК ******************************
int16_t position;
boolean direction;

void lighter() {
  FastLED.clear();
  if (direction) {
    position++;
    if (position > NUM_LEDS - 2) {
      direction = false;
    }
  } else {
    position--;
    if (position < 1) {
      direction = true;
    }
  }
  leds[position] = CRGB::White;
}

// ****************************** СВЕТЛЯЧКИ ******************************
#define MAX_SPEED 30
#define BUGS_AMOUNT 3
int16_t speed[BUGS_AMOUNT];
int16_t pos[BUGS_AMOUNT];
CRGB bugColors[BUGS_AMOUNT];

void lightBugs() {
  if (loadingFlag) {
    loadingFlag = false;
    for (int i = 0; i < BUGS_AMOUNT; i++) {
      bugColors[i] = CHSV(random(0, 9) * 28, 255, 255);
      pos[i] = random(0, NUM_LEDS);
      speed[i] += random(-5, 6);
    }
  }
  FastLED.clear();
  for (int i = 0; i < BUGS_AMOUNT; i++) {
    speed[i] += random(-5, 6);
    if (speed[i] == 0) speed[i] += (-5, 6);

    if (abs(speed[i]) > MAX_SPEED) speed[i] = 0;
    pos[i] += speed[i] / 10;
    if (pos[i] < 0) {
      pos[i] = 0;
      speed[i] = -speed[i];
    }
    if (pos[i] > NUM_LEDS - 1) {
      pos[i] = NUM_LEDS - 1;
      speed[i] = -speed[i];
    }
    leds[pos[i]] = bugColors[i];
  }
}

// ****************************** ЦВЕТА ******************************
byte hue;
void colors() {
  hue += 2;
  CRGB thisColor = CHSV(hue, 255, 255);
  fillAll(CHSV(hue, 255, 255));
}

// ****************************** РАДУГА ******************************
void rainbow() {
  hue += 2;
  for (int i = 0; i < NUM_LEDS; i++)
    leds[i] = CHSV((byte)(hue + i * float(255 / NUM_LEDS)), 255, 255);
}

// ****************************** КОНФЕТТИ ******************************
void sparkles() {
  byte thisNum = random(0, NUM_LEDS);
  if (getPixColor(thisNum) == 0)
    leds[thisNum] = CHSV(random(0, 255), 255, 255);
  fade();
}

// ****************************** ОГОНЬ ******************************
#define COOLING  55
// SPARKING: What chance (out of 255) is there that a new spark will be lit?
// Higher chance = more roaring fire.  Lower chance = more flickery fire.
// Default 120, suggested range 50-200.
#define SPARKING 120

void fire() {
  random16_add_entropy( random());
  Fire2012WithPalette(); // run simulation frame, using palette colors
}

void Fire2012WithPalette()
{
  // Array of temperature readings at each simulation cell
  static byte heat[NUM_LEDS1];

  // Step 1.  Cool down every cell a little
  for ( int i = 0; i < NUM_LEDS1; i++) {
    heat[i] = qsub8( heat[i],  random8(0, ((COOLING * 10) / NUM_LEDS) + 2));
  }

  // Step 2.  Heat from each cell drifts 'up' and diffuses a little
  for ( int k = NUM_LEDS1 - 1; k >= 2; k--) {
    heat[k] = (heat[k - 1] + heat[k - 2] + heat[k - 2] ) / 3;
  }

  // Step 3.  Randomly ignite new 'sparks' of heat near the bottom
  if ( random8() < SPARKING ) {
    int y = random8(7);
    heat[y] = qadd8( heat[y], random8(160, 255) );
  }

  // Step 4.  Map from heat cells to LED colors
  for ( int j = 0; j < NUM_LEDS1; j++) {
    // Scale the heat value from 0-255 down to 0-240
    // for best results with color palettes.
    byte colorindex = scale8( heat[j], 240);
    CRGB color = ColorFromPalette( gPal, colorindex);
    int pixelnumber;
    if ( gReverseDirection ) {
    pixelnumber = (NUM_LEDS - 1) - j;
    } else {
      pixelnumber = j;
  }
    leds[pixelnumber] = color;
    leds[(NUM_LEDS-1)-pixelnumber] = color;
  }
}

// *************** ВИНИГРЕТ ***************
/*void vinigret() {
  for (int i = 0; i < NUM_LEDS; i++) {
    if ((uint32_t)getPixColor(i) == 0) {
      
    }
  }
}
*/
// ****************** СЛУЖЕБНЫЕ ФУНКЦИИ *******************
void fade() {
  for (int i = 0; i < NUM_LEDS; i++) {
    if ((uint32_t)getPixColor(i) == 0) continue;
    leds[i].fadeToBlackBy(TRACK_STEP);
  }
}
void autoplay_off_anim(int delayy){
           loadingFlag = true; // fixed bug(after animation mode don't worked correct)
            for(int i=0; i<NUM_LEDS; i++){
            leds[i]=CRGB::Red;
          }
          FastLED.show();
          for(int i=NUM_LEDS-1; i>=0; i--){
            leds[i]=CRGB::Black;
           delay(delayy);
          FastLED.show();
          }
}
void autoplay_on_anim(int delayy){
       loadingFlag = true; // fixed bug(after animation mode don't worked correct)
            for(int i=0; i<NUM_LEDS; i++){
            leds[i]=CRGB::Black;
          }
          FastLED.show();
          for(int i=0; i<NUM_LEDS; i++){
            leds[i]=CRGB::Red;
            delay(delayy);
          FastLED.show();
          }
}
void autobright_on_anim(int delayy, int maxx){
  loadingFlag = true; // fixed bug(after animation mode don't worked correct)
              for(int i=0; i<NUM_LEDS; i++){
            leds[i]=CRGB::Red;
          }
          for(int i=0; i<maxx; i++){
            FastLED.setBrightness(i);
            delay(delayy);
          FastLED.show();
          }
          
}
void autobright_off_anim(int delayy, int maxx){
  loadingFlag = true; // fixed bug(after animation mode don't worked correct)
              for(int i=0; i<NUM_LEDS; i++){
            leds[i]=CRGB::Red;
          }
          for(int i=maxx; i>0; i--){
            FastLED.setBrightness(i);
            delay(delayy);
          FastLED.show();
          }
          
}
#define HUE_START 3     // начальный цвет огня (0 красный, 80 зелёный, 140 молния, 190 розовый)
#define HUE_GAP 18      // коэффициент цвета огня (чем больше - тем дальше заброс по цвету)
#define SMOOTH_K 0.15   // коэффициент плавности огня
#define MIN_BRIGHT 80   // мин. яркость огня
#define MAX_BRIGHT 255  // макс. яркость огня
#define MIN_SAT 245     // мин. насыщенность
#define MAX_SAT 255     // макс. насыщенность

// для разработчиков
#define ZONE_AMOUNT NUM_LEDS   // количество зон
byte zoneValues[ZONE_AMOUNT];
byte zoneRndValues[ZONE_AMOUNT];

#define ORDER_GRB       // порядок цветов ORDER_GRB / ORDER_RGB / ORDER_BRG
#define COLOR_DEBTH 3   // цветовая глубина: 1, 2, 3 (в байтах)

void fire1(){
  int val;
  aa++;
  if((aa%5)==0){
    for(int i=0; i<NUM_LEDS; i++){
      zoneRndValues[i] = random(0, 10);
    }
  }
  int thisPos = 0, lastPos = 0;
 for(int i=0; i<NUM_LEDS; i++){
      zoneValues[i] = (float)zoneValues[i] * (1 - SMOOTH_K) + (float)zoneRndValues[i] * 10 * SMOOTH_K;
      val=zoneValues[i];
      CHSV color = CHSV(HUE_START + map(val, 20, 60, 0, HUE_GAP), constrain(map(val, 20, 60, MAX_SAT, MIN_SAT), 0, 255), constrain(map(val, 20, 60, MIN_BRIGHT, MAX_BRIGHT), 0, 255));
      leds[i]=color;
    }  
 FastLED.show();
}

/*void phototocirc(){
      int val;
   val=map(analogRead(A6), 360, 900, 0, NUM_LEDS-1);
   FastLED.clear();
   for(int i=0; i<val; i++){
    leds[i]=CRGB::Red;
   }
   FastLED.show();
  }*/
