Wall Art
========

This project's primary home is
https://git.woozle.org/neale/wallart/

This an art piece I have
hanging in the wall of my house, with
pixels crammed into a cardboard box.

It doesn't display anything significant.
The idea is to have something to look at if you're idle,
without it being a distraction from more pressing issues.
If you're familiar with the album "music for airports",
it's that.

Most people, after watching it for a bit,
form different ideas about what it's displaying.
That's cool.


Network Server
--------------

If you provide a wallart server in 
`network-server.h`,
the program will periodically fetch an image/animation from it,
and display that image/animation.

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

