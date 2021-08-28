#ifndef LERP_H
#define LERP_H

#include <SDL2/SDL.h>

typedef struct{
    double value;
    double step;
    double to;
    double remainingTime;
} Lerp;

typedef struct {
    float values[2];
    uint8_t current_value_idx;
    float blinkRate;
    float remainingTime;
} Blink;

Lerp lerpInit( double to, double from, double time ) {
    Lerp l;
    l.remainingTime = time;
    l.value = from;
    l.step = ( to - from ) / time;
    l.to = to;
    return l;
}

void lerpSet(Lerp *l, double to, double from, double time ) {
    l->remainingTime = time;
    l->value = from;
    l->step = ( to - from ) / time;
    l->to = to;

}

SDL_bool interpolate( Lerp *lerp, double deltaTime ) {
    lerp->remainingTime -= deltaTime;
    lerp->value += lerp->step * deltaTime;
    if( lerp->step > 0 ) { //positive 
        if(lerp->value > lerp->to) {
            lerp->value = lerp->to;
        }
    }
    else if( lerp->step < 0 ) { //negative
        if(lerp->value < lerp->to) {
            lerp->value = lerp->to;
        }
    }
    return (SDL_bool)( lerp->remainingTime <= 0 );
}

float blinkProcess( Blink *b, float deltaTime ) {
    b->remainingTime -= deltaTime;
    if( b->remainingTime <= 0.0f ) {
        b->current_value_idx = !b->current_value_idx;
        b->remainingTime = b->blinkRate;
    }
    return b->values[ b->current_value_idx ];
}

Blink blinkInit( float time, float value1, float value2 ) {
    Blink b;
    b.blinkRate = time;
    b.current_value_idx = 0;
    b.remainingTime = time;
    b.values[ 0 ] = value1;
    b.values[ 1 ] = value2;

    return b;
}

#endif