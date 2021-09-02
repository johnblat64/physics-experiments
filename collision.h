#ifndef collision_h
#define collision_h
#include "v2d.h"

v2d_f impulseVector(float E, v2d_f v1, v2d_f v2, v2d_f n, float massKg1, float massKg2);
v2d_f impulseVectorAgainstStaticSurface(float E, v2d_f v1, v2d_f v2, v2d_f n, float massKg1);
v2d_f relativeVelocity(v2d_f va, v2d_f vb);
float impulseMagnitudeCalc(float e, v2d_f relVel_ab, v2d_f n, float massA_kg, float massB_kg);
float impulseMagnitudeCalcStatic(float e, v2d_f relVel_ab, v2d_f n, float massA_kg);
void velsAfterCollisionCalc(v2d_f v1a, v2d_f v1b, v2d_f *v2a, v2d_f *v2b, float j, float massA, float massB, v2d_f n);

v2d_f impulseVector(float E, v2d_f v1, v2d_f v2, v2d_f n, float massKg1, float massKg2){
    float top = (E + 1) * ( v2d_f_dot(v2, n) - v2d_f_dot(v1, n) );
    float bottom = ( ( 1 / massKg1 ) + ( 1 / massKg2 ) );
    v2d_f p_hat = v2d_f_scale( ( top / bottom), n);
    return p_hat;

}

v2d_f impulseVectorAgainstStaticSurface(float E, v2d_f v1, v2d_f v2, v2d_f n, float massKg1){
    float top = (E + 1) * ( v2d_f_dot(v2, n) - v2d_f_dot(v1, n) );
    float bottom = ( ( 1 / massKg1 ) );
    v2d_f p_hat = v2d_f_scale( ( top / bottom), n);
    return p_hat;
}

v2d_f relativeVelocity(v2d_f va, v2d_f vb){
    return v2d_f_sub(va, vb);
}

/**
 * Two Decently sized objects
 * 
 * Neither is massive or static
 */
float impulseMagnitudeCalc(float e, v2d_f relVel_ab, v2d_f n, float massA_kg, float massB_kg){
    float j;
    float top = -(1 + e) * v2d_f_dot(relVel_ab,n);
    float bottom = v2d_f_dot(n, v2d_f_scale((1/massA_kg) - (1/ massB_kg), n));
    j = top/bottom;
    return j; 
}

/**
 * Mass A is infinite.
 * Earth/ground/something stuck in place
 */
float impulseMagnitudeCalcStatic(float e, v2d_f relVel_ab, v2d_f n, float massA_kg){
    float j;
    float top = -(1 + e) * v2d_f_dot(relVel_ab,n);
    float bottom = v2d_f_dot(n, v2d_f_scale((1/massA_kg), n));
    j = top/bottom;
    return j; 
}

void velsAfterCollisionCalc(v2d_f v1a, v2d_f v1b, v2d_f *v2a, v2d_f *v2b, float j, float massA, float massB, v2d_f n){
    *v2a = v2d_f_add(
        v1a,
        v2d_f_scale(j/massA, n)
    );

    *v2b = v2d_f_sub(
        v1b,
        v2d_f_scale(j/massB, n)
    );
}

#endif