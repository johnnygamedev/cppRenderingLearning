#ifndef PARTICLE_H
#define PARTICLE_H

#include "Vec2.h"

class Particle {
public:
    Vec2 position;
    Vec2 velocity;
    Vec2 acceleration;
    Vec2 forceAccumulator;

    float mass;
    float invMass;
    float radius;

   
    Particle(Vec2 pos, float m = 1.0f);
    void addForce(const Vec2& force);
    void update(float dt);
};

#endif