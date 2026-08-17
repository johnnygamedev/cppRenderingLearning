#ifndef COLLISION_H
#define COLLISION_H

#include "Vec2.h"
#include "RigidBody.h"


struct Manifold {
    RigidBody* bodyA;
    RigidBody* bodyB;
    Vec2 normal;         
    float depth;          
    Vec2 contactPoint;   
    bool hasCollision;   
};

class Collision {
public:
   
    static Manifold testBoxBox(RigidBody& a, RigidBody& b);

    
    static void resolveCollision(Manifold& m);
};

#endif#pragma once
