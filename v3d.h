#ifndef VEC_H
#define VEC_H
#include <math.h>




typedef struct {
    float x, y, z;
} Vec3_f;

typedef struct {
    float x, y;
} Vec2_f;

Vec3_f unit_vec3_f( Vec3_f v ) ;
Vec3_f add_vec3_f( Vec3_f a, Vec3_f b );
Vec3_f sub_vec3_f( Vec3_f a, Vec3_f b ) ;
Vec3_f scalar_mult_vec3_f( Vec3_f a, float b) ;
Vec3_f point_on_ray( Vec3_f origin, Vec3_f viewport_point, float t) ;
int vec_3_f_equal( Vec3_f a, Vec3_f b );
float dot_product_vec3_f( Vec3_f a, Vec3_f b );
float mag_vec3_f( Vec3_f v ) ;
float mag_no_root_vec_3_f( Vec3_f v );

inline Vec3_f unit_vec3_f( Vec3_f v ) {
    Vec3_f unit_vector;
    float magnitude = mag_vec3_f( v );

    unit_vector.x = v.x / magnitude;
    unit_vector.y = v.y / magnitude;
    unit_vector.z = v.z / magnitude;

    float mag_unit_vector = mag_vec3_f(unit_vector);


    return unit_vector;
}

inline Vec3_f add_vec3_f( Vec3_f a, Vec3_f b ) {
    Vec3_f result = {
        a.x + b.x,
        a.y + b.y,
        a.z + b.z
    };
    return result;
}

inline Vec3_f sub_vec3_f( Vec3_f a, Vec3_f b ) {
    Vec3_f result = {
        a.x - b.x,
        a.y - b.y,
        a.z - b.z
    };
    return result;
}

inline Vec3_f scalar_mult_vec3_f( Vec3_f a, float b) {
    Vec3_f result = {
        a.x * b,
        a.y * b,
        a.z * b
    };
    return result;
}

inline Vec3_f point_on_ray( Vec3_f origin, Vec3_f viewport_point, float t) {
    Vec3_f result, dir;
    dir = sub_vec3_f( viewport_point, origin );
    result = add_vec3_f( origin, scalar_mult_vec3_f( dir, t ) );
    return result;

}

inline int vec_3_f_equal( Vec3_f a, Vec3_f b ) {
    int result = a.x == b.x && a.y == b.y && a.z == b.z;
    return  result;

}

inline float dot_product_vec3_f( Vec3_f a, Vec3_f b ) {
    float  result = 0.0f;
    result += a.x * b.x;
    result += a.y * b.y;
    result += a.z * b.z;
    return result;
}

inline float mag_vec3_f( Vec3_f v ) {
    float result;
    result = sqrtf( dot_product_vec3_f(v, v) );
    return result;
}

inline float mag_no_root_vec_3_f( Vec3_f v ) {
    float result;
    result = dot_product_vec3_f(v, v);
    return result;
}

#endif