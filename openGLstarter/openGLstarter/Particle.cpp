#include "Particle.h"

Particle::Particle(Vec2 pos, float m)
    : position(pos),
    velocity(0.0f, 0.0f),       
    acceleration(0.0f, 0.0f),   
    forceAccumulator(0.0f, 0.0f),
    mass(m),
    radius(0.05f) {
    invMass = (m > 0.0f) ? 1.0f / m : 0.0f;
}

void Particle::addForce(const Vec2& force) {
    forceAccumulator += force;
}

void Particle::update(float dt) {
    if (invMass == 0.0f) return;

    acceleration = forceAccumulator * invMass;
    velocity += acceleration * dt;
    position += velocity * dt;

    forceAccumulator = Vec2(0.0f, 0.0f);
}