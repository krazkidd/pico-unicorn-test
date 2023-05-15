#include <stdlib.h>
#include <time.h>
#include <list>

#include "pico/stdlib.h"

#include "libraries/pico_graphics/pico_graphics.hpp"
#include "libraries/pico_unicorn/pico_unicorn.hpp"

using pimoroni::PicoUnicorn;
using pimoroni::PicoGraphics_PenRGB565;
using pimoroni::Point;

PicoUnicorn pico_unicorn;
PicoGraphics_PenRGB565 graphics(PicoUnicorn::WIDTH, PicoUnicorn::HEIGHT, nullptr);

int main() {
    stdio_init_all();

    pico_unicorn.init();

    graphics.clear();

    graphics.set_pen(255, 0, 0);

    while (true) {
        graphics.pixel(Point(0, 0));

        pico_unicorn.update(&graphics);
    }
}
