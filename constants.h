#ifndef constants_h
#define constants_h

#include <SDL2/SDL.h>

#define FLIPPER_TOTAL_TIME  0.033f
#define PIN_TABLE_ANGLE_RADS  0.113446401379f // 6.5 degrees
#define PINBALL_DECLINE_GRAVITY  1.10939149493f // 9.8(sin(6.5 degrees))
#define PINBALL_MASS_g  80.0f //grams
#define PINBALL_MASS_kg  0.08f //kilo grams
#define PINBALL_DIAMETER_m  0.027f // meters
#define SURFACE_MASS  SDL_MAX_UINT32
#define FLIPPER_ANGULAR_VELOCITY  60.0f//70.33f //revs per second
#define FLIPPER_MAX_ANGLE  1.0f
#define FLIPPER_MIN_ANGLE  0.0f

#endif