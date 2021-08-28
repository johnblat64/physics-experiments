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
inline v2d v2d_sub(v2d a, v2d b);
inline v2d_f v2d_f_add(v2d_f a, v2d_f b);
inline v2d_f v2d_f_sub(v2d_f a, v2d_f b);
inline v2d_f v2d_f_scalar_mult(float val, v2d_f a);
inline float dot_product_v2d_f( v2d_f a, v2d_f b );
inline float mag_v2d_f( v2d_f v );
inline float mag_no_root_v2d_f( v2d_f v );
inline v2d_f v2d_f_unit( v2d_f v );

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

v2d_f v2d_f_scalar_mult(float val, v2d_f a){
    return (v2d_f){a.x*val, a.y*val};
}

float dot_product_v2d_f( v2d_f a, v2d_f b ){
    return (float)((a.x*b.x)+(a.y*b.y));
}

float mag_v2d_f( v2d_f v ) {
    return sqrtf(dot_product_v2d_f(v, v));
}

float mag_no_root_v2d_f( v2d_f v ){
    return dot_product_v2d_f(v, v);

}

v2d_f v2d_f_unit( v2d_f v ) {
    v2d_f unit_vector;
    float magnitude = mag_v2d_f( v );

    unit_vector.x = (float)v.x / magnitude;
    unit_vector.y = (float)v.y / magnitude;


    return unit_vector;
}

#endif