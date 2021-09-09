#ifndef shapes_h
#define shapes_h

#include <SDL2/SDL.h>
#include "v2d.h"
#include "tween.h"

typedef struct LineSegment {
    v2d pt1, pt2;
} LineSegment;


typedef struct CollisionResult {
    SDL_bool isColliding;
    v2d collisionPt;
    v2d collisionNormal;
    uint32_t collisionObjectId;
    v2d r; // vec from object origin to collision pt
} CollisionResult;

typedef struct Circle {
    v2d midPoint;
    unsigned int radius;
    CollisionResult collisionResult;
} Circle;

typedef struct CollisionPolygon {
    v2d *modelPts;
    v2d *trfmPts;
    v2d *prevTrfmPts;
    float rotationRads;
    v2d pos;
    SDL_bool flip;
    Lerp lerp;
} CollisionPolygon;

#endif