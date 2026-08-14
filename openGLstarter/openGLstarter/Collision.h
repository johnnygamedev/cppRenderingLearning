#ifndef COLLISION_H
#define COLLISION_H

#include "Vec2.h"
#include "RigidBody.h"

// collision details container
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
    // sat box vs box collision test
    static Manifold testBoxBox(RigidBody& a, RigidBody& b);

    // impulse and position resolution
    static void resolveCollision(Manifold& m);
};

#endif#pragma once
