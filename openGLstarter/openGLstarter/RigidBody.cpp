#include "RigidBody.h"

RigidBody::RigidBody(Vec2 pos, float w, float h, float m)
    : position(pos),
    velocity(0.0f, 0.0f),
    acceleration(0.0f, 0.0f),
    forceAccumulator(0.0f, 0.0f),
    mass(m),
    width(w),
    height(h),
    orientation(0.0f),
    angularVelocity(0.0f),
    angularAcceleration(0.0f),
    torqueAccumulator(0.0f) {

    // inverse mass
    invMass = (m > 0.0f) ? 1.0f / m : 0.0f;

    // moment of inertia for 2d box
    if (m > 0.0f) {
        inertia = (1.0f / 12.0f) * mass * (width * width + height * height);
        invInertia = 1.0f / inertia;
    }
    else {
        inertia = 0.0f;
        invInertia = 0.0f; // static body
    }
}

void RigidBody::addForce(const Vec2& force) {
    forceAccumulator += force;
}

void RigidBody::addForceAtPoint(const Vec2& force, const Vec2& point) {
    forceAccumulator += force;

    // lever arm
    Vec2 r = point - position;

    // 2d cross product for torque
    float torque = (r.x * force.y) - (r.y * force.x);
    torqueAccumulator += torque;
}

void RigidBody::update(float dt) {
    if (invMass == 0.0f) return;

    // linear physics
    acceleration = forceAccumulator * invMass;
    velocity += acceleration * dt;
    position += velocity * dt;

    // rotational physics
    angularAcceleration = torqueAccumulator * invInertia;
    angularVelocity += angularAcceleration * dt;
    orientation += angularVelocity * dt;

    // reset forces
    forceAccumulator = Vec2(0.0f, 0.0f);
    torqueAccumulator = 0.0f;
}