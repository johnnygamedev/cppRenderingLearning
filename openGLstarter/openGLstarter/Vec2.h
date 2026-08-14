#ifndef VEC2_H
#define VEC2_H

#include <cmath>

struct Vec2 {
    float x;
    float y;

    // constructors
    Vec2() : x(0.0f), y(0.0f) {}
    Vec2(float x, float y) : x(x), y(y) {}

    // dot product
    float dot(const Vec2& v) const {
        return x * v.x + y * v.y;
    }

    // length of vector
    float length() const {
        return std::sqrt(x * x + y * y);
    }

    // returns unit vector
    Vec2 normalized() const {
        float len = length();
        if (len > 0.0f) {
            return Vec2(x / len, y / len);
        }
        return Vec2(0.0f, 0.0f);
    }

    // operator overloads
    Vec2 operator+(const Vec2& v) const {
        return Vec2(x + v.x, y + v.y);
    }

    Vec2 operator-(const Vec2& v) const {
        return Vec2(x - v.x, y - v.y);
    }

    Vec2 operator*(float scalar) const {
        return Vec2(x * scalar, y * scalar);
    }

    Vec2& operator+=(const Vec2& v) {
        x += v.x;
        y += v.y;
        return *this;
    }

    Vec2& operator-=(const Vec2& v) {
        x -= v.x;
        y -= v.y;
        return *this;
    }

    Vec2& operator*=(float scalar) {
        x *= scalar;
        y *= scalar;
        return *this;
    }
};

#endif