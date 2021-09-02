#ifndef collision_h
#define collision_h
#include "v2d.h"

v2d_f impulseVector(float E, v2d_f v1, v2d_f v2, v2d_f n, float massKg1, float massKg2){
    float top = (E + 1) * ( dot_product_v2d_f(v2, n) - dot_product_v2d_f(v1, n) );
    float bottom = ( ( 1 / massKg1 ) + ( 1 / massKg2 ) );
    v2d_f p_hat = v2d_f_scalar_mult( ( top / bottom), n);
    return p_hat;

}

v2d_f impulseVectorAgainstStaticSurface(float E, v2d_f v1, v2d_f v2, v2d_f n, float massKg1){
    float top = (E + 1) * ( dot_product_v2d_f(v2, n) - dot_product_v2d_f(v1, n) );
    float bottom = ( ( 1 / massKg1 ) );
    v2d_f p_hat = v2d_f_scalar_mult( ( top / bottom), n);
    return p_hat;
}

#endif