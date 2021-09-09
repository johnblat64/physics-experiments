#include <SDL2/SDL.h>
#include "v2d.h"
#include "util.h"
#include "darr.h"
#include "tween.h"
#include "collision.h"
#include "shapes.h"
#include "math.h"
#include "physics.h"
#include <limits.h>
#include "constants.h"

CollisionPolygon *cPolys[2];

int
sign(float x){
    return x >= 0 ? 1 : -1;
}

v2d lineVecNormal(LineSegment line){
    v2d lineVec = v2d_sub(line.pt2, line.pt1);
    v2d lineNorm = v2d_unit(lineVec);
    return lineNorm;
}

double
getAngleOfLine(LineSegment line){
    v2d upVec = {0,-1};
    v2d lineVec = v2d_sub(line.pt2, line.pt1);
    v2d lineNorm = v2d_unit(lineVec);
    double theta = acos(v2d_dot(lineNorm, upVec)/(v2d_magnitude(lineNorm)*v2d_magnitude(upVec)));
    return theta;
}

void 
flipModel(CollisionPolygon *cPoly) {
    for(int i = 0; i < lenDarr(cPoly->trfmPts); i++){
        cPoly->trfmPts[i] = v2d_scale(-1, cPoly->trfmPts[i]); 
    }
}

void 
polyDeepCopy(CollisionPolygon *src, CollisionPolygon *dest){
    dest->modelPts = initDarr(dest->modelPts, sizeof(v2d), capDarr(src->modelPts));
    dest->trfmPts = initDarr(dest->trfmPts, sizeof(v2d), capDarr(src->trfmPts));

    for(int i = 0; i < lenDarr(src->modelPts); i++){
        addDarr(dest->modelPts, src->modelPts[i], v2d);
        addDarr(dest->trfmPts, src->trfmPts[i], v2d);
    }

    dest->rotationRads = src->rotationRads;
    dest->pos = src->pos;
    dest->flip = src->flip;
    dest->lerp.step = src->lerp.step;
    dest->lerp.remainingTime = src->lerp.remainingTime;
    dest->lerp.value = src->lerp.value;

}

SDL_bool 
intersects_PointAndCircle(v2d pt, Circle circle){
    v2d ptToCirclev2d_f = v2d_sub(pt, circle.midPoint);
    float magPtToCircleCenter = v2d_magnitude_noroot(ptToCirclev2d_f);
    if(magPtToCircleCenter <= circle.radius*circle.radius){
        return SDL_TRUE;
    }
    return SDL_FALSE;
}

SDL_bool 
intersects_PointAndLine(v2d pt, LineSegment line){
    float magPtToLinePt1 = v2d_magnitude(v2d_sub(pt, line.pt1));
    float magPtToLinePt2 = v2d_magnitude(v2d_sub(pt, line.pt2));
    float magLine = v2d_magnitude(v2d_sub(line.pt1, line.pt2));
    float buffer = 0.1;
    if(magPtToLinePt1+magPtToLinePt2 >= magLine-buffer && magPtToLinePt1+magPtToLinePt2 <= magLine+buffer){
        return SDL_TRUE;
    }
    return SDL_FALSE;
}

SDL_bool 
intersects_LineAndCircle(LineSegment line, Circle circle){
    if(intersects_PointAndCircle(line.pt1, circle)){
        return SDL_TRUE;
    }
    else if(intersects_PointAndCircle(line.pt2, circle)) {
        return SDL_TRUE;
    }

    v2d linePt1ToCircle = v2d_sub(circle.midPoint, line.pt1);
    v2d linev2d_f = v2d_sub(line.pt2, line.pt1);
    float magLinev2d_f = v2d_magnitude(linev2d_f);
    v2d nomralizedLinev2d_f = v2d_unit(linev2d_f);


    float dot = v2d_dot(linePt1ToCircle, nomralizedLinev2d_f);

    v2d ptOnVector = v2d_scale(dot, nomralizedLinev2d_f);
    v2d ptOnLine = v2d_add(ptOnVector, line.pt1);

    if(!intersects_PointAndLine(ptOnLine, line)){
        return SDL_FALSE;
    }

    float x = circle.midPoint.x - ptOnLine.x;
    float y = circle.midPoint.y - ptOnLine.y;
    if((x*x + y*y )<= (circle.radius*circle.radius)){
        return SDL_TRUE;
    }
    return SDL_FALSE;

}

void 
intersects_LineAndCircle_Result(LineSegment line, Circle *circle){
    // if(intersects_PointAndCircle(line.pt1, *circle)){
    //     circle->collisionResult = (CollisionResult){
    //         SDL_TRUE,
    //         (v2d_f){0,0},
    //         (v2d_f){0,0}
    //     };
    //     return;
    // }
    // else if(intersects_PointAndCircle(line.pt2, *circle)) {
    //     circle->collisionResult = (CollisionResult){
    //         SDL_TRUE,
    //         (v2d_f){0,0},
    //         (v2d_f){0,0}
    //     };
    //     return;
    // }

    v2d linePt1ToCircle = v2d_sub(circle->midPoint, line.pt1);
    v2d linev2d_f = v2d_sub(line.pt2, line.pt1);
    float magLinev2d_f = v2d_magnitude(linev2d_f);
    v2d nomralizedLinev2d_f = v2d_unit(linev2d_f);


    float dot = v2d_dot(linePt1ToCircle, nomralizedLinev2d_f);

    v2d ptOnVector = v2d_scale(dot, nomralizedLinev2d_f);
    v2d ptOnLine = v2d_add(ptOnVector, line.pt1);

    if(!intersects_PointAndLine(ptOnLine, line)){
        circle->collisionResults[circle->numCollisions] = (CollisionResult){
            SDL_FALSE,
            (v2d){0,0},
            (v2d){0,0},
        };
        circle->numCollisions++;
        return;
    }

    float x = circle->midPoint.x - ptOnLine.x;
    float y = circle->midPoint.y - ptOnLine.y;
    v2d collisionPt = ptOnLine;
    v2d collisionPtToCircleMidPoint = {x,y};
    v2d collisionNormal = v2d_unit(collisionPtToCircleMidPoint);
    if((x*x + y*y )<= (circle->radius*circle->radius)){
        circle->collisionResults[circle->numCollisions] =(CollisionResult){
            SDL_TRUE,
            collisionPt,
            collisionNormal
        };
        circle->numCollisions++;
        return;
    }
    circle->collisionResults[circle->numCollisions] = (CollisionResult){
        SDL_FALSE,
        (v2d){0,0},
        (v2d){0,0}
    };

}

void 
intersects_PolygonAndCircle(CollisionPolygon *cPoly, Circle *circle){
    unsigned int numVertices = lenDarr(cPoly->trfmPts);
    for(int i = 0;i < numVertices; i++){
        v2d pt1, pt2;
        pt1 = cPoly->trfmPts[i];
        pt2 = cPoly->trfmPts[(i+1)%numVertices];
        LineSegment polygonLineSegment = {pt1, pt2};

        intersects_LineAndCircle_Result(polygonLineSegment, circle);
    
        if(circle->collisionResult.isColliding){
            circle->collisionResult.r = v2d_sub(circle->collisionResult.collisionPt, cPoly->pos);
            return;
        }
            
    }
    
}

void 
positionTransformPoly(CollisionPolygon *cPoly){
    for(int i = 0; i < lenDarr(cPoly->trfmPts); i++){
        cPoly->trfmPts[i] = v2d_add(cPoly->modelPts[i], cPoly->pos);
    }
}

void 
rotateTransformPoly(CollisionPolygon *cPoly){
    float s = sin(cPoly->rotationRads);
    float c = cos(cPoly->rotationRads);
    for(int i = 0; i < 7; i++){
        v2d pivotPt = cPoly->pos;
        v2d subtracted = v2d_sub(cPoly->trfmPts[i], pivotPt);
        v2d p;
        p.x = subtracted.x;
        p.y = subtracted.y;

        float xnew = p.x * c - p.y * s;
        float ynew = p.x * s + p.y * c;

        p.x = xnew + pivotPt.x;
        p.y = ynew + pivotPt.y;

        cPoly->trfmPts[i].x = p.x;
        cPoly->trfmPts[i].y = p.y;
        
    }
}

void 
flipTransformPoly(CollisionPolygon *cPoly){
    if(cPoly->flip){
        for(int i = 0; i < lenDarr(cPoly->trfmPts);i++){
            cPoly->trfmPts[i].x = cPoly->pos.x + (cPoly->pos.x - cPoly->trfmPts[i].x);
        }
    }
}


v2d
addGravityToVelocity(v2d velocity){
    return (v2d){velocity.x, velocity.y+=PINBALL_MASS_g*9.81};
}

void
moveThing(v2d velocity, v2d *position, float deltaTime){
    position->x += velocity.x*deltaTime;
    position->y += velocity.y*deltaTime;
}



int
main() {
    SDL_Window *window;
    SDL_Renderer *renderer;

    SDL_Init(SDL_INIT_VIDEO);
    window = SDL_CreateWindow("Polygon", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 640,1000, 0);

    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED |SDL_RENDERER_PRESENTVSYNC );

    LineSegment bottomLine = {
        (v2d){0,800},
        (v2d){640,800}
    };

    LineSegment leftWallLine = {
        (v2d){25,0},
        (v2d){25,1000}
    };

    LineSegment rightWallLine = {
        (v2d){640-25,0},
        (v2d){640-25,1000}
    };

    LineSegment topLine = {
        (v2d){0,10},
        (v2d){640,10}
    };

    CollisionPolygon cPoly_1;
    cPoly_1.modelPts = initDarr(cPoly_1.modelPts, sizeof(v2d), 7);
    cPoly_1.trfmPts = initDarr(cPoly_1.trfmPts, sizeof(v2d), 7);
    addDarr(cPoly_1.modelPts, ((v2d){10,-20}), v2d);
    addDarr(cPoly_1.modelPts, ((v2d){-10,-20}), v2d);
    addDarr(cPoly_1.modelPts, ((v2d){-20,-5}), v2d);
    addDarr(cPoly_1.modelPts, ((v2d){-12,25}), v2d);
    addDarr(cPoly_1.modelPts, ((v2d){100,80}), v2d);
    addDarr(cPoly_1.modelPts, ((v2d){110,75}), v2d);
    addDarr(cPoly_1.modelPts, ((v2d){110,68}), v2d);
    for(int i = 0; i < lenDarr(cPoly_1.modelPts); i++){
        addDarr(cPoly_1.trfmPts, ((v2d){0,0}), v2d);
    }
    cPoly_1.rotationRads = 0.0f;
    cPoly_1.pos = (v2d){-1,-1};//{150,400};
    cPoly_1.flip = SDL_FALSE;
    cPoly_1.lerp = lerpInit(0,0,0);

    float cPoly1AngularVel = 0.0f;

    CollisionPolygon cPoly_2;
    polyDeepCopy(&cPoly_1, &cPoly_2);
    cPoly_2.pos = (v2d){-1,-1};//{520,400};
    cPoly_2.flip = SDL_TRUE;

    Circle circle_1;
    circle_1.radius = 20;
    circle_1.midPoint = (v2d){200,150};

    float cPoly2AngularVel = 0.0f;


    float pixelToMeter = circle_1.radius*2 / PINBALL_DIAMETER_m;
    


    v2d circleVelocity = {0.0f,0.0f};
    if( PINBALL_DECLINE_GRAVITY == 0){
        circleVelocity = (v2d){100.0f, 500.0f};
    }
    SDL_bool circleOnGround = SDL_FALSE;
    float circleSpeed = 0.0f;
    // float circleAcceleration = 0.0f;
    // circleVelocity = addGravityToVelocity(circleVelocity);

    SDL_bool quit = SDL_FALSE;
    SDL_Event event;
    SDL_bool leftFlipperJustPressed = SDL_FALSE;
    SDL_bool leftFlipperPressed = SDL_FALSE;
    SDL_bool rightFlipperJustPressed = SDL_FALSE;
    SDL_bool rightFlipperPressed = SDL_FALSE;
    SDL_bool lmbPressed = SDL_FALSE;

    float max_delta_time = 1 / 60.0;
    float previous_frame_ticks = SDL_GetTicks() / 1000.0;

    while(!quit) {
        float current_frame_ticks = SDL_GetTicks() / 1000.0;
        float deltaTime = current_frame_ticks - previous_frame_ticks;
        previous_frame_ticks = current_frame_ticks;
        // adjust for any pauses, debugging breaks, etc
        deltaTime = deltaTime < max_delta_time ?  deltaTime : max_delta_time;

        leftFlipperJustPressed = SDL_FALSE;
        rightFlipperJustPressed = SDL_FALSE;

        circle_1.numCollisions = 0;

        while(SDL_PollEvent(&event)) {
            if(event.type == SDL_QUIT) {
                quit = SDL_TRUE;
            }
            if(event.type == SDL_KEYUP){
                double time;
                switch(event.key.keysym.sym){
                    case SDLK_r:
                        circle_1.midPoint = (v2d){100,100};
                        break;

                    case SDLK_LEFT:
                        cPoly_1.pos.x -= 30;
                        break;
                    case SDLK_RIGHT:
                        cPoly_1.pos.x += 30;
                        break;
                    case SDLK_UP:
                        cPoly_1.pos.y -= 30;
                        break;
                    case SDLK_DOWN:
                        cPoly_1.pos.y += 30;
                        break;
                    case SDLK_q:
                        cPoly_1.rotationRads += .2;
                        break;
                    case SDLK_e:
                        cPoly_1.rotationRads -= .2;
                        break;
                    case SDLK_f:
                        cPoly_1.flip = !cPoly_1.flip;
                    case SDLK_RSHIFT:
                        rightFlipperPressed = SDL_FALSE;
                        time = FLIPPER_TOTAL_TIME - cPoly_1.lerp.remainingTime;
                        lerpSet(&cPoly_2.lerp,0,cPoly_2.rotationRads, time );
                        //cPoly_1.rotationRads = 0;
                        break;
                    case SDLK_LSHIFT:
                        leftFlipperPressed = SDL_FALSE;
                        

                        time = FLIPPER_TOTAL_TIME - cPoly_1.lerp.remainingTime;
                        lerpSet(&cPoly_1.lerp,0,cPoly_1.rotationRads, time );
                        //cPoly_2.rotationRads = 0;
                        break;
                    default:
                        break;
                    
                }
                
            } 
            else if(event.type == SDL_KEYDOWN){
                switch(event.key.keysym.sym){
                    case SDLK_RSHIFT:
                        if(!rightFlipperPressed)
                            rightFlipperJustPressed = SDL_TRUE;
                        rightFlipperPressed = SDL_TRUE;
                        //cPoly_1.rotationRads = -1;
                        break;
                    case SDLK_LSHIFT:
                        if(!leftFlipperPressed)
                            leftFlipperJustPressed = SDL_TRUE;
                        leftFlipperPressed = SDL_TRUE;
                        //lerpSet(&cPoly_2.lerp, -1, cPoly_2.rotationRads, 500 );
                        break;
                }
            }
        }

        // flippers presed. Set the lerp
        // if(leftFlipperJustPressed) {
        //     double time = FLIPPER_TOTAL_TIME - cPoly_1.lerp.remainingTime;
        //     lerpSet(&cPoly_1.lerp, -1, cPoly_1.rotationRads, time );
        // }
        // else if(rightFlipperJustPressed){
        //     double time = FLIPPER_TOTAL_TIME - cPoly_1.lerp.remainingTime;
        //     lerpSet(&cPoly_2.lerp, -1, cPoly_2.rotationRads, time );

        // }

        if(leftFlipperPressed){
            cPoly1AngularVel = cPoly_1.rotationRads <= -FLIPPER_MAX_ANGLE ? 0 : -FLIPPER_ANGULAR_VELOCITY;
            //cPoly_1.rotationRads -= cPoly1AngularVel * deltaTime;
        }

        // if( cPoly_1.rotationRads <= -FLIPPER_MAX_ANGLE) {
        //     cPoly1AngularVel = 0;
        //     cPoly_1.rotationRads = -FLIPPER_MAX_ANGLE;
        // }

        if(!leftFlipperPressed){
            cPoly1AngularVel =  cPoly_1.rotationRads >= FLIPPER_MIN_ANGLE ? 0 : FLIPPER_ANGULAR_VELOCITY;
            //cPoly_1.rotationRads += cPoly1AngularVel *deltaTime;
        }

        // if(cPoly_1.rotationRads > FLIPPER_MIN_ANGLE){
        //     cPoly1AngularVel = 0;
        //     cPoly_1.rotationRads = FLIPPER_MIN_ANGLE;
        // }
        cPoly_1.rotationRads += cPoly1AngularVel *deltaTime;

        // process the lerp
        // if(cPoly_1.lerp.remainingTime > 0.0f){
        //     interpolate(&cPoly_1.lerp, deltaTime);
        //     cPoly_1.rotationRads = cPoly_1.lerp.value;
        // }

        // if(cPoly_2.lerp.remainingTime > 0.0f){
        //     interpolate(&cPoly_2.lerp, deltaTime);
        //     cPoly_2.rotationRads = cPoly_2.lerp.value;
        // }
        
        

        // position
        positionTransformPoly(&cPoly_1);
        positionTransformPoly(&cPoly_2);

        // rotation
        rotateTransformPoly(&cPoly_1);
        rotateTransformPoly(&cPoly_2);
        
        // flip
        flipTransformPoly(&cPoly_1);
        flipTransformPoly(&cPoly_2);

        //move
        //v2d_f force = (v2d_f){0, PINBALL_MASS_kg*9.81};
        //v2d_f acceleration = (v2d_f){force.x / PINBALL_MASS_kg, force.y/PINBALL_MASS_kg};
        //circleVelocity.x+=acceleration.x*deltaTime;

        float E = 0.3f;
        
        intersects_LineAndCircle_Result(bottomLine, &circle_1);
        if(circle_1.collisionResult.isColliding){
            circle_1.midPoint.y = circle_1.collisionResult.collisionPt.y + v2d_scale(circle_1.radius, circle_1.collisionResult.collisionNormal).y;
            circle_1.midPoint.x = circle_1.collisionResult.collisionPt.x + v2d_scale(circle_1.radius, circle_1.collisionResult.collisionNormal).x;
 
            
            v2d n = circle_1.collisionResult.collisionNormal;
            v2d surfaceVel = {0,0};
            //if(leftFlipperPressed){
            // surfaceVel = (v2d_f){50,-600};
           // }

            v2d relVelab = v2d_sub(circleVelocity, surfaceVel);
            float j = impulseMagnitudeCalcStatic(E, relVelab, n, PINBALL_MASS_kg);
            velsAfterCollisionCalc(circleVelocity, surfaceVel, &circleVelocity, &surfaceVel, j, PINBALL_MASS_kg, 1000000, n );


            moveThing(circleVelocity, &circle_1.midPoint, deltaTime);
        }

        if(!circle_1.collisionResult.isColliding){
            intersects_PolygonAndCircle(&cPoly_1, &circle_1);
            if(circle_1.collisionResult.isColliding){
                collisionResponseFlipper(&circle_1, &circleVelocity, cPoly1AngularVel);
            }
        }
            
        if(!circle_1.collisionResult.isColliding){
            intersects_PolygonAndCircle(&cPoly_2, &circle_1);
            if(circle_1.collisionResult.isColliding){
                collisionResponseFlipper(&circle_1, &circleVelocity, cPoly1AngularVel);
            }
        }
        if(!circle_1.collisionResult.isColliding){
            intersects_LineAndCircle_Result(leftWallLine, &circle_1);
            if(circle_1.collisionResult.isColliding){
                collisionResponseFlipper(&circle_1, &circleVelocity, FLIPPER_ANGULAR_VELOCITY);
            }
        }
        
        if(!circle_1.collisionResult.isColliding){
            intersects_LineAndCircle_Result(rightWallLine, &circle_1);
            if(circle_1.collisionResult.isColliding){
                collisionResponseFlipper(&circle_1, &circleVelocity, FLIPPER_ANGULAR_VELOCITY);
            }
        }

        if(!circle_1.collisionResult.isColliding){
            intersects_LineAndCircle_Result(topLine, &circle_1);
            if(circle_1.collisionResult.isColliding){
                collisionResponseFlipper(&circle_1, &circleVelocity, FLIPPER_ANGULAR_VELOCITY);
            }
        }



        // if(circle_1.collisionResult.isColliding){
        //     circleOnGround = SDL_TRUE;
        //     circle_1.midPoint.y = circle_1.collisionResult.collisionPt.y + v2d_scale(circle_1.radius, circle_1.collisionResult.collisionNormal).y;
        //     circle_1.midPoint.x = circle_1.collisionResult.collisionPt.x + v2d_scale(circle_1.radius, circle_1.collisionResult.collisionNormal).x;
 
        //     float E = 0.6f;
        //     v2d n = circle_1.collisionResult.collisionNormal;
        //     v2d surfaceVel = {0,0};
        //     //if(leftFlipperPressed){
        //     // surfaceVel = (v2d_f){50,-600};
        //    // }

        //     v2d relVelab = v2d_sub(circleVelocity, surfaceVel);
        //     float j = impulseMagnitudeCalcStatic(E, relVelab, n, PINBALL_MASS_kg);
        //     velsAfterCollisionCalc(circleVelocity, surfaceVel, &circleVelocity, &surfaceVel, j, PINBALL_MASS_kg, 1000000, n );


        //     moveThing(circleVelocity, &circle_1.midPoint, deltaTime);

        // }
        //else{
            //circleOnGround = SDL_FALSE;
            //circleVelocity.y
            circleVelocity.y+=(pixelToMeter*PINBALL_DECLINE_GRAVITY)*deltaTime;
            
            
            moveThing(circleVelocity, &circle_1.midPoint, deltaTime);
        //}

        // if(circleOnGround){
        //     circle
        // }
        
        // render
        SDL_SetRenderDrawColor(renderer, 20,20,20,255);
        SDL_RenderClear(renderer);
        
        drawGrid(renderer, 0,0,100,100,10, (SDL_Color){50,50,50,255} );
        
        SDL_SetRenderDrawColor(renderer, 250,250,0,255);
        SDL_RenderDrawLine(renderer, bottomLine.pt1.x, bottomLine.pt1.y, bottomLine.pt2.x, bottomLine.pt2.y);
        SDL_RenderDrawLine(renderer, leftWallLine.pt1.x, leftWallLine.pt1.y, leftWallLine.pt2.x, leftWallLine.pt2.y);
        SDL_RenderDrawLine(renderer, rightWallLine.pt1.x, rightWallLine.pt1.y, rightWallLine.pt2.x, rightWallLine.pt2.y);
        SDL_RenderDrawLine(renderer, topLine.pt1.x, topLine.pt1.y, topLine.pt2.x, topLine.pt2.y);
        



        if(circle_1.collisionResult.isColliding ) {
            SDL_SetRenderDrawColor(renderer, 0,255,100,255);
        }
        else {
            SDL_SetRenderDrawColor(renderer, 200,0,0,255);
        }

        


        drawCircle(renderer, circle_1);

        SDL_SetRenderDrawColor(renderer, 255,150,0,255);
        drawPolygon(renderer, &cPoly_1);
        SDL_SetRenderDrawColor(renderer, 0,255,150,255);
        drawPolygon(renderer, &cPoly_2);

        SDL_SetRenderDrawColor(renderer, 100,100,100,255);
        drawCross(renderer, cPoly_1.pos.x, cPoly_1.pos.y, 7 );
        SDL_RenderPresent(renderer);

    }

    exit(0);
}