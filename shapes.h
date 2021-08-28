#ifndef shapes_h
#define shapes_h

#include <SDL2/SDL.h>
#include "v2d.h"
#include "tween.h"

typedef struct LineSegment {
    v2d_f pt1, pt2;
} LineSegment;

typedef struct Circle {
    v2d_f midPoint;
    unsigned int radius;
} Circle;

typedef struct CollisionPolygon {
    v2d_f *modelPts;
    v2d_f *trfmPts;
    float rotationRads;
    v2d_f pos;
    SDL_bool flip;
    Lerp lerp;
} CollisionPolygon;

#endif