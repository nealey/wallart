#include <FastLED.h>
#include <ArduinoHttpClient.h>
#include <WiFiClientSecure.h>
#include <WiFiUdp.h>
#include <TimeLib.h>
#include "durations.h"
#include "picker.h"
#include "network.h"
#include "settings.h"

#define VERSION 2
#define GRIDLEN 64

#define HTTPS_TIMEOUT (2 * SECOND)
#define IMAGE_PULL_MIN_INTERVAL (5 * MINUTE)

CRGB grid[GRIDLEN];
CRGB actual[GRIDLEN];

// Rotation, in degrees: [0, 90, 180, 270]
int rotation = 0;

void show() {
  for (int y = 0; y < 8; y++) {
    for (int x = 0; x < 8; x++) {
      int pos;
      switch (rotation) {
        case 90:
          pos = (x)*8 + (7-y);
          break;
        case 180:
          pos = (7-y)*8 + (7-x);
          break;
        case 270:
          pos = (7-x)*8 + y;
          break;
        default:
          pos = (y)*8 + x;
          break;
      }
      actual[pos] = grid[y*8 + x];
    }
  }
  FastLED.show();
}

void clear() {
  fill_solid(grid, GRIDLEN, CRGB::Black);
}

// I am so ashamed of this.
// But C++ is a real pain for me at this point.
#include "clock.h"

void fade(int cycles = 2) {
  int reps = (cycles*GRIDLEN) + random(GRIDLEN);
  int hue = random(256);
  for (int i = 0; i < reps; i++) {
    for (int pos = 0; pos < 8; pos++) {
      uint8_t p = cm5xlat(8, (i+pos) % GRIDLEN);
      grid[p] = CHSV(hue, 255, pos * 32);
    }
    show();
    pause(80);
  }
}

void singleCursor(int count = 80) {
  for (int i = 0; i < count; i++) {
    grid[20] = CHSV(0, 210, 127 * (i%2));
    show();
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
		show();
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
    show();
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
    show();
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
    show();
    pause(500);
  }
}

// Display MAC address at startup.
void displayMacAddress(int cycles=40) {
  uint64_t addr = ESP.getEfuseMac();

  Serial.println("mac=" + String(ESP.getEfuseMac(), HEX));

  // Set some custom things per device.
  // It would have been nice if doing this in the Access Point UI were easier than switching the MAC address.
  switch (addr) {
    case 0x18fc1d519140:
      rotation = 270;
      break;
  }

  for (; cycles > 0; cycles -= 1) {
    // Top: version
    for (int i = 0; i < 8; i++) {
      bool bit = (VERSION>>i) & 1;
      grid[7-i] = bit ? CRGB::Black : CRGB::Aqua;
    }

    // Middle: MAC address
    for (int octet = 0; octet < 6; octet++) {
      for (int i = 0; i < 8; i++) {
        int pos = 8 + (octet*8) + (7-i);
        bool bit = (addr>>(octet*8 + i)) & 1;
        grid[pos] = bit ? CRGB::Yellow: CRGB::Black;
      }
    }

    // Bottom: connected status
    fill_solid(grid+56, 8, connected() ? CRGB::Aqua : CRGB::Red);

    show();
    pause(250*MILLISECOND);
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
		show();
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
	show();
	pause(500);
	return hue;
}

void netget(int count=60) {
	uint8_t hue = netgetStatus(HUE_BLUE);
  static unsigned long nextPull = 0; // when to pull next

#if defined(ART_HOSTNAME) && defined(ART_PORT) && defined(ART_PATH)
  if (millis() < nextPull) {
    // Let's not bombard the server
    hue = HUE_ORANGE;
  } else if (connected()) {
		WiFiClientSecure scli;

    nextPull = millis() + IMAGE_PULL_MIN_INTERVAL;

		hue = netgetStatus(HUE_AQUA);
		scli.setInsecure();

		HttpClient https(scli, ART_HOSTNAME, ART_PORT);
		do {
      String path = String(ART_PATH) + "?mac=" + String(ESP.getEfuseMac(), HEX);
      Serial.println(path);
			if (https.get(path) != 0) break;
			hue = netgetStatus(HUE_GREEN);

			if (https.skipResponseHeaders() != HTTP_SUCCESS) break;
			hue = netgetStatus(HUE_YELLOW);

      size_t readBytes = 0;
      for (int i = 0; i < 12; i++) {
        size_t artBytesLeft = sizeof(NetArt) - readBytes;

        if (https.endOfBodyReached() || (artBytesLeft == 0)) {
          hue = netgetStatus(HUE_ORANGE);
    			NetArtFrames = (readBytes / 3) / GRIDLEN;
          break;
        }
        int l = https.read((uint8_t *)NetArt + readBytes, artBytesLeft);
        if (-1 == l) {
          break;
        }
        readBytes += l;
      }
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
		show();
		pause(125);
		grid[pos] = CRGB::Black;
	}
}

void displayTimeDozenal(unsigned long duration = 20*SECOND) {
  if (!clock_is_set()) return;
  unsigned long end = millis() + duration;

  clear();

  while (millis() < end) {
    struct tm info;
    getLocalTime(&info);

    int hh = info.tm_hour;
    int mmss = (info.tm_min * 60) + info.tm_sec;
    uint8_t hue = HUE_YELLOW;

    // Top: Hours
    if (hh >= 12) {
      hue = HUE_ORANGE;
      hh -= 12;
    }

    // Middle: 5m (300s)
    uint8_t mm = (mmss/300) % 12;

    // Bottom: 25s
    uint8_t ss = (mmss/25) % 12;

    // Outer: 5s
    uint8_t s = (mmss/5) % 5;
    grid[64 -  7 - 1] = CHSV(HUE_GREEN, 128, (s==1)?96:0);
    grid[64 - 15 - 1] = CHSV(HUE_GREEN, 128, (s==2)?96:0);
    grid[64 -  8 - 1] = CHSV(HUE_GREEN, 128, (s==3)?96:0);
    grid[64 -  0 - 1] = CHSV(HUE_GREEN, 128, (s==4)?96:0);

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
    show();

    pause(250 * MILLISECOND);
  }
}

void setup() {
  pinMode(RESET_PIN, INPUT_PULLUP);
  pinMode(LED_BUILTIN, OUTPUT);
  Serial.begin(19200);
  FastLED.addLeds<WS2812, NEOPIXEL_PIN, GRB>(actual, GRIDLEN);
  // Maybe it's the plexiglass, but for my build, I need to dial back the red
  //FastLED.setCorrection(0xd0ffff);
  network_setup(WFM_PASSWORD);

  // Show our mac address, for debugging?
  FastLED.setBrightness(DAY_BRIGHTNESS);
  displayMacAddress();
  sparkle();
}

void loop() {
	Picker p;
  bool day = true;

  if (clock_is_set()) {
    struct tm info;
    getLocalTime(&info);
    day = ((info.tm_hour >= DAY_BEGIN) && (info.tm_hour < DAY_END));
  }
  FastLED.setBrightness(day?DAY_BRIGHTNESS:NIGHT_BRIGHTNESS);

  // At night, always display the clock
  if (!day && clock_is_set()) {
    displayTimeDigits(day);
    return;
  }
  
  if (p.Pick(4) && clock_is_set()) {
    displayTimeDigits(day, 2 * MINUTE);
  } else if (p.Pick(4) && clock_is_set()) {
    displayTimeDozenal();
  } else if (p.Pick(4)) {
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
