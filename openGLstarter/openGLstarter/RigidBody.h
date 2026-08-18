#pragma once
#ifndef RIGIDBODY_H
#define RIGIDBODY_H

#include "Vec2.h"
#include "axisB.h"



class RigidBody {
public:
    AxisB getAxisB() const;
    Vec2 position;
    Vec2 velocity;
    Vec2 acceleration;
    Vec2 forceAccumulator;
    float mass;
    float invMass;

	bool isColliding = false;
    float width;
    float height;
    
    float orientation;      
    
    float angularVelocity;  
    float angularAcceleration;
    float torqueAccumulator;
    float inertia;          
    
    float invInertia;       

    RigidBody(Vec2 pos, float w, float h, float m = 1.0f);

    void addForce(const Vec2& force);
   
    void addForceAtPoint(const Vec2& force, const Vec2& point); 
    void update(float dt);
};

#endif