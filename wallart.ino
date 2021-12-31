#include <FastLED.h>

#define NEOPIXEL_PIN 2
#define GRIDLEN 50

#define MILLISECOND 1
#define SECOND (1000 * MILLISECOND)

CRGB grid[GRIDLEN];

void fade() {
  int reps = 300 + random(GRIDLEN);
  int hue = random(256);
  for (int i = 0; i < reps; i++) {
    FastLED.clear();
    for (int pos = 0; pos < 8; pos++) {
      grid[(i+pos) % GRIDLEN] = CHSV(hue, 255, pos * 32);
    }
    FastLED.show();
    delay(80);
  }
}

void singleCursor() {
  for (int i = 0; i < 80; i++) {
    grid[20] = CHSV(0, 210, 127 * (i%2));
    FastLED.show();
    delay(120);
  }
}

#define NUM_SPARKS 3
void sparkle() {
  int pos[NUM_SPARKS] = {0};
  for (int i = 0; i < 50; i++) {
    for (int j = 0; j < NUM_SPARKS; j++) {
      pos[j] = random(GRIDLEN);
      grid[pos[j]] = CRGB::Gray;
    }
    FastLED.show();
    for (int j = 0; j < NUM_SPARKS; j++) {
      grid[pos[j]] = CRGB::Black;
    }
    delay(40);
  }
}

#define NUM_GLITCH 4
#define GLITCH_FRAMES 64
void glitchPulse() {
  int steps[NUM_GLITCH] = {0};
  int pos[NUM_GLITCH] = {0};
  CRGB color[NUM_GLITCH];

  for (int i = 0; i < NUM_GLITCH; i++) {
    steps[i] = GLITCH_FRAMES / NUM_GLITCH * i;
    color[i] = CRGB::Brown;
  }
  
  for (int frame = 0; frame < 1000; frame++) {
    for (int i = 0; i < NUM_GLITCH; i++) {
      if (steps[i] == 0) {
        steps[i] = GLITCH_FRAMES;
        pos[i] = random(GRIDLEN);
        color[i] = CHSV(random(256), 64 + random(64), 255);
      } 
      CRGB c = color[i];
      int bmask = (0xff * steps[i] / 32) & 0xff;
      if (steps[i] == GLITCH_FRAMES/2) {
        bmask = 0xff - bmask;
      }
      c.red &= bmask;
      c.green &= bmask;
      c.blue &= bmask;
      grid[pos[i]] = c;
      steps[i]--;
    }
    FastLED.show();
    delay(100);
  }

}

void setup() {
  FastLED.addLeds<WS2812, NEOPIXEL_PIN, RGB>(grid, GRIDLEN);
  FastLED.setBrightness(52);
  pinMode(LED_BUILTIN, OUTPUT);
}

void loop() {
  fade();
  singleCursor();
  sparkle();
  glitchPulse();
}