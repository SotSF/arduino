#include <FastLED.h>

FASTLED_USING_NAMESPACE

#define SND_SNS_PIN       12
#define INF_LED_PIN       3
#define BRD_LED_PIN       5
#define COLOR_ORDER       GRB
#define CHIPSET           WS2812B
#define NUM_LEDS          219
#define NUM_LEDS_BORDER   219
#define BRIGHTNESS        60
#define INIT_FRAMES_PER_SECOND   30
#define SOUND_THRESHOLD_MS       100
#define PATTERN_SWITCH_THRESHOLD_MS 300000 // change every 5 minutes
int soundVal = 1;
bool gReverseDirection = false;

bool sound = false;
bool soundLast = false;
bool newPatt = true;
unsigned long timeOfLastFrame;
unsigned long timeOfLastSound;
int framesPS;
uint8_t motion;

CRGB leds[NUM_LEDS];
//CRGB leds_border[NUM_LEDS_BORDER];

//ColorPalette--------------------------------------------------
CRGBPalette16 currentPalette;
TBlendType    currentBlending;

extern CRGBPalette16 myRedWhiteBluePalette;
extern const TProgmemPalette16 myRedWhiteBluePalette_p PROGMEM;
//ColorPalette--------------------------------------------------

//Fire----------------------------------------------------------
void fire()
{
  framesPS = 30;
  
  // Array of temperature readings at each simulation cell
  static byte heat[NUM_LEDS];

  // Step 1.  Cool down every cell a little
    for( int i = 0; i < NUM_LEDS; i++) {
      heat[i] = qsub8( heat[i],  random8(0, ((55 * 10) / NUM_LEDS) + 2));
    }
  
    // Step 2.  Heat from each cell drifts 'up' and diffuses a little
    for( int k= NUM_LEDS - 1; k >= 2; k--) {
      heat[k] = (heat[k - 1] + heat[k - 2] + heat[k - 2] ) / 3;
    }
    
    // Step 3.  Randomly ignite new 'sparks' of heat near the bottom
    if( random8() < 120 ) {
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
//Fire----------------------------------------------------------

//PatternRotation-----------------------------------------------
// List of patterns to cycle through.  Each is defined as a separate function below.
typedef void (*SimplePatternList[])();
SimplePatternList patternList = { colorPaletteRainbowVortex, colorPaletteRainbowStripes, colorPaletteTotallyRandom, colorPaletteCloudColors, colorPalettePartyColors
, rainbowWithGlitter, confetti, sinelon, juggle, bpm };
uint8_t pattNum = 0;    // Index number of which pattern is current
uint8_t gHue = 0;       // rotating "base color" used by many of the patterns
//PatternRotation-----------------------------------------------

void setup() {
  // sanity delay
  delay(2000);

  // map both LED strips to the leds array
  FastLED.addLeds<CHIPSET, INF_LED_PIN, COLOR_ORDER>(leds, 14, NUM_LEDS).setCorrection(TypicalLEDStrip);
  FastLED.addLeds<CHIPSET, BRD_LED_PIN, COLOR_ORDER>(leds, NUM_LEDS).setCorrection(TypicalLEDStrip);
  FastLED.setBrightness(BRIGHTNESS);

  // set digital pin to input for sound sensor
  pinMode(SND_SNS_PIN, INPUT);
  
  Serial.begin(9600);

  timeOfLastFrame = millis();
  timeOfLastSound = millis();
  framesPS = INIT_FRAMES_PER_SECOND;
  
  currentPalette = RainbowColors_p;
  currentBlending = LINEARBLEND;
}

void loop() {

  // perform a periodic check for the sound signal
  sound = false;
  if ( !sound && checkSound() ) sound = true;
  
  if ( (millis() - timeOfLastFrame) >= (1000 / framesPS) ) {

    timeOfLastFrame = millis();    
    patternList[pattNum]();

    // perform a periodic check for the sound signal
    if ( !sound && checkSound() ) sound = true;
    
    FastLED.show();
    
  }

  // perform a periodic check for the sound signal
  if ( !sound && checkSound() ) sound = true;

  // if a sound is starting, or if a sound hasn't played for a certain amount of time, switch patterns
  if ( ( !soundLast && sound && ( (millis() - timeOfLastSound) > SOUND_THRESHOLD_MS) ) || ((millis() - timeOfLastSound) >= PATTERN_SWITCH_THRESHOLD_MS) ) {
    
    timeOfLastSound = millis();
    nextPattern();
    
  }
  soundLast = sound;
  
}

//-----------------------------------------------------------
// Utility Functions
//-----------------------------------------------------------

#define ARRAY_SIZE(A) (sizeof(A) / sizeof((A)[0]))

void nextPattern()
{
  // add one to the current pattern number, and wrap around at the end
  pattNum = (pattNum + 1) % ARRAY_SIZE( patternList);
  newPatt = true;
}

bool checkSound() {
  if (digitalRead(SND_SNS_PIN) == 0)
    return true;
  else
    return false;
}

void fadeAll() {
  for(int i = 0; i < NUM_LEDS; i++) {
    leds[i].nscale8(250);
  }
}

void SolidColor(int r, int g, int b)
{
  for( int j = 0; j < NUM_LEDS; j++) {
    leds[j].setRGB(r,g,b);
  }
}

void randomMotion()
{
  motion = random8(1,4);
}

//-----------------------------------------------------------
// Pattern Functions
//-----------------------------------------------------------

/* for fire() function, see above */

//-----------------------------------------------------------

void colorPaletteRainbowVortex() {
  framesPS = 40;
  static uint8_t startIndex = 0;

  if (newPatt) {
    newPatt = false;
    startIndex = 0;
    currentPalette = RainbowColors_p;         
    currentBlending = LINEARBLEND; 
  }
  
  startIndex = startIndex + 1; /* motion speed */
  FillLEDsFromPaletteColors(startIndex);
}

void colorPaletteRainbowStripes() {
  framesPS = 30;
  static uint8_t startIndex = 0;
  
  if (newPatt) {
    newPatt = false;
    startIndex = 0;
    currentPalette = RainbowStripeColors_p;         
    currentBlending = LINEARBLEND; 
  }
  
  startIndex = startIndex + 2; /* motion speed */
  FillLEDsFromPaletteColors(startIndex);
}

void colorPalettePurpleAndGreen() {
  framesPS = 30;
  static uint8_t startIndex = 0;

  if (newPatt) {
    newPatt = false;
    startIndex = 0;
    SetupPurpleAndGreenPalette();
    currentBlending = LINEARBLEND;
  }
  
  startIndex = startIndex + 1; /* motion speed */
  FillLEDsFromPaletteColors(startIndex);
}

void colorPaletteTotallyRandom() {
  framesPS = 30;
  static uint8_t startIndex = 0;

  if (newPatt) {
    newPatt = false;
    startIndex = 0;
    SetupTotallyRandomPalette();
    randomMotion();
    currentBlending = LINEARBLEND;
  }
  
  startIndex = startIndex + motion; /* motion speed */
  FillLEDsFromPaletteColors(startIndex);
}

void colorPaletteBlackAndWhiteStriped1() {
  framesPS = 30;
  static uint8_t startIndex = 0;

  if (newPatt) {
    newPatt = false;
    startIndex = 0;
    SetupBlackAndWhiteStripedPalette();
    currentBlending = NOBLEND;
  }
  
  startIndex = startIndex + 1; /* motion speed */
  FillLEDsFromPaletteColors(startIndex);
}

void colorPaletteBlackAndWhiteStriped2() {
  framesPS = 30;
  static uint8_t startIndex = 0;

  if (newPatt) {
    newPatt = false;
    startIndex = 0;
    SetupBlackAndWhiteStripedPalette();
    currentBlending = LINEARBLEND;
  }
  
  startIndex = startIndex + 1; /* motion speed */
  FillLEDsFromPaletteColors(startIndex);
}

void colorPaletteCloudColors() {
  framesPS = 30;
  static uint8_t startIndex = 0;

  if (newPatt) {
    newPatt = false;
    startIndex = 0;
    currentPalette = CloudColors_p;         
    currentBlending = LINEARBLEND;
  }
  
  startIndex = startIndex + 1; /* motion speed */
  FillLEDsFromPaletteColors(startIndex);
}

void colorPalettePartyColors() {
  framesPS = 30;
  static uint8_t startIndex = 0;

  if (newPatt) {
    newPatt = false;
    startIndex = 0;
    currentPalette = PartyColors_p;         
    currentBlending = LINEARBLEND;
  }
  
  startIndex = startIndex + 1; /* motion speed */
  FillLEDsFromPaletteColors(startIndex);
}

/*
void colorPalette() {
  framesPS = 30;
  ChangePalettePeriodically();
  
  static uint8_t startIndex = 0;
  startIndex = startIndex + 1;
  
  FillLEDsFromPaletteColors(startIndex);
}
*/

void FillLEDsFromPaletteColors( uint8_t colorIndex)
{
    uint8_t brightness = 255;
    
    for( int i = 0; i < NUM_LEDS; i++) {
        leds[i] = ColorFromPalette( currentPalette, colorIndex, brightness, currentBlending);
        colorIndex += 3;
    }
}

// This function fills the palette with totally random colors.
void SetupTotallyRandomPalette()
{
    for( int i = 0; i < 16; i++) {
        currentPalette[i] = CHSV( random8(), 255, random8());
    }
}

// This function sets up a palette of black and white stripes,
// using code.  Since the palette is effectively an array of
// sixteen CRGB colors, the various fill_* functions can be used
// to set them up.
void SetupBlackAndWhiteStripedPalette()
{
    // 'black out' all 16 palette entries...
    fill_solid( currentPalette, 16, CRGB::Black);
    // and set every fourth one to white.
    currentPalette[0] = CRGB::White;
    currentPalette[4] = CRGB::White;
    currentPalette[8] = CRGB::White;
    currentPalette[12] = CRGB::White;
    
}

// This function sets up a palette of purple and green stripes.
void SetupPurpleAndGreenPalette()
{
    CRGB purple = CHSV( HUE_PURPLE, 255, 255);
    CRGB yellow  = CHSV( HUE_YELLOW, 255, 255);
    CRGB black  = CRGB::Black;
    
    currentPalette = CRGBPalette16(
                                   yellow,  black,  black,  black,
                                   purple, purple, black,  black,
                                   yellow,  black,  black,  black,
                                   purple, purple, black,  black );
}

//-----------------------------------------------------------

void cylon()
{
  framesPS = 30;
  static uint8_t hue = 0;
  // First slide the led in one direction
  for(int i = 0; i < NUM_LEDS; i++) {
    // Set the i'th led to red 
    leds[i] = CHSV(hue++, 255, 255);
    // Show the leds
    FastLED.show(); 
    // now that we've shown the leds, reset the i'th led to black
    // leds[i] = CRGB::Black;
    fadeAll();
    // Wait a little bit before we loop around and do it again
    delay(10);
  }

  // Now go in the other direction.  
  for(int i = (NUM_LEDS)-1; i >= 0; i--) {
    // Set the i'th led to red 
    leds[i] = CHSV(hue++, 255, 255);
    // Show the leds
    FastLED.show();
    // now that we've shown the leds, reset the i'th led to black
    // leds[i] = CRGB::Black;
    fadeAll();
    // Wait a little bit before we loop around and do it again
    delay(10);
  }
}

//-----------------------------------------------------------

void rainbow() 
{
  // FastLED's built-in rainbow generator
  fill_rainbow( leds, NUM_LEDS, gHue, 7);
}

//-----------------------------------------------------------

void rainbowWithGlitter() 
{
  // built-in FastLED rainbow, plus some random sparkly glitter
  rainbow();
  addGlitter(80);
}

void addGlitter( fract8 chanceOfGlitter) 
{
  if( random8() < chanceOfGlitter) {
    leds[ random16(NUM_LEDS) ] += CRGB::White;
  }
}

//-----------------------------------------------------------

void confetti() 
{
  framesPS = 40;
  
  // random colored speckles that blink in and fade smoothly
  fadeToBlackBy( leds, NUM_LEDS, 10);
  int pos = random16(NUM_LEDS);
  leds[pos] += CHSV( gHue + random8(64), 200, 255);
}

//-----------------------------------------------------------

void sinelon()
{
  // a colored dot sweeping back and forth, with fading trails
  fadeToBlackBy( leds, NUM_LEDS, 20);
  int pos = beatsin16(15,0,NUM_LEDS);
  leds[pos] += CHSV( gHue, 255, 192);
}

//-----------------------------------------------------------

void bpm()
{
  // colored stripes pulsing at a defined Beats-Per-Minute (BPM)
  uint8_t BeatsPerMinute = 62;
  CRGBPalette16 palette = PartyColors_p;
  uint8_t beat = beatsin8( BeatsPerMinute, 64, 255);
  for( int i = 0; i < NUM_LEDS; i++) { //9948
    leds[i] = ColorFromPalette(palette, gHue+(i*2), beat-gHue+(i*10));
  }
}

//-----------------------------------------------------------

void juggle() {
  // eight colored dots, weaving in and out of sync with each other
  fadeToBlackBy( leds, NUM_LEDS, 20);
  byte dothue = 0;
  for( int i = 0; i < 8; i++) {
    leds[beatsin16(i+7,0,NUM_LEDS)] |= CHSV(dothue, 200, 255);
    dothue += 32;
  }
}



