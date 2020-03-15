# Trinket IO demo
# Welcome to CircuitPython 3.1.1 :)

import board
from digitalio import DigitalInOut, Direction, Pull
import adafruit_dotstar as dotstar
import adafruit_fancyled.adafruit_fancyled as fancy
import time
import neopixel
import random

# One pixel connected internally!
dot = dotstar.DotStar(board.APA102_SCK, board.APA102_MOSI, 1, brightness=0.2)

# Built in red LED
led = DigitalInOut(board.D13)
led.direction = Direction.OUTPUT

# NeoPixel strip (of 16 LEDs) connected on D4
GRIDLEN = 64
grid = neopixel.NeoPixel(board.D4, GRIDLEN, brightness=0.2, auto_write=False, pixel_order=neopixel.GRB)


class GlitchPixel:
    def __init__(self):
        self.init()
        self.nsteps = 64
        self.step = random.randrange(self.nsteps)
    
    def init(self):
        self.step = 0
        self.pos = random.randrange(GRIDLEN)
        self.color = fancy.CHSV(random.random()).pack()

    def frame(self):
        bmask = (0xff * self.step // 32) & 0xff
        if self.step > self.nsteps/2:
            bmask = 0xff - bmask
        mask = (bmask << 16) | (bmask << 8) | (bmask << 0)
        color = self.color & mask
        grid[self.pos] = color

        self.step += 1
        if self.step > self.nsteps:
            self.init()

def fade():
    reps = 300 + random.randrange(GRIDLEN)
    hue = random.randrange(256)
    colors = [fancy.CHSV(hue, 255, v).pack() for v in range(0, 256, 32)]
    rcolors = colors[:]
    rcolors.reverse()
    colors = colors + rcolors
    for count in range(reps):
        pos = count % GRIDLEN
        for color in colors:
            grid[pos] = color
            pos -= 1
        grid.show()

def singleCursor():
    red = fancy.CHSV(0, 210, 127).pack()
    pos = 20
    for i in range(80):
        grid[pos] = red * (i % 2)
        grid.show()
        time.sleep(0.08)
    
def sparkle():
    white = fancy.CHSV(0, 0, 127).pack()
    pos = [0,0,0]
    for i in range(50):
        for j in range(len(pos)):
            pos[j] = random.randrange(GRIDLEN)
            grid[pos[j]] = white
        grid.show()
        for p in pos:
            grid[p] = 0
        grid.show()

def glitchPulse():
    grid.fill(0)
    pixels = []
    for i in range(4):
        p = GlitchPixel()
        pixels.append(p)

    for f in range(1000):
        for p in pixels:
            p.frame()
        grid.show()
        time.sleep(0.1)

def loop():
    fade()
    singleCursor()
    sparkle()
    glitchPulse()

while True:
    loop()
