#include <FastLED.h>
#include "picker.h"
#include "network.h"

#define NEOPIXEL_PIN 32
#define GRIDLEN 64
#define WFM_PASSWORD "artsy fartsy"
#define ART_URL "https://sweetums.woozle.org/public/wallart.bin"

#define MILLISECOND 1
#define SECOND (1000 * MILLISECOND)

CRGB grid[GRIDLEN];

void setup() {
  FastLED.addLeds<WS2812, NEOPIXEL_PIN, GRB>(grid, GRIDLEN);
  FastLED.setBrightness(32);
  network_setup(WFM_PASSWORD);
}
 
void fade(int cycles = 2) {
  int reps = (cycles*GRIDLEN) + random(GRIDLEN);
  int hue = random(256);
  for (int i = 0; i < reps; i++) {
    for (int pos = 0; pos < 8; pos++) {
      grid[(i+pos) % GRIDLEN] = CHSV(hue, 255, pos * 32);
    }
    FastLED.show();
    pause(80);
  }
}

void singleCursor(int count = 80) {
  for (int i = 0; i < count; i++) {
    grid[20] = CHSV(0, 210, 127 * (i%2));
    FastLED.show();
    pause(120);
  }
}

#define NUM_SPARKS 3
void sparkle(int cycles=50) {
	int pos[NUM_SPARKS] = {0};

	for (int i = 0; i < cycles; i++) {
		for (int j = 0; j < NUM_SPARKS; j++) {
			grid[pos[j]] = CRGB::Black;
			pos[j] = random(GRIDLEN);
			grid[pos[j]] = CRGB::Gray;
		}
		FastLED.show();
		pause(40);
	}
}

#define NUM_GLITCH 4
#define GLITCH_FRAMES 64
void glitchPulse(int cycles=1000) {
  int steps[NUM_GLITCH] = {0};
  int pos[NUM_GLITCH] = {0};
  CRGB color[NUM_GLITCH];

  for (int i = 0; i < NUM_GLITCH; i++) {
    steps[i] = GLITCH_FRAMES / NUM_GLITCH * i;
    color[i] = CRGB::Brown;
  }
  
  for (int frame = 0; frame < cycles; frame++) {
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
    pause(100);
  }
}

void conwayish(int cycles=5000) {
  uint8_t total[GRIDLEN];
  uint8_t left[GRIDLEN] = {0};
  uint8_t hue = random(0, 64);

  for (int i = 0; i < GRIDLEN; i++) {
    total[i] = random(64, 256);
    left[i] = total[i];
  }

  for (int frame = 0; frame < cycles; frame++) {
    for (int i = 0; i < GRIDLEN; i++) {
      if (left[i] == 0) {
        left[i] = total[i];
        if (grid[i].getLuma() == 0) {
          grid[i].setHSV(hue, 180, 192);
        } else {
          grid[i] = CRGB::Black;
        }
      } else {
        left[i]--;
      }
    }
    FastLED.show();
    pause(20);
  }
}

void cm5(int cycles=200) {
  for (int frame = 0; frame < cycles; frame++) {
    int val = 127 * random(2);
    for (int pos = 0; pos < GRIDLEN; pos++) {
      if (pos < GRIDLEN-1) {
        grid[pos] = grid[pos + 1];
      } else {
      grid[pos] = CHSV(0, 255, val);
      }
    }
    FastLED.show();
    pause(500);
  }
}

// Art from the network
bool NetArtExists = false;
CRGB NetArt[GRIDLEN];

void netart() {
	if (NetArtExists) {
		memcpy(grid, NetArt, GRIDLEN*3);
		FastLED.show();
		pause(10 * SECOND);
	}
}

int netgetStatus(int hue) {
	static int positions[4] = {0};
	for (int j = 0; j < 4; j++) {
		grid[positions[j]] = CHSV(0, 0, 0);
		positions[j] = random(GRIDLEN);
		grid[positions[j]] = CHSV(hue, 255, 180);
	}
	FastLED.show();
	pause(500);
	return hue;
}

void netget(int count=30) {
	int hue = netgetStatus(HUE_BLUE);

	if (connected()) {
		WiFiClientSecure scli;

		hue = netgetStatus(hue - 32);
		scli.setInsecure();

		{
			HTTPClient https;

			if (https.begin(scli, ART_URL)) {
				hue = netgetStatus(hue - 32);
				int code = https.GET();
				if (code == HTTP_CODE_OK) {
					hue = netgetStatus(hue - 32);
					String resp = https.getString();
					for (int i = 0; i < resp.length(); i += 3) {
						if (resp.length() < i+3) {
							break;
						}
						CRGB pixel = CRGB(resp[i], resp[i+1], resp[i+2]);
						NetArt[i/3] = rgb2hsv_approximate(pixel);
					}
					NetArtExists = true;
					hue = hue - 32;
				}
				https.end();

				FastLED.show();
				for (int i = 0; i < 4*4; i++) {
					pause(250);
				}
			}
			scli.stop();
		}
	}

	for (int i = 0; i < count; i++) {
		netgetStatus(hue);
	}
}

const int spinner_pos[4] = {27, 28, 36, 35};
void spinner(int count=32) {
	for (int i = 0; i < count; i++) {
		int pos = spinner_pos[i % 4];
		grid[pos] = CRGB::OliveDrab;
		FastLED.show();
		pause(125);
		grid[pos] = CRGB::Black;
	}
}

void loop() {
	Picker p;

	if (p.Pick(1)) {
		fade();
		singleCursor(20);
	} else if (p.Pick(1)) {
		sparkle();
	} else if (p.Pick(4)) {
		singleCursor();
	} else if (p.Pick(8)) {
		conwayish();
	} else if (p.Pick(8)) {
		glitchPulse();
	} else if (p.Pick(8)) {
		cm5();
	} else if (p.Pick(10)) {
		spinner();
	} else if (p.Pick(4) || !connected()) {
		netget();
	}
}
