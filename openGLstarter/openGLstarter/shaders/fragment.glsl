#version 330 core
out vec4 FragColor;

uniform vec3 shapeColour;

void main() {
    FragColor = vec4(shapeColour, 1.0);
}