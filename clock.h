const uint8_t digits4x4[5][4] = {
    {
        0b00100110,
        0b00101010,
        0b00101010,
        0b00101100,
    },
    {
        0b11001110,
        0b01100010,
        0b00101000,
        0b11001110,
    },
    {
        0b11101010,
        0b10001010,
        0b00101110,
        0b11100010,
    },
    {
        0b11101000,
        0b00101100,
        0b00101010,
        0b00100110,
    },
    {
        0b11001110,
        0b10101010,
        0b01101110,
        0b00101110,
    }
};

void digitDraw(int xoffset, int yoffset, int val, CRGB color) {
    for (int y=0; y<4; y++) {
        uint8_t row = digits4x4[val/2][y] >> (4*(val%2));
        for (int x=0; x<4; x++) {
            bool bit = (row>>(3-x)) & 1;
            int pos = (yoffset+y)*8 + (xoffset+x);
            if (bit) {
                grid[pos] = color;
            }
        }
    }
}

void displayTimeDigits(bool day, unsigned long duration = 20*SECOND) {
    unsigned long end = millis() + duration;

    bool flash = false;

    while (millis() < end) {
        struct tm info;
        getLocalTime(&info);

        int h0 = (info.tm_hour /  1) % 10;
        int h1 = (info.tm_hour / 10) % 10;
        int m0 = (info.tm_min /  1) % 10;
        int m1 = (info.tm_min / 10) % 10;

        uint8_t hhue = day?HUE_AQUA:HUE_ORANGE;
        uint8_t mhue = day?HUE_ORANGE:HUE_RED;

        // Draw background
        if (day) {
            fill_solid(grid, 32, CHSV(hhue, 120, 32));
            fill_solid(grid+32, 32, CHSV(mhue, 120, 32));
        } else {
            clear();
        }
        
        // Draw foreground
        CRGB hcolor = CHSV(hhue, 240, 120);
        CRGB mcolor = CHSV(mhue, 120, 120);
        digitDraw(0, 0, h1, hcolor);
        digitDraw(4, 0, h0, hcolor);
        digitDraw(0, 4, m1, mcolor);
        digitDraw(4, 4, m0, mcolor);

        show();
        pause(SECOND);

        flash = !flash;
    }
}
