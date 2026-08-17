#ifndef AXISB_H
#define AXISB_H

#include "Vec2.h"

struct AxisB {
    Vec2 min;
    Vec2 max;

    bool overlaps(const AxisB& other) const {
        if (max.x < other.min.x || min.x > other.max.x) return false;
        if (max.y < other.min.y || min.y > other.max.y) return false;
        return true;
    }
};

#endif#pragma once
