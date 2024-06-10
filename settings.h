#pragma once

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

/*
 * The password used when running as an access point.
 */
#define WFM_PASSWORD "artsy fartsy"

/*
 * The output pin your neopixel array is connected to.
 */
#define NEOPIXEL_PIN 32
