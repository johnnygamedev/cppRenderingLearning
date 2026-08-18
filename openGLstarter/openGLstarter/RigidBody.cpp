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

    
    invMass = (m > 0.0f) ? 1.0f / m : 0.0f;

    
    if (m > 0.0f) {
        inertia = (1.0f / 12.0f) * mass * (width * width + height * height);
        invInertia = 1.0f / inertia;
    }
    else {
        inertia = 0.0f;
        invInertia = 0.0f;
    }
}

void RigidBody::addForce(const Vec2& force) {
    forceAccumulator += force;
}

void RigidBody::addForceAtPoint(const Vec2& force, const Vec2& point) {
    forceAccumulator += force;

    
    Vec2 r = point - position;

    
    float torque = (r.x * force.y) - (r.y * force.x);
    torqueAccumulator += torque;
}

void RigidBody::update(float dt) { //----------------------------------------------------------------------------------- Work here for friction
    if (invMass == 0.0f) return;

    
    acceleration = forceAccumulator * invMass;
    velocity += acceleration * dt;

    position += velocity * dt;

    
    angularAcceleration = torqueAccumulator * invInertia;
    angularVelocity += angularAcceleration * dt;
    orientation += angularVelocity * dt;

	velocity *= 0.99f;
	angularVelocity *= 0.98f;

    
    forceAccumulator = Vec2(0.0f, 0.0f);
    torqueAccumulator = 0.0f;
}


 AxisB RigidBody::getAxisB() const {
    float hw = width * 0.5f;
    float hh = height * 0.5f;

    float cosA = std::abs(std::cos(orientation));
    float sinA = std::abs(std::sin(orientation));

    float boundingHw = hw * cosA + hh * sinA;
    float boundingHh = hw * sinA + hh * cosA;

    AxisB box;
    box.min = Vec2(position.x - boundingHw, position.y - boundingHh);
    box.max = Vec2(position.x + boundingHw, position.y + boundingHh);
    return box;
}
