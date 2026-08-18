#ifndef CONSTRAINT_H
#define CONSTRAINT_H

#include "Vec2.h"
#include "RigidBody.h"


class DistanceConstraint {
public:
	RigidBody* bodyA;
	RigidBody* bodyB;
	float restLength;

	DistanceConstraint(RigidBody* a, RigidBody* b, float length)

		: bodyA(a), bodyB(b), restLength(length) {
	}

	void applyConstraint() {
		Vec2 delta = bodyB->position - bodyA->position; //---------------------------------------------------Pointers here, i need to get on the cherno and understand these more in depth icl

		float currentLength = delta.length();
		float difference = currentLength - restLength;

		if (currentLength > 0.0f) {

			Vec2 correction = delta.normalized() * (difference * 0.5f);
			bodyA->position += correction;
			bodyB->position -= correction;
		}
	}

};








































#endif#pragma once