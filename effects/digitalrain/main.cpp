#include <stdlib.h>
#include <time.h>
#include <list>

#include "pico/stdlib.h"

#include "libraries/pico_graphics/pico_graphics.hpp"
#include "libraries/pico_unicorn/pico_unicorn.hpp"

//#define FPS 30         // Maximum frames-per-second

#define DECAY_SCALE 125

using pimoroni::PicoUnicorn;
using pimoroni::PicoGraphics_PenRGB565;
using pimoroni::Point;

PicoUnicorn pico_unicorn;
PicoGraphics_PenRGB565 graphics(PicoUnicorn::WIDTH, PicoUnicorn::HEIGHT, nullptr);

uint8_t penCold;
uint8_t penHot;

uint8_t penHead;
uint8_t penBody1;
uint8_t penBody2;
uint8_t penBody3;
uint8_t penTail;

struct Stream {
    uint8_t PosX;
    uint8_t PosY;

    absolute_time_t CreationTime;
};

std::list<Stream> streams;

void reset() {
    streams.clear();

    graphics.clear();

    pico_unicorn.update(&graphics);
}

void init() {
    penCold = graphics.create_pen(0, 0, 0); // #000000
    penHot = graphics.create_pen(255, 255, 255); // #ffffff

    penHead = graphics.create_pen(146, 229, 161); // #92e5a1
    penBody1 = graphics.create_pen(128, 206, 135); // #80ce87
    penBody2 = graphics.create_pen(34, 180, 85); // #22b455
    penBody3 = graphics.create_pen(32, 72, 41); // #204829
    penTail = graphics.create_pen(2, 2, 4); // #020204

    pico_unicorn.init();

    reset();
}

void create_stream() {
    // NOTE: When to spawn a new stream:
    //       * if there are less than 3 on screen at any time
    //       * 50% chance if the youngest stream is more than 1/3 down the length of the screen
    //       * otherwise, chance of (1 / # of columns)

    if (streams.size() < 3 || ((streams.end()->PosX > PicoUnicorn::WIDTH / 3) && (rand() % 2 == 0)) || (rand() % PicoUnicorn::HEIGHT == 0)) {
        //TODO use 'new' keyword?
        Stream s = Stream();

        s.PosX = 0;

        //TODO this could look back even further (compare .PosX)
        // don't follow the most recent stream too closely
        do {
            s.PosY = rand() % PicoUnicorn::HEIGHT;
        } while (streams.end()->PosY == s.PosY && streams.end()->PosX <= 5);

        s.CreationTime = get_absolute_time();

        //TODO is the struct still valid after we exit this routine? we may need to allocate on heap
        streams.push_back(s);
    }
}

void update() {
    for (std::list<Stream>::iterator i = streams.begin(); i != streams.end(); /* i++; */)
    {
        //int64_t step = absolute_time_diff_us(i->CreationTime, get_absolute_time()) / 1000 / FPS;
        int64_t step = absolute_time_diff_us(i->CreationTime, get_absolute_time()) / 1000;

        i->PosX++;

        graphics.set_pen(penCold);
        graphics.pixel(Point(i->PosX - 5, i->PosY));

        graphics.set_pen(penTail);
        graphics.pixel(Point(i->PosX - 4, i->PosY));

        graphics.set_pen(penBody3);
        graphics.pixel(Point(i->PosX - 3, i->PosY));

        graphics.set_pen(penBody2);
        graphics.pixel(Point(i->PosX - 2, i->PosY));

        graphics.set_pen(penBody1);
        graphics.pixel(Point(i->PosX - 1, i->PosY));

        graphics.set_pen(penHead);
        graphics.pixel(Point(i->PosX, i->PosY));

        if (i->PosX > PicoUnicorn::WIDTH + 5) {
            i = streams.erase(i);
        } else {
            i++;
        }
    }

    pico_unicorn.update(&graphics);
}

int main() {
    stdio_init_all();

    init();

    while (true) {
        //sleep_ms(1000 / FPS);
        sleep_ms(DECAY_SCALE);

        if (pico_unicorn.is_pressed(PicoUnicorn::A)) {
            reset();

            continue;
        }

        create_stream();

        update();
    }
}
