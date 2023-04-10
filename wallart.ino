#include <FastLED.h>
#include <ArduinoHttpClient.h>
#include <WiFiClientSecure.h>
#include <WiFiUdp.h>
#include <NTPClient.h>
#include <Time.h>
#include "durations.h"
#include "timezones.h"
#include "picker.h"
#include "network.h"

#define NEOPIXEL_PIN 32
#define GRIDLEN 64
#define WFM_PASSWORD "artsy fartsy"
#define TIMEZONE TZ_US_Mountain

/* 
 * The hours when the day begins and ends.
 * At night, all you get is a dim clock.
 */
#define DAY_BEGIN 6
#define DAY_END 20
#define DAY_BRIGHTNESS 0x80
#define NIGHT_BRIGHTNESS 0x10

/*
 * Define these to fetch from a wallart-server
 *
 * https://git.woozle.org/neale/wallart-server
 */
#define ART_HOSTNAME "www.woozle.org"
#define ART_PORT 443
#define ART_PATH "/wallart/wallart.bin"

#define HTTPS_TIMEOUT (2 * SECOND)

CRGB grid[GRIDLEN];

WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP);

void setup() {
  FastLED.addLeds<WS2812, NEOPIXEL_PIN, GRB>(grid, GRIDLEN);
  // Maybe it's the plexiglass but for my build I really need to dial back the red
  FastLED.setCorrection(0xc0ffff);
  network_setup(WFM_PASSWORD);
}

bool updateTime() {
  if (timeClient.update()) {
    time_t now = timeClient.getEpochTime();
    time_t local = TIMEZONE.toLocal(now);
    setTime(local);
    return true;
  }
  return false;
}

void fade(int cycles = 2) {
  int reps = (cycles*GRIDLEN) + random(GRIDLEN);
  int hue = random(256);
  for (int i = 0; i < reps; i++) {
    for (int pos = 0; pos < 8; pos++) {
      uint8_t p = cm5xlat(8, (i+pos) % GRIDLEN);
      grid[p] = CHSV(hue, 255, pos * 32);
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

uint8_t cm5xlat(uint8_t width, uint8_t pos) {
  if (width == 0) {
    return pos;
  }

  uint8_t x = pos % width;
  uint8_t y = pos / width;
  uint8_t odd = y % 2;

  return (y*width) + ((width-x-1)*odd) + (x*(1-odd));
}

void cm5(uint8_t width=0, int cycles=200) {
  for (int frame = 0; frame < cycles; frame++) {
    int val = 127 * random(2);
    for (uint8_t pos = 0; pos < GRIDLEN; pos++) {
      uint8_t xpos = cm5xlat(width, pos);
      if (pos < GRIDLEN-1) {
        uint8_t x2pos = cm5xlat(width, pos+1);
        grid[xpos] = grid[x2pos];
      } else {
        grid[xpos] = CHSV(0, 255, val);
      }
    }
    FastLED.show();
    pause(500);
  }
}

// Art from the network
int NetArtFrames = 0;
CRGB NetArt[8][GRIDLEN];

void netart(int count=40) {
	if (NetArtFrames < 1) {
		return;
	}

	for (int i = 0; i < count; i++) {
		memcpy(grid, NetArt[i%NetArtFrames], GRIDLEN*3);
		FastLED.show();
		pause(500);
	}
}

uint8_t netgetStatus(uint8_t hue) {
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

void netget(int count=60) {
	uint8_t hue = netgetStatus(HUE_BLUE);

#if defined(ART_HOSTNAME) && defined(ART_PORT) && defined(ART_PATH)
	if (connected()) {
		WiFiClientSecure scli;

		hue = netgetStatus(HUE_AQUA);
		scli.setInsecure();

		HttpClient https(scli, ART_HOSTNAME, ART_PORT);
		do {
			if (https.get(ART_PATH) != 0) break;
			hue = netgetStatus(HUE_GREEN);

			if (https.skipResponseHeaders() != HTTP_SUCCESS) break;
			hue = netgetStatus(HUE_YELLOW);

			int artlen = https.read((uint8_t *)NetArt, sizeof(NetArt));
			hue = netgetStatus(HUE_ORANGE);
			NetArtFrames = (artlen / 3) / GRIDLEN;
		} while(false);
		https.stop();
	}
#endif

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

void displayTime(unsigned long duration = 20 * SECOND) {
  if (timeStatus() != timeSet) return;
  unsigned long end = millis() + duration;
  FastLED.clear();

  while (millis() < end) {
    updateTime();
    int hh = hour();
    int mmss = now() % 3600;
    uint8_t hue = HUE_YELLOW;

    // Top: Hours
    if (isPM()) {
      hue = HUE_ORANGE;
      hh -= 12;
    }

    // Middle: 5m (300s)
    uint8_t mm = (mmss/300) % 12;

    // Bottom: 25s
    uint8_t ss = (mmss/25) % 12;

    // Outer: 5s
    uint8_t s = (mmss/5) % 5;
    grid[64 -  7 - 1] = CHSV(HUE_PURPLE, 128, (s==1)?96:0);
    grid[64 - 15 - 1] = CHSV(HUE_PURPLE, 128, (s==2)?96:0);
    grid[64 -  8 - 1] = CHSV(HUE_PURPLE, 128, (s==3)?96:0);
    grid[64 -  0 - 1] = CHSV(HUE_PURPLE, 128, (s==4)?96:0);

    for (int i = 0; i < 12; i++) {
      // Omit first and last position on a row
      int pos = i + 1;
      if (pos > 6) {
        pos += 2;
      }

      grid[pos + 0] = CHSV(hue, 255, (i<hh)?128:48);
      grid[pos + 24] = CHSV(HUE_RED, 255, (i<mm)?128:48);
      grid[pos + 48] = CHSV(HUE_PINK, 128, (i<ss)?96:48);
    }
    FastLED.show();

    pause(250 * MILLISECOND);
  }
}

void loop() {
	Picker p;
  uint8_t getprob = 4;
  bool conn = connected();
  bool day = true;

  updateTime();
  if (timeStatus() == timeSet) {
    int hh = hour();
    day = ((hh >= DAY_BEGIN) && (hh < DAY_END));
  }
  FastLED.setBrightness(day?DAY_BRIGHTNESS:NIGHT_BRIGHTNESS);

  // If we don't yet have net art, try a little harder to get it.
  if ((NetArtFrames == 0) || !conn) {
    getprob = 16;
  }

  if (!day || p.Pick(4)) {
    // At night, only ever show the clock
    displayTime(2 * MINUTE);
  } else if (p.Pick(getprob)) {
    netget();
  } else if (day && p.Pick(4)) {
    // These can be hella bright
		netart();
  } else if (p.Pick(1)) {
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
	} else if (p.Pick(2)) {
		cm5(0);
  } else if (p.Pick(2)) {
    cm5(8);
  } else if (p.Pick(2)) { 
    cm5(16);
	}
}
