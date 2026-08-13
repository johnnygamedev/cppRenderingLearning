#pragma once
#ifndef RIGIDBODY_H
#define RIGIDBODY_H

#include "Vec2.h"

class RigidBody {
public:
    // linear
    Vec2 position;
    Vec2 velocity;
    Vec2 acceleration;
    Vec2 forceAccumulator;
    float mass;
    float invMass;

    // rotational
    float width;
    float height;
    // angle in radians
    float orientation;      
    // rad/s
    float angularVelocity;  
    float angularAcceleration;
    float torqueAccumulator;
    float inertia;          
    // 1 / inertia
    float invInertia;       

    RigidBody(Vec2 pos, float w, float h, float m = 1.0f);

    void addForce(const Vec2& force);
    // generates torque
    void addForceAtPoint(const Vec2& force, const Vec2& point); 
    void update(float dt);
};

#endif