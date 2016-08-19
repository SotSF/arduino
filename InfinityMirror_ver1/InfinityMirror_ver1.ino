#include <FastLED.h>

#define SND_SNS_PIN       12
#define INF_LED_PIN       3
#define BKG_LED_PIN       5
#define COLOR_ORDER       GRB
#define CHIPSET           WS2812B
#define NUM_LEDS          40
#define BRIGHTNESS        50
#define INIT_CYCLES_PER_FRAME    10000

int cycleCounter = 0;
int cols = 0;
int maxCols = 50;
int countPrint = 4000;
int soundVal = 1;
bool gReverseDirection = false;

int cyclesPerFrame;
int mode = 0;
bool sound = false;
bool soundLast = false;
long cyclesSinceSound = 0;

CRGB leds[NUM_LEDS];

void setup() {
  // sanity delay
  delay(2000);

  // map both LED strips to the leds array
  FastLED.addLeds<CHIPSET, INF_LED_PIN, COLOR_ORDER>(leds, NUM_LEDS).setCorrection(TypicalLEDStrip);
  FastLED.addLeds<CHIPSET, BKG_LED_PIN, COLOR_ORDER>(leds, NUM_LEDS).setCorrection(TypicalLEDStrip);
  FastLED.setBrightness(BRIGHTNESS);

  // set digital pin to input for sound sensor
  pinMode(SND_SNS_PIN, INPUT);
  
  Serial.begin(9600);

  // default number of cycles that constitute a frame are initialized here
  cyclesPerFrame = INIT_CYCLES_PER_FRAME;
}

void loop() {

  // increment the cycleCounter
  cycleCounter++;

  // if the cycleCounter has counted up to a new frame
  if (cycleCounter >= cyclesPerFrame) {
    
    // reset the cycleCounter
    cycleCounter = 0;

    switch(mode) {
      case 0:
        Fire();
        break;
      
      case 1:
        SolidColor(255,0,0);
        break;
        
      case 2:
        SolidColor(0,255,0);
        break;
        
      case 3:
        SolidColor(0,0,255);
        break;
    }
    
    FastLED.show();
  }

  // if a sound is ending, switch modes
  cyclesSinceSound++;
  sound = checkSound();
  if ( soundLast && !sound && (cyclesSinceSound > 10000) ) {
    cyclesSinceSound = 0;
    
    if (mode == 0) mode = 1;
    else if (mode == 1) mode = 2;
    else if (mode == 2) mode = 3;
    else if (mode == 3) mode = 0;
  }
  soundLast = sound;
    
}

//-----------------------------------------------------------
// Utility Functions
//-----------------------------------------------------------

bool checkSound() {
  if (digitalRead(SND_SNS_PIN) == 0)
    return true;
  else
    return false;
}

//-----------------------------------------------------------
// Mode Functions
//-----------------------------------------------------------

#define COOLING  55
#define SPARKING 120

void Fire()
{
// Array of temperature readings at each simulation cell
  static byte heat[NUM_LEDS];

  // Step 1.  Cool down every cell a little
    for( int i = 0; i < NUM_LEDS; i++) {
      heat[i] = qsub8( heat[i],  random8(0, ((COOLING * 10) / NUM_LEDS) + 2));
    }
  
    // Step 2.  Heat from each cell drifts 'up' and diffuses a little
    for( int k= NUM_LEDS - 1; k >= 2; k--) {
      heat[k] = (heat[k - 1] + heat[k - 2] + heat[k - 2] ) / 3;
    }
    
    // Step 3.  Randomly ignite new 'sparks' of heat near the bottom
    if( random8() < SPARKING ) {
      int y = random8(7);
      heat[y] = qadd8( heat[y], random8(160,255) );
    }

    // Step 4.  Map from heat cells to LED colors
    for( int j = 0; j < NUM_LEDS; j++) {
      CRGB color = HeatColor( heat[j]);
      int pixelnumber;
      if( gReverseDirection ) {
        pixelnumber = (NUM_LEDS-1) - j;
      } else {
        pixelnumber = j;
      }
      leds[pixelnumber] = color;
    }
}

void SolidColor(int r, int g, int b)
{
  for( int j = 0; j < NUM_LEDS; j++) {
    leds[j].setRGB(r,g,b);
  }
}

