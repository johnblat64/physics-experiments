#include <SDL2/SDL.h>
#include "v2d.h"
#include "util.h"
#include "darr.h"
#include "tween.h"
#include "shapes.h"
#include "math.h"

const double FLIPPER_TOTAL_TIME = 0.5f;
const double PIN_TABLE_ANGLE_RADS = 0.113446401379f; // 6.5 degrees
const double PINBALL_DECLINE_GRAVITY = 1.10939149493f; // 9.8(sin(6.5 degrees))
const double PINBALL_MASS_g = 80.0f; //grams
const double PINBALL_MASS_kg = 0.08f; //kilo grams
const double PINBALL_DIAMETER_m = 0.027f; // meters


v2d_f lineVecNormal(LineSegment line){
    v2d_f lineVec = v2d_f_sub(line.pt2, line.pt1);
    v2d_f lineNorm = v2d_f_unit(lineVec);
    return lineNorm;
}

double
getAngleOfLine(LineSegment line){
    v2d_f upVec = {0,-1};
    v2d_f lineVec = v2d_f_sub(line.pt2, line.pt1);
    v2d_f lineNorm = v2d_f_unit(lineVec);
    double theta = acos(dot_product_v2d_f(lineNorm, upVec)/(mag_v2d_f(lineNorm)*mag_v2d_f(upVec)));
    return theta;
}

void 
flipModel(CollisionPolygon *cPoly) {
    for(int i = 0; i < lenDarr(cPoly->trfmPts); i++){
        cPoly->trfmPts[i] = v2d_f_scalar_mult(-1, cPoly->trfmPts[i]); 
    }
}

void 
polyDeepCopy(CollisionPolygon *src, CollisionPolygon *dest){
    dest->modelPts = initDarr(dest->modelPts, sizeof(v2d_f), capDarr(src->modelPts));
    dest->trfmPts = initDarr(dest->trfmPts, sizeof(v2d_f), capDarr(src->trfmPts));

    for(int i = 0; i < lenDarr(src->modelPts); i++){
        addDarr(dest->modelPts, src->modelPts[i], v2d_f);
        addDarr(dest->trfmPts, src->trfmPts[i], v2d_f);
    }

    dest->rotationRads = src->rotationRads;
    dest->pos = src->pos;
    dest->flip = src->flip;
    dest->lerp.step = src->lerp.step;
    dest->lerp.remainingTime = src->lerp.remainingTime;
    dest->lerp.value = src->lerp.value;

}

SDL_bool 
intersects_PointAndCircle(v2d_f pt, Circle circle){
    v2d_f ptToCirclev2d_f = v2d_f_sub(pt, circle.midPoint);
    float magPtToCircleCenter = mag_no_root_v2d_f(ptToCirclev2d_f);
    if(magPtToCircleCenter <= circle.radius*circle.radius){
        return SDL_TRUE;
    }
    return SDL_FALSE;
}

SDL_bool 
intersects_PointAndLine(v2d_f pt, LineSegment line){
    float magPtToLinePt1 = mag_v2d_f(v2d_f_sub(pt, line.pt1));
    float magPtToLinePt2 = mag_v2d_f(v2d_f_sub(pt, line.pt2));
    float magLine = mag_v2d_f(v2d_f_sub(line.pt1, line.pt2));
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

    v2d_f linePt1ToCircle = v2d_f_sub(circle.midPoint, line.pt1);
    v2d_f linev2d_f = v2d_f_sub(line.pt2, line.pt1);
    float magLinev2d_f = mag_v2d_f(linev2d_f);
    v2d_f nomralizedLinev2d_f = v2d_f_unit(linev2d_f);


    float dot = dot_product_v2d_f(linePt1ToCircle, nomralizedLinev2d_f);

    v2d_f ptOnVector = v2d_f_scalar_mult(dot, nomralizedLinev2d_f);
    v2d_f ptOnLine = v2d_f_add(ptOnVector, line.pt1);

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

    v2d_f linePt1ToCircle = v2d_f_sub(circle->midPoint, line.pt1);
    v2d_f linev2d_f = v2d_f_sub(line.pt2, line.pt1);
    float magLinev2d_f = mag_v2d_f(linev2d_f);
    v2d_f nomralizedLinev2d_f = v2d_f_unit(linev2d_f);


    float dot = dot_product_v2d_f(linePt1ToCircle, nomralizedLinev2d_f);

    v2d_f ptOnVector = v2d_f_scalar_mult(dot, nomralizedLinev2d_f);
    v2d_f ptOnLine = v2d_f_add(ptOnVector, line.pt1);

    if(!intersects_PointAndLine(ptOnLine, line)){
        circle->collisionResult = (CollisionResult){
            SDL_FALSE,
            (v2d_f){0,0},
            (v2d_f){0,0}
        };
        return;
    }

    float x = circle->midPoint.x - ptOnLine.x;
    float y = circle->midPoint.y - ptOnLine.y;
    v2d_f collisionPt = ptOnLine;
    v2d_f collisionPtToCircleMidPoint = {x,y};
    v2d_f collisionNormal = v2d_f_unit(collisionPtToCircleMidPoint);
    if((x*x + y*y )<= (circle->radius*circle->radius)){
        circle->collisionResult =(CollisionResult){
            SDL_TRUE,
            collisionPt,
            collisionNormal
        };
        return;
    }
    circle->collisionResult = (CollisionResult){
        SDL_FALSE,
        (v2d_f){0,0},
        (v2d_f){0,0}
    };

}

void 
intersects_PolygonAndCircle(CollisionPolygon *cPoly, Circle *circle){
    unsigned int numVertices = lenDarr(cPoly->trfmPts);
    for(int i = 0;i < numVertices; i++){
        v2d_f pt1, pt2;
        pt1 = cPoly->trfmPts[i];
        pt2 = cPoly->trfmPts[(i+1)%numVertices];
        LineSegment polygonLineSegment = {pt1, pt2};

        intersects_LineAndCircle_Result(polygonLineSegment, circle);
        if(circle->collisionResult.isColliding){
            return;
        }
            
    }
    
}

void 
positionTransformPoly(CollisionPolygon *cPoly){
    for(int i = 0; i < lenDarr(cPoly->trfmPts); i++){
        cPoly->trfmPts[i] = v2d_f_add(cPoly->modelPts[i], cPoly->pos);
    }
}

void 
rotateTransformPoly(CollisionPolygon *cPoly){
    float s = sin(cPoly->rotationRads);
    float c = cos(cPoly->rotationRads);
    for(int i = 0; i < 7; i++){
        v2d_f pivotPt = cPoly->pos;
        v2d_f subtracted = v2d_f_sub(cPoly->trfmPts[i], pivotPt);
        v2d_f p;
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


v2d_f
addGravityToVelocity(v2d_f velocity){
    return (v2d_f){velocity.x, velocity.y+=PINBALL_MASS_g*9.81};
}

void
moveThing(v2d_f velocity, v2d_f *position, float deltaTime){
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
        (v2d_f){0,800},
        (v2d_f){640,900}
    };

    CollisionPolygon cPoly_1;
    cPoly_1.modelPts = initDarr(cPoly_1.modelPts, sizeof(v2d_f), 7);
    cPoly_1.trfmPts = initDarr(cPoly_1.trfmPts, sizeof(v2d_f), 7);
    addDarr(cPoly_1.modelPts, ((v2d_f){10,-20}), v2d_f);
    addDarr(cPoly_1.modelPts, ((v2d_f){-10,-20}), v2d_f);
    addDarr(cPoly_1.modelPts, ((v2d_f){-20,-5}), v2d_f);
    addDarr(cPoly_1.modelPts, ((v2d_f){-12,25}), v2d_f);
    addDarr(cPoly_1.modelPts, ((v2d_f){100,80}), v2d_f);
    addDarr(cPoly_1.modelPts, ((v2d_f){110,75}), v2d_f);
    addDarr(cPoly_1.modelPts, ((v2d_f){110,68}), v2d_f);
    for(int i = 0; i < lenDarr(cPoly_1.modelPts); i++){
        addDarr(cPoly_1.trfmPts, ((v2d_f){0,0}), v2d_f);
    }
    cPoly_1.rotationRads = 0.0f;
    cPoly_1.pos = (v2d_f){150,400};
    cPoly_1.flip = SDL_FALSE;
    cPoly_1.lerp = lerpInit(0,0,0);

    CollisionPolygon cPoly_2;
    polyDeepCopy(&cPoly_1, &cPoly_2);
    cPoly_2.pos = (v2d_f){300,400};
    cPoly_2.flip = SDL_TRUE;

    Circle circle_1;
    circle_1.radius = 40;
    circle_1.midPoint = (v2d_f){260,150};

    float pixelToMeter = circle_1.radius*2 / PINBALL_DIAMETER_m;


    v2d_f circleVelocity = {0.0f,0.0f};
    SDL_bool circleOnGround = SDL_FALSE;
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

        while(SDL_PollEvent(&event)) {
            if(event.type == SDL_QUIT) {
                quit = SDL_TRUE;
            }
            if(event.type == SDL_KEYUP){
                double time;
                switch(event.key.keysym.sym){
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
                        leftFlipperJustPressed = SDL_TRUE;
                        //lerpSet(&cPoly_2.lerp, -1, cPoly_2.rotationRads, 500 );
                        break;
                }
            }
        }

        // flippers presed. Set the lerp
        if(leftFlipperJustPressed) {
            double time = FLIPPER_TOTAL_TIME - cPoly_1.lerp.remainingTime;
            lerpSet(&cPoly_1.lerp, -1, cPoly_1.rotationRads, time );
        }
        else if(rightFlipperJustPressed){
            double time = FLIPPER_TOTAL_TIME - cPoly_1.lerp.remainingTime;
            lerpSet(&cPoly_2.lerp, -1, cPoly_2.rotationRads, time );

        }

        // process the lerp
        if(cPoly_1.lerp.remainingTime > 0.0f){
            interpolate(&cPoly_1.lerp, deltaTime);
            cPoly_1.rotationRads = cPoly_1.lerp.value;
        }

        if(cPoly_2.lerp.remainingTime > 0.0f){
            interpolate(&cPoly_2.lerp, deltaTime);
            cPoly_2.rotationRads = cPoly_2.lerp.value;
        }
        

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
        v2d_f force = (v2d_f){0, PINBALL_MASS_kg*9.81};
        //v2d_f acceleration = (v2d_f){force.x / PINBALL_MASS_kg, force.y/PINBALL_MASS_kg};
        //circleVelocity.x+=acceleration.x*deltaTime;
        
        intersects_LineAndCircle_Result(bottomLine, &circle_1);
        if(!circle_1.collisionResult.isColliding)
            intersects_PolygonAndCircle(&cPoly_1, &circle_1);
            
        if(!circle_1.collisionResult.isColliding)
            intersects_PolygonAndCircle(&cPoly_2, &circle_1);
        if(circle_1.collisionResult.isColliding && !circleOnGround){
            circleVelocity.y = 0;
            circleVelocity.x = 0;
        }
        if(circle_1.collisionResult.isColliding){
            circleOnGround = SDL_TRUE;
            circle_1.midPoint.y = circle_1.collisionResult.collisionPt.y + v2d_f_scalar_mult(circle_1.radius, circle_1.collisionResult.collisionNormal).y;
            circle_1.midPoint.x = circle_1.collisionResult.collisionPt.x + v2d_f_scalar_mult(circle_1.radius, circle_1.collisionResult.collisionNormal).x;
            // float rampAccel;
            // float rampAngle = getAngleOfLine(bottomLine);
            // rampAccel = PINBALL_DECLINE_GRAVITY*sin(rampAngle);
            // v2d_f ground_direction = lineVecNormal(bottomLine);
            // circleVelocity.x += (ground_direction.x*rampAccel)*deltaTime;
            // circleVelocity.y += (ground_direction.y*rampAccel)*deltaTime;
            // moveThing(circleVelocity, &circle_1.midPoint, deltaTime);
            //circle_1.midPoint.y += 2;
        }
        else{
            circleOnGround = SDL_FALSE;
            circleVelocity.y+=(pixelToMeter*PINBALL_DECLINE_GRAVITY)*deltaTime;
            moveThing(circleVelocity, &circle_1.midPoint, deltaTime);
        }
        
        // render
        SDL_SetRenderDrawColor(renderer, 20,20,20,255);
        SDL_RenderClear(renderer);
        
        drawGrid(renderer, 0,0,100,100,10, (SDL_Color){50,50,50,255} );
        
        SDL_SetRenderDrawColor(renderer, 250,250,0,255);
        SDL_RenderDrawLine(renderer, bottomLine.pt1.x, bottomLine.pt1.y, bottomLine.pt2.x, bottomLine.pt2.y);



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