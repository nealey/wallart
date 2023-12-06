Wall Art
========

This project's primary home is
https://git.woozle.org/neale/wallart/

This an art piece I have
hanging in the wall of my house, with
pixels crammed into a cardboard box.
Newer versions use an 8x8 grid,
in a 3D-printed enclosure.

It doesn't display anything significant.
The idea is to have something to look at if you're idle,
without it being a distraction from more pressing issues.
If you're familiar with the album "music for airports",
it's that.

Most people, after watching it for a bit,
form different ideas about what it's displaying.
That's cool.


Setup
-----

If the red light on the board is lit,
that means it doesn't know how to connect to the WiFi.

Get your phone or computer to connect to an access point
called "WallArt".
The password is "artsy fartsy", unless you changed it in the source code.
Once connected,
you should get a browser window that lets you connect.
If not, try going to http://neverssl.com/.

Please configure the clock before the WiFi.
This will set up your time zone,
so it doesn't blind you in the middle of the night.

You can clear the wifi information with a reset.


Reset
------

Plug the device in,
and connect GND to pin A0 (right next to GND).
The red LED on the Feather board should come on immediately,
indicating it needs the network set up again.


Network Server
--------------

If you `#define` a wallart server,
the program will periodically fetch an image/animation from it,
and display that image/animation.
I added this so we could send little pictures to my daughter in college.

Because the fetch takes some time,
one of the animation patterns actually shows progress of the fetch.
It's the one with the four lights of the same color.
After it's either succeeded or failed,
it stays on that color for a while,
so you can tell how everything is going.
Here's what the colors indicate:

Color it stays on | Status
--- | ---
Blue | Unable to connect to access point
Aqua | Some sort of problem initiating the HTTPS request (DNS?)
Green | HTTPS server request failed (404 or similar)
Yellow | Image download failed
Orange | Everything worked!

A happy setup will cycle through each color once,
and then display orange for a while.


Clock
-----

At night, 
and sometimes during the day,
it displays something like a clock.

* Each pixel in the top row is 1 hour (3600 seconds)
* Each pixel in the middle row is 5 minutes (300 seconds)
* Each pixel in the bottom row is 25 seconds
* There are four pixels around the bottom that move every 5 seconds


Update
------

You can upload a new version of the firmware.
Reset the device, 
and select the "Update" button instead of configuring WiFi.
Then you can upload the new .bin firmware file.

You may have to reconfigure networking after this.

### Uploading from CLI

    python3 esptool.py --chip esp32 --port "/dev/ttyUSB0" --baud 921600  --before default_reset --after hard_reset write_flash  -z --flash_mode dio --flash_freq 80m --flash_size 4MB 0x1000 wallart.ino.bootloader.bin 0x8000 wallart.ino.partitions.bin 0xe000 boot_app0.bin 0x10000 wallart.ino.bin 



Philosophy
----------

I was trying to make pixels fade in and out,
and got the math wrong.
But what I wound up with looked more interesting
than what I was trying to make,
so I kept it.

After many trips to Meow Wolf's House of Eternal Return,
I realized that the presentation of the piece
is more important
than how complicated the algorithm is.

It's hard to encapsulate in a git repository,
but if you want to make NeoPixel art,
think hard about what the end result should look like.
It's not enough to make a cool light show;
it has to make people wonder "what is that for?"
