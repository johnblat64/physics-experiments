#ifndef v2d_h
#define v2d_h

#include <math.h>

typedef struct v2d {
    int x, y;
}v2d;

typedef struct v2d_f {
    float x, y;
} v2d_f;

inline v2d v2d_add(v2d a, v2d b);

v2d v2d_add(v2d a, v2d b){
    return (v2d){a.x+b.x, a.y+b.y};
}

v2d v2d_sub(v2d a, v2d b){
    return (v2d){a.x-b.x, a.y-b.y};
}

v2d_f v2d_f_add(v2d_f a, v2d_f b){
    return (v2d_f){a.x+b.x, a.y+b.y};
}

v2d_f v2d_f_sub(v2d_f a, v2d_f b){
    return (v2d_f){a.x-b.x, a.y-b.y};
}

v2d v2d_scalar_mult(float val, v2d a) {
    return (v2d){a.x*val, a.y*val};
}

#endif