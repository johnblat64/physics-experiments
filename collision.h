#ifndef collision_h
#define collision_h
#include "v2d.h"
#include "physics.h"
#include "constants.h"

v2d impulseVector(float E, v2d v1, v2d v2, v2d n, float massKg1, float massKg2);
v2d impulseVectorAgainstStaticSurface(float E, v2d v1, v2d v2, v2d n, float massKg1);
v2d relativeVelocity(v2d va, v2d vb);
float impulseMagnitudeCalc(float e, v2d relVel_ab, v2d n, float massA_kg, float massB_kg);
float impulseMagnitudeCalcStatic(float e, v2d relVel_ab, v2d n, float massA_kg);
void velsAfterCollisionCalc(v2d v1a, v2d v1b, v2d *v2a, v2d *v2b, float j, float massA, float massB, v2d n);
void collisionResponseFlipper(Circle *circle, v2d *circleVelocity, float flipperAngularVel);

v2d impulseVector(float E, v2d v1, v2d v2, v2d n, float massKg1, float massKg2){
    float top = (E + 1) * ( v2d_dot(v2, n) - v2d_dot(v1, n) );
    float bottom = ( ( 1 / massKg1 ) + ( 1 / massKg2 ) );
    v2d p_hat = v2d_scale( ( top / bottom), n);
    return p_hat;

}

v2d impulseVectorAgainstStaticSurface(float E, v2d v1, v2d v2, v2d n, float massKg1){
    float top = (E + 1) * ( v2d_dot(v2, n) - v2d_dot(v1, n) );
    float bottom = ( ( 1 / massKg1 ) );
    v2d p_hat = v2d_scale( ( top / bottom), n);
    return p_hat;
}

v2d relativeVelocity(v2d va, v2d vb){
    return v2d_sub(va, vb);
}

/**
 * Two Decently sized objects
 * 
 * Neither is massive or static
 */
float impulseMagnitudeCalc(float e, v2d relVel_ab, v2d n, float massA_kg, float massB_kg){
    float j;
    float top = -(1 + e) * v2d_dot(relVel_ab,n);
    float bottom = v2d_dot(n, v2d_scale((1/massA_kg) - (1/ massB_kg), n));
    j = top/bottom;
    return j; 
}

/**
 * Mass A is infinite.
 * Earth/ground/something stuck in place
 */
float impulseMagnitudeCalcStatic(float e, v2d relVel_ab, v2d n, float massA_kg){
    float j;
    float top = -(1 + e) * v2d_dot(relVel_ab,n);
    float bottom = v2d_dot(n, v2d_scale((1/massA_kg), n));
    j = top/bottom;
    return j; 
}

void velsAfterCollisionCalc(v2d v1a, v2d v1b, v2d *v2a, v2d *v2b, float j, float massA, float massB, v2d n){
    *v2a = v2d_add(
        v1a,
        v2d_scale(j/massA, n)
    );

    *v2b = v2d_sub(
        v1b,
        v2d_scale(j/massB, n)
    );
}


void
collisionResponseFlipper(Circle *circle, v2d *circleVelocity, float flipperAngularVel ){
    //circleOnGround = SDL_TRUE;
    circle->midPoint.y = circle->collisionResult.collisionPt.y + v2d_scale(circle->radius, circle->collisionResult.collisionNormal).y;
    circle->midPoint.x = circle->collisionResult.collisionPt.x + v2d_scale(circle->radius, circle->collisionResult.collisionNormal).x;

    float E = 0.3f;
    v2d n = circle->collisionResult.collisionNormal;
    v2d flipperLinearVel = angularVelToLinearVel(flipperAngularVel, circle->collisionResult.r);
    //v2d surfaceVel = {0,0};
    //if(leftFlipperPressed){
    // surfaceVel = (v2d_f){50,-600};
    // }

    v2d relVelab = v2d_sub(*circleVelocity, flipperLinearVel);
    float j = impulseMagnitudeCalcStatic(E, relVelab, n, PINBALL_MASS_kg);
    velsAfterCollisionCalc(*circleVelocity, flipperLinearVel, circleVelocity, &flipperLinearVel, j, PINBALL_MASS_kg, 1000000, n );
}

#endif