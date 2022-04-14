/**
 Chitty Car Led Controllers
  includes Pacifica code for wave effect:
  //  "Pacifica"
  //  Gentle, blue-green ocean waves.
  //  December 2019, Mark Kriegsman and Mary Corey March.
  //  For Dan.
 */

#define FASTLED_ALLOW_INTERRUPTS 0
#include <FastLED.h>
FASTLED_USING_NAMESPACE

// for Pacifica
#define MAX_POWER_MILLIAMPS 700
#define LED_TYPE            WS2812B
#define COLOR_ORDER         GRB

// How many leds in your strip?
#define NUM_HEADLIGHT_LEDS 32
#define NUM_EFFECT_LEDS 150

// For led chips like WS2812, which have a data line, ground, and power, you just
// need to define DATA_PIN.  For led chipsets that are SPI based (four wires - data, clock,
// ground, and power), like the LPD8806 define both DATA_PIN and CLOCK_PIN
// Clock pin only needed for SPI based chipsets when not using hardware SPI
#define HEADLIGHT_DATA_PIN 2
#define EFFECTS_DATA_PIN 3
#define BUTTON_A 4
#define BUTTON_B 5
#define BUTTON_C 6
#define BUTTON_D 7
#define CLOCK_PIN 13
#define BRIGHTNESS_PIN A0

int buttons[4] = {BUTTON_A, BUTTON_B, BUTTON_C, BUTTON_D};

// Define the array of leds
CRGB headlightLeds[NUM_HEADLIGHT_LEDS];
CRGB effectLeds[NUM_EFFECT_LEDS];

int lastBrightness = 100;
int lastValue = 0;

void setup() { 
  
    Serial.begin(9600);
    while (!Serial);
    Serial.println("Starting up...");
    delay(250);
    
    FastLED.addLeds<LED_TYPE, HEADLIGHT_DATA_PIN, COLOR_ORDER>(headlightLeds, NUM_HEADLIGHT_LEDS);//.setCorrection(Candle);  // GRB ordering is typical
    FastLED.addLeds<LED_TYPE,EFFECTS_DATA_PIN,COLOR_ORDER>(effectLeds, NUM_EFFECT_LEDS);//.setCorrection(Candle); //.setCorrection( TypicalLEDStrip );
    FastLED.setMaxPowerInVoltsAndMilliamps( 5, MAX_POWER_MILLIAMPS);

    lastValue = analogRead(BRIGHTNESS_PIN);
    lastBrightness = map(lastValue, 0, 1023, 0, 255);
    FastLED.setBrightness(lastBrightness);
    
    for (int i = 0; i < 4; i++) {
      Serial.print(i);
      Serial.print(") = ");
      Serial.println(buttons[i]);
      pinMode(buttons[i], INPUT);
    }
    turnHeadlightsOff();
    delay(250);
    circleHeadlights();
    delay(250);
    turnHeadlightsOff();
    delay(250);
    flashHeadlights(4);
    delay(250);
    turnHeadlightsOff();

}

void clearEffectsLeds() {
  Serial.println("Turn effects off");
  FastLED.clear();
  fill_solid( effectLeds, NUM_EFFECT_LEDS, CRGB::Black);
  FastLED.show();
}

void turnHeadlightsOff() {
  Serial.println("Turn headlights off");
  FastLED.clear();
  fill_solid( headlightLeds, NUM_HEADLIGHT_LEDS, CRGB::Black);
  FastLED.show();
}

void turnHeadlightsOn() {
  Serial.println("Turn headlights on");
  fill_solid( headlightLeds, NUM_HEADLIGHT_LEDS, CRGB::White);
  FastLED.show();
}

void flashHeadlights(int times) {
  Serial.print("Flash headlights ");
  Serial.print(times);
  Serial.println(" times");
  turnHeadlightsOff();
  for (int i = 0; i < times; i++) {
    turnHeadlightsOn();
    delay(250);
    turnHeadlightsOff(); 
    delay(250);
  }
}

void circleHeadlights() {

  Serial.println("Circle headlights");
  
  for(int whiteLed = 0; whiteLed < (NUM_HEADLIGHT_LEDS/2); whiteLed++) {
    // Turn our current led on to white, then show the leds
    headlightLeds[whiteLed] = CRGB::White;
    headlightLeds[NUM_HEADLIGHT_LEDS - whiteLed] = CRGB::White;

    // Show the leds (only one of which is set to white, from above)
    FastLED.show();

    // Wait a little bit
    delay(50);

    // Turn our current led back to black for the next loop around
    //leds[whiteLed] = CRGB::Black;

  }

}

void sky_loop() {
  return;
  for(int Counter=0;Counter<255;Counter++) { 
    int ledPosition = random8(NUM_EFFECT_LEDS);   // Step 3A: random dimming LEDs
    effectLeds[ledPosition] = effectLeds[ledPosition].nscale8_video( random(50) );
    FastLED.show();                    // Step 4B : make the changes visible
    delay(100);                        // just taking a short time interval for testing
 }
}
int idx = 0;
int prevState[4] = {0, 0, 0, 0};
int buttonState = 0;
boolean pacifica = false;
boolean sky = false;

void loop() {

  // Read potentiometer for brightness value
  int v = analogRead(BRIGHTNESS_PIN);
  int diff = lastValue > v ? lastValue - v : v - lastValue;
  if (diff >= 10) {
    lastValue = v;
    int value = map(v, 0, 1023, 0, 255);
    if (value != lastBrightness) {
      lastBrightness = value;
      Serial.print("Setting brightness: ");
      Serial.println(value);
      FastLED.setBrightness(value);
    }
  }
  
  // Loop through buttons on RF to see if any
  // were pushed/released
  for (int i = 0; i < 4; i++) {
    buttonState = digitalRead(buttons[i]);
    if (buttonState != prevState[i]) {
      switch (i) {
        case 0:
          if (buttonState == HIGH) {
            turnHeadlightsOn();
         } else {
            turnHeadlightsOff();
          }
        break;
        case 1:
          if (buttonState == HIGH) {
            if (pacifica) {
              pacifica = false;
              clearEffectsLeds();
            } else {
              pacifica = true;
              sky = false;
            }
          }
        break;
        case 2:
          if (buttonState == HIGH) {
            if (sky) {
              sky = false;
              clearEffectsLeds();
            } else {
              pacifica = false;
              sky = true;
              fill_solid( effectLeds, NUM_EFFECT_LEDS, CRGB::HTMLColorCode(0x1E90FF));
              FastLED.show();
            }
          }
        break;
        case 3:
        break;
      }
    }
    prevState[i] = buttonState;
  }

  EVERY_N_MILLISECONDS( 100) {
    if (sky) {
      sky_loop();
    }
  }
  
  EVERY_N_MILLISECONDS( 20) {
    if (pacifica) {
      pacifica_loop();
      FastLED.show();
    }
  }

}

//////////////////////////////////////////////////////////////////////////
//
// The code for this animation is more complicated than other examples, and 
// while it is "ready to run", and documented in general, it is probably not 
// the best starting point for learning.  Nevertheless, it does illustrate some
// useful techniques.
//
//////////////////////////////////////////////////////////////////////////
//
// In this animation, there are four "layers" of waves of light.  
//
// Each layer moves independently, and each is scaled separately.
//
// All four wave layers are added together on top of each other, and then 
// another filter is applied that adds "whitecaps" of brightness where the 
// waves line up with each other more.  Finally, another pass is taken
// over the led array to 'deepen' (dim) the blues and greens.
//
// The speed and scale and motion each layer varies slowly within independent 
// hand-chosen ranges, which is why the code has a lot of low-speed 'beatsin8' functions
// with a lot of oddly specific numeric ranges.
//
// These three custom blue-green color palettes were inspired by the colors found in
// the waters off the southern coast of California, https://goo.gl/maps/QQgd97jjHesHZVxQ7
//
CRGBPalette16 pacifica_palette_1 = 
    { 0x000507, 0x000409, 0x00030B, 0x00030D, 0x000210, 0x000212, 0x000114, 0x000117, 
      0x000019, 0x00001C, 0x000026, 0x000031, 0x00003B, 0x000046, 0x14554B, 0x28AA50 };
CRGBPalette16 pacifica_palette_2 = 
    { 0x000507, 0x000409, 0x00030B, 0x00030D, 0x000210, 0x000212, 0x000114, 0x000117, 
      0x000019, 0x00001C, 0x000026, 0x000031, 0x00003B, 0x000046, 0x0C5F52, 0x19BE5F };
CRGBPalette16 pacifica_palette_3 = 
    { 0x000208, 0x00030E, 0x000514, 0x00061A, 0x000820, 0x000927, 0x000B2D, 0x000C33, 
      0x000E39, 0x001040, 0x001450, 0x001860, 0x001C70, 0x002080, 0x1040BF, 0x2060FF };


void pacifica_loop()
{
  // Increment the four "color index start" counters, one for each wave layer.
  // Each is incremented at a different speed, and the speeds vary over time.
  static uint16_t sCIStart1, sCIStart2, sCIStart3, sCIStart4;
  static uint32_t sLastms = 0;
  uint32_t ms = GET_MILLIS();
  uint32_t deltams = ms - sLastms;
  sLastms = ms;
  uint16_t speedfactor1 = beatsin16(3, 179, 269);
  uint16_t speedfactor2 = beatsin16(4, 179, 269);
  uint32_t deltams1 = (deltams * speedfactor1) / 256;
  uint32_t deltams2 = (deltams * speedfactor2) / 256;
  uint32_t deltams21 = (deltams1 + deltams2) / 2;
  sCIStart1 += (deltams1 * beatsin88(1011,10,13));
  sCIStart2 -= (deltams21 * beatsin88(777,8,11));
  sCIStart3 -= (deltams1 * beatsin88(501,5,7));
  sCIStart4 -= (deltams2 * beatsin88(257,4,6));

  // Clear out the LED array to a dim background blue-green
  fill_solid( effectLeds, NUM_EFFECT_LEDS, CRGB( 2, 6, 10));

  // Render each of four layers, with different scales and speeds, that vary over time
  pacifica_one_layer( pacifica_palette_1, sCIStart1, beatsin16( 3, 11 * 256, 14 * 256), beatsin8( 10, 70, 130), 0-beat16( 301) );
  pacifica_one_layer( pacifica_palette_2, sCIStart2, beatsin16( 4,  6 * 256,  9 * 256), beatsin8( 17, 40,  80), beat16( 401) );
  pacifica_one_layer( pacifica_palette_3, sCIStart3, 6 * 256, beatsin8( 9, 10,38), 0-beat16(503));
  pacifica_one_layer( pacifica_palette_3, sCIStart4, 5 * 256, beatsin8( 8, 10,28), beat16(601));

  // Add brighter 'whitecaps' where the waves lines up more
  pacifica_add_whitecaps();

  // Deepen the blues and greens a bit
  pacifica_deepen_colors();
}

// Add one layer of waves into the led array
void pacifica_one_layer( CRGBPalette16& p, uint16_t cistart, uint16_t wavescale, uint8_t bri, uint16_t ioff)
{
  uint16_t ci = cistart;
  uint16_t waveangle = ioff;
  uint16_t wavescale_half = (wavescale / 2) + 20;
  for( uint16_t i = 0; i < NUM_EFFECT_LEDS; i++) {
    waveangle += 250;
    uint16_t s16 = sin16( waveangle ) + 32768;
    uint16_t cs = scale16( s16 , wavescale_half ) + wavescale_half;
    ci += cs;
    uint16_t sindex16 = sin16( ci) + 32768;
    uint8_t sindex8 = scale16( sindex16, 240);
    CRGB c = ColorFromPalette( p, sindex8, bri, LINEARBLEND);
    effectLeds[i] += c;
  }
}

// Add extra 'white' to areas where the four layers of light have lined up brightly
void pacifica_add_whitecaps()
{
  uint8_t basethreshold = beatsin8( 9, 55, 65);
  uint8_t wave = beat8( 7 );
  
  for( uint16_t i = 0; i < NUM_EFFECT_LEDS; i++) {
    uint8_t threshold = scale8( sin8( wave), 20) + basethreshold;
    wave += 7;
    uint8_t l = effectLeds[i].getAverageLight();
    if( l > threshold) {
      uint8_t overage = l - threshold;
      uint8_t overage2 = qadd8( overage, overage);
      effectLeds[i] += CRGB( overage, overage2, qadd8( overage2, overage2));
    }
  }
}

// Deepen the blues and greens
void pacifica_deepen_colors()
{
  for( uint16_t i = 0; i < NUM_EFFECT_LEDS; i++) {
    effectLeds[i].blue = scale8( effectLeds[i].blue,  145); 
    effectLeds[i].green= scale8( effectLeds[i].green, 200); 
    effectLeds[i] |= CRGB( 2, 5, 7);
  }
}
