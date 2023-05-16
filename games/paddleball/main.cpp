#include <stdlib.h>
#include <time.h>
#include <list>

#include "pico/stdlib.h"

#include "libraries/pico_graphics/pico_graphics.hpp"
#include "libraries/pico_unicorn/pico_unicorn.hpp"

#define FPS 15         // Maximum frames-per-second

using pimoroni::PicoUnicorn;
using pimoroni::PicoGraphics_PenRGB565;
using pimoroni::Point;

PicoUnicorn pico_unicorn;
PicoGraphics_PenRGB565 graphics(PicoUnicorn::WIDTH, PicoUnicorn::HEIGHT, nullptr);

uint8_t penCold;

uint8_t penBall;
uint8_t penPaddleAB;
uint8_t penPaddleXY;

struct Ball {
    int8_t PosX;
    int8_t PosY;
    int8_t VelocityX;
    int8_t VelocityY;
};

struct Paddle {
    int8_t PosX;
    int8_t PosY;
};

Ball ball;
Paddle paddleAB;
Paddle paddleXY;

void reset() {
    graphics.clear();

    pico_unicorn.update(&graphics);
}

void init() {
    penCold = graphics.create_pen(0, 0, 0); // #000000

    penBall = graphics.create_pen(255, 255, 255); // #ffffff
    penPaddleAB = graphics.create_pen(51, 209, 122); // #33d17a
    penPaddleXY = graphics.create_pen(255, 120, 0); // #ff7800

    ball.PosX = PicoUnicorn::WIDTH / 2;
    ball.PosY = PicoUnicorn::HEIGHT / 2;
    ball.VelocityX = (rand() % 2 == 0) ? 1 : -1;
    ball.VelocityY = 0;

    paddleAB.PosX = 0;
    paddleAB.PosY = PicoUnicorn::HEIGHT / 2;

    paddleXY.PosX = PicoUnicorn::WIDTH - 1;
    paddleXY.PosY = PicoUnicorn::HEIGHT / 2;

    pico_unicorn.init();

    reset();
}

void updateBall(const Ball& b, uint8_t pen) {
    graphics.set_pen(penCold);
    graphics.pixel(Point(b.PosX - b.VelocityX, b.PosY - b.VelocityY));

    // BUG: We have to erase the edge rows because our bounce logic
    //      seems to not be aligned with the draw logic.

    for (int x = 0; x < PicoUnicorn::WIDTH; x++) {
        graphics.pixel(Point(x, 0));
        graphics.pixel(Point(x, PicoUnicorn::HEIGHT - 1));
    }

    graphics.set_pen(pen);
    graphics.pixel(Point(b.PosX, b.PosY));
}

void updatePaddle(const Paddle& p, uint8_t pen) {
    graphics.set_pen(penCold);

    // NOTE: We have to erase the entire column in case the ball passes.

    for (int y = 0; y < p.PosY; y++) {
        graphics.pixel(Point(p.PosX, y));
    }

    for (int y = p.PosY + 1 + 1; y < PicoUnicorn::HEIGHT; y++) {
        graphics.pixel(Point(p.PosX, y));
    }

    graphics.set_pen(pen);
    graphics.pixel(Point(p.PosX, p.PosY));
    graphics.pixel(Point(p.PosX, p.PosY + 1));
}

void update() {
    updateBall(ball, penBall);

    updatePaddle(paddleAB, penPaddleAB);
    updatePaddle(paddleXY, penPaddleXY);

    pico_unicorn.update(&graphics);
}

void checkInput(Paddle *p, uint8_t inputDec, uint8_t inputInc) {
    if (pico_unicorn.is_pressed(inputDec) && p->PosY > 0) {
        p->PosY = std::clamp(p->PosY - 1, 0, PicoUnicorn::HEIGHT - 1 - 1);
    }

    if (pico_unicorn.is_pressed(inputInc) && (p->PosY + 1 < PicoUnicorn::HEIGHT - 1)) {
        p->PosY = std::clamp(p->PosY + 1, 0, PicoUnicorn::HEIGHT - 1 - 1);
    }
}

bool isColliding(const Paddle& p, const Ball& b) {
    // NOTE: We check if the ball is on top of the paddle or adjacent to it
    //       *without* accounting for the ball's Y velocity. This is meant
    //       to be a little forgiving to the player.

    if (b.PosX == p.PosX) {
        if (b.PosY == p.PosY || b.PosY == (p.PosY + 1)) {
            return true;
        }
    } else if (b.PosX + b.VelocityX == p.PosX) {
        if (b.PosY == p.PosY || b.PosY == (p.PosY + 1)) {
            return true;
        }
    }

    return false;
}

int main() {
    stdio_init_all();

    init();

    while (true) {
        sleep_ms(1000 / FPS);

        if (isColliding(paddleAB, ball)) {
            ball.VelocityX *= -1;

            if (ball.PosX <= paddleAB.PosX && rand() % 3 == 0) {
                ball.VelocityY = std::clamp(ball.VelocityY - 1, -1, 1);
            } else {
                ball.VelocityY = std::clamp(ball.VelocityY + 1, -1, 1);
            }
        } else if (isColliding(paddleXY, ball)) {
            ball.VelocityX *= -1;

            if (ball.PosX <= paddleXY.PosX && rand() % 3 == 0) {
                ball.VelocityY = std::clamp(ball.VelocityY - 1, -1, 1);
            } else {
                ball.VelocityY = std::clamp(ball.VelocityY + 1, -1, 1);
            }
        }

        ball.PosX += ball.VelocityX;
        ball.PosY += ball.VelocityY;


        if (ball.PosX < 0 || ball.PosX > PicoUnicorn::WIDTH - 1) {
            ball.PosX = PicoUnicorn::WIDTH / 2;
            ball.PosY = PicoUnicorn::HEIGHT / 2;

            ball.VelocityX = (rand() % 2 == 0) ? 1 : -1;
            ball.VelocityY = 0;
        } else if (ball.PosY < 0) {
            ball.PosY = std::clamp((int)ball.PosY, 0, PicoUnicorn::HEIGHT - 1);

            ball.VelocityY *= -1;
        } else if (ball.PosY > PicoUnicorn::HEIGHT - 1) {
            ball.PosY = std::clamp((int)ball.PosY, 0, PicoUnicorn::HEIGHT - 1);

            ball.VelocityY *= -1;
        }

        checkInput(&paddleAB, PicoUnicorn::A, PicoUnicorn::B);
        checkInput(&paddleXY, PicoUnicorn::X, PicoUnicorn::Y);

        update();
    }
}
