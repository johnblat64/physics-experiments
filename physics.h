#ifndef physics_h
#define physics_h

#include "v2d.h"

v2d angularVelToLinearVel(float w, v2d r){
    v2d perpR = v2d_perp(r);
    v2d v = v2d_scale(-w, perpR);
    return v;
}

#endif