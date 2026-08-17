#include "Collision.h"
#include <cmath>
#include <algorithm>


static void getBoxCorners(const RigidBody& body, Vec2 corners[4]) {
    float hw = body.width * 0.5f;
    float hh = body.height * 0.5f;

    float cosA = std::cos(body.orientation);
    float sinA = std::sin(body.orientation);

    
    Vec2 localCorners[4] = {
        Vec2(-hw,  hh),
        Vec2(hw,  hh),
        Vec2(hw, -hh),
        Vec2(-hw, -hh)
    };

    
    for (int i = 0; i < 4; ++i) {
        float rx = localCorners[i].x * cosA - localCorners[i].y * sinA;
        float ry = localCorners[i].x * sinA + localCorners[i].y * cosA;
        corners[i] = Vec2(rx + body.position.x, ry + body.position.y);
    }
}


static void projectVertices(const Vec2 corners[4], const Vec2& axis, float& minOut, float& maxOut) {
    minOut = corners[0].dot(axis);
    maxOut = minOut;

    for (int i = 1; i < 4; ++i) {
        float projection = corners[i].dot(axis);
        if (projection < minOut) minOut = projection;
        if (projection > maxOut) maxOut = projection;
    }
}

Manifold Collision::testBoxBox(RigidBody& a, RigidBody& b) {
    Manifold m;
    m.bodyA = &a;
    m.bodyB = &b;
    m.hasCollision = false;

    Vec2 cornersA[4];
    Vec2 cornersB[4];
    getBoxCorners(a, cornersA);
    getBoxCorners(b, cornersB);

    
    Vec2 axes[4] = {
        Vec2(std::cos(a.orientation), std::sin(a.orientation)),   
        Vec2(-std::sin(a.orientation), std::cos(a.orientation)), 
        Vec2(std::cos(b.orientation), std::sin(b.orientation)),   
        Vec2(-std::sin(b.orientation), std::cos(b.orientation))  
    };

    float minOverlap = 1e9f;
    Vec2 smallestAxis;

    
    for (int i = 0; i < 4; ++i) {
        Vec2 axis = axes[i].normalized();

        float minA, maxA, minB, maxB;
        projectVertices(cornersA, axis, minA, maxA);
        projectVertices(cornersB, axis, minB, maxB);

       
        if (maxA < minB || maxB < minA) {
            return m;
        }

        
        float overlap = std::min(maxA, maxB) - std::max(minA, minB);
        if (overlap < minOverlap) {
            minOverlap = overlap;
            smallestAxis = axis;
        }
    }

    
    m.hasCollision = true;
    m.depth = minOverlap;

    
    Vec2 dir = b.position - a.position;
    if (dir.dot(smallestAxis) < 0.0f) {
        smallestAxis = smallestAxis * -1.0f;
    }
    m.normal = smallestAxis;

   
    m.contactPoint = a.position + (dir * 0.5f);

    return m;
}

void Collision::resolveCollision(Manifold& m) {
    if (!m.hasCollision) return;

    RigidBody* a = m.bodyA;
    RigidBody* b = m.bodyB;

    float totalInvMass = a->invMass + b->invMass;
    if (totalInvMass == 0.0f) return; 

    
    const float percent = 0.8f;
    Vec2 correction = m.normal * (m.depth / totalInvMass) * percent;
    a->position -= correction * a->invMass;
    b->position += correction * b->invMass;

    
    Vec2 ra = m.contactPoint - a->position;
    Vec2 rb = m.contactPoint - b->position;

    
    Vec2 va = a->velocity + Vec2(-a->angularVelocity * ra.y, a->angularVelocity * ra.x);
    Vec2 vb = b->velocity + Vec2(-b->angularVelocity * rb.y, b->angularVelocity * rb.x);
    Vec2 rv = vb - va; 

    float velAlongNormal = rv.dot(m.normal);
    if (velAlongNormal > 0.0f) return; 

    
    float e = 0.5f;

    
    float raCrossN = (ra.x * m.normal.y) - (ra.y * m.normal.x);
    float rbCrossN = (rb.x * m.normal.y) - (rb.y * m.normal.x);

    float invMassSum = a->invMass + b->invMass +
        (raCrossN * raCrossN * a->invInertia) +
        (rbCrossN * rbCrossN * b->invInertia);

    float j = -(1.0f + e) * velAlongNormal / invMassSum;
    Vec2 impulse = m.normal * j;

    
    a->velocity -= impulse * a->invMass;
    a->angularVelocity -= (ra.x * impulse.y - ra.y * impulse.x) * a->invInertia;

    b->velocity += impulse * b->invMass;
    b->angularVelocity += (rb.x * impulse.y - rb.y * impulse.x) * b->invInertia;
}