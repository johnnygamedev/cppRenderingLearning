#version 330 core
layout (location = 0) in vec3 aPos;

uniform vec2 uOffset;
uniform float uAngle; // angle in radians

void main() {
    // 2d rotation matrix
    float cosA = cos(uAngle);
    float sinA = sin(uAngle);

    // rotate around center
    float rotX = aPos.x * cosA - aPos.y * sinA;
    float rotY = aPos.x * sinA + aPos.y * cosA;

    // apply position offset
    gl_Position = vec4(rotX + uOffset.x, rotY + uOffset.y, aPos.z, 1.0);
}