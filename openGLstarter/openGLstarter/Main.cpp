#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <random>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include "Vec2.h"
#include "RigidBody.h"
#include "Collision.h"
#include "axisB.h"
float deltaTime = 0.0f;
float lastFrame = 0.0f;

struct Uniforms {
    int offset = -1;
    int colour = -1;
    int angle = -1;
};

void framebuffer_size_callback(GLFWwindow* window, int width, int height) {
    glViewport(0, 0, width, height);
}

static unsigned int compileShader(unsigned int type, const char* source) {
    unsigned int id = glCreateShader(type);
    glShaderSource(id, 1, &source, NULL);
    glCompileShader(id);

    int success;
    glGetShaderiv(id, GL_COMPILE_STATUS, &success);
    if (!success) {
        char infoLog[512];
        glGetShaderInfoLog(id, 512, NULL, infoLog);
        std::cerr << "[shader compile error]: " << infoLog << "\n";
    }
    return id;
}

unsigned int loadShader(const char* vertPath, const char* fragPath) {
    std::ifstream vFile(vertPath), fFile(fragPath);
    if (!vFile.is_open() || !fFile.is_open()) {
        std::cerr << "[file error]: failed to open shader files\n";
        return 0;
    }

    std::stringstream vStream, fStream;
    vStream << vFile.rdbuf();
    fStream << fFile.rdbuf();
    std::string vStr = vStream.str(), fStr = fStream.str();

    unsigned int vertShader = compileShader(GL_VERTEX_SHADER, vStr.c_str());
    unsigned int fragShader = compileShader(GL_FRAGMENT_SHADER, fStr.c_str());

    unsigned int program = glCreateProgram();
    glAttachShader(program, vertShader);
    glAttachShader(program, fragShader);
    glLinkProgram(program);

    int success;
    glGetProgramiv(program, GL_LINK_STATUS, &success);
    if (!success) {
        char infoLog[512];
        glGetProgramInfoLog(program, 512, NULL, infoLog);
        std::cerr << "[shader link error]: " << infoLog << "\n";
    }

    glDeleteShader(vertShader);
    glDeleteShader(fragShader);

    return program;
}

unsigned int createVAO(float* verts, int size, unsigned int& vbo_out) {
    unsigned int VAO, VBO;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);

    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, size, verts, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    vbo_out = VBO;
    return VAO;
}

int main() {
    if (!glfwInit()) return -1;

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    GLFWwindow* window = glfwCreateWindow(800, 600, "2D Engine", NULL, NULL);
    if (!window) {
        glfwTerminate();
        return -1;
    }

    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) return -1;

    unsigned int shaderProgram = loadShader("shaders/vertex.glsl", "shaders/fragment.glsl");
    if (shaderProgram == 0) return -1;

    Uniforms uniforms;
    uniforms.offset = glGetUniformLocation(shaderProgram, "uOffset");
    uniforms.colour = glGetUniformLocation(shaderProgram, "shapeColour");
    uniforms.angle = glGetUniformLocation(shaderProgram, "uAngle");


    //random(i keep losing where it is) fs < ------------------------------------------------------------------------------------------------------------------------ Rand Here
    std::random_device rd;
    std::mt19937 rng(rd());
    std::uniform_real_distribution<float> distVelX(-2.0f, 2.0f);
    std::uniform_real_distribution<float> distAngVel(-5.0f, 5.0f);

    float bodyQuad[] = {
        -0.04f,  0.04f, 0.0f,
         0.04f,  0.04f, 0.0f,
         0.04f, -0.04f, 0.0f,

        -0.04f,  0.04f, 0.0f,
         0.04f, -0.04f, 0.0f,
        -0.04f, -0.04f, 0.0f
    };

    unsigned int bodyVBO;
    unsigned int bodyVAO = createVAO(bodyQuad, sizeof(bodyQuad), bodyVBO);

    std::vector<RigidBody> bodies;
    Vec2 gravity(0.0f, -9.81f);

    bool spaceLast = false;
    bool mouseLast = false;

    while (!glfwWindowShouldClose(window)) {
        float currentFrame = static_cast<float>(glfwGetTime());
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;
        if (deltaTime > 0.05f) deltaTime = 0.05f;

        if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
            glfwSetWindowShouldClose(window, true);

        bool spacePressed = glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS;
        if (spacePressed && !spaceLast) {
            RigidBody body(Vec2(0.0f, -0.5f), 0.08f, 0.08f, 1.0f);
            body.velocity = Vec2(distVelX(rng), 5.0f);
            body.angularVelocity = 6.0f;
            bodies.push_back(body);
        }
        spaceLast = spacePressed;

        bool mousePressed = glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS;
        if (mousePressed && !mouseLast) {
            double xpos, ypos;
            glfwGetCursorPos(window, &xpos, &ypos);

            int width, height;
            glfwGetWindowSize(window, &width, &height);

            float ndcX = (2.0f * static_cast<float>(xpos)) / width - 1.0f;
            float ndcY = 1.0f - (2.0f * static_cast<float>(ypos)) / height;

            RigidBody body(Vec2(ndcX, ndcY), 0.08f, 0.08f, 1.0f);
            body.angularVelocity = distAngVel(rng);
            bodies.push_back(body);
        }
        mouseLast = mousePressed;

        for (auto& b : bodies) {
            b.addForce(gravity * b.mass);
            b.update(deltaTime);

            if (b.position.y < -0.9f) {
                b.position.y = -0.9f;
                b.velocity.y *= -0.5f;
                b.angularVelocity *= 0.8f;
            }
        }

        for (size_t i = 0; i < bodies.size(); ++i) {
            AxisB boxA = bodies[i].getAxisB();

            for (size_t j = i + 1; j < bodies.size(); ++j) {
                AxisB boxB = bodies[j].getAxisB();

                if (!boxA.overlaps(boxB)) {
                    continue;
                }

                Manifold m = Collision::testBoxBox(bodies[i], bodies[j]);
                if (m.hasCollision) {
                    Collision::resolveCollision(m);
                }
            }
        }

        glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        glUseProgram(shaderProgram);
        glUniform3f(uniforms.colour, 0.57f, 0.0f, 1.0f);
        glBindVertexArray(bodyVAO);

        for (const auto& b : bodies) {
            glUniform2f(uniforms.offset, b.position.x, b.position.y);
            glUniform1f(uniforms.angle, b.orientation);
            glDrawArrays(GL_TRIANGLES, 0, 6);
        }

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glDeleteVertexArrays(1, &bodyVAO);
    glDeleteBuffers(1, &bodyVBO);
    glDeleteProgram(shaderProgram);
    glfwTerminate();

    return 0;
}