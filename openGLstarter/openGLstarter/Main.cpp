#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <cstdlib>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include "Vec2.h"
#include "RigidBody.h"
#include "Collision.h"

// dt tracking
float deltaTime = 0.0f;
float lastFrame = 0.0f;

void framebuffer_size_callback(GLFWwindow* window, int width, int height) {
    glViewport(0, 0, width, height);
}

unsigned int loadShader(const char* vertPath, const char* fragPath) {
    std::ifstream vFile(vertPath), fFile(fragPath);
    std::stringstream vStream, fStream;
    vStream << vFile.rdbuf();
    fStream << fFile.rdbuf();
    std::string vStr = vStream.str(), fStr = fStream.str();
    const char* vSrc = vStr.c_str();
    const char* fSrc = fStr.c_str();

    unsigned int vertShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertShader, 1, &vSrc, NULL);
    glCompileShader(vertShader);

    unsigned int fragShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragShader, 1, &fSrc, NULL);
    glCompileShader(fragShader);

    unsigned int program = glCreateProgram();
    glAttachShader(program, vertShader);
    glAttachShader(program, fragShader);
    glLinkProgram(program);

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

    GLFWwindow* window = glfwCreateWindow(800, 600, "2D Engine - Rigid Bodies & SAT Collisions", NULL, NULL);
    if (!window) {
        glfwTerminate();
        return -1;
    }

    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) return -1;

    unsigned int shaderProgram = loadShader("shaders/vertex.glsl", "shaders/fragment.glsl");

    // quad mesh (width: 0.08, height: 0.08)
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

    // physics world setup
    std::vector<RigidBody> bodies;
    Vec2 gravity(0.0f, -9.81f);

    // debounce flags
    bool spaceLast = false;
    bool mouseLast = false;

    while (!glfwWindowShouldClose(window)) {
        // dt step
        float currentFrame = static_cast<float>(glfwGetTime());
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;
        if (deltaTime > 0.05f) deltaTime = 0.05f;

        // exit check
        if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
            glfwSetWindowShouldClose(window, true);

        // spawn on spacebar
        bool spacePressed = glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS;
        if (spacePressed && !spaceLast) {
            RigidBody body(Vec2(0.0f, -0.5f), 0.08f, 0.08f, 1.0f);
            float randomX = ((rand() % 200) / 50.0f) - 2.0f;
            body.velocity = Vec2(randomX, 5.0f);
            body.angularVelocity = 6.0f; // initial spin
            bodies.push_back(body);
        }
        spaceLast = spacePressed;

        // spawn on mouse click
        bool mousePressed = glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS;
        if (mousePressed && !mouseLast) {
            double xpos, ypos;
            glfwGetCursorPos(window, &xpos, &ypos);

            int width, height;
            glfwGetWindowSize(window, &width, &height);

            // pixel to ndc
            float ndcX = (2.0f * static_cast<float>(xpos)) / width - 1.0f;
            float ndcY = 1.0f - (2.0f * static_cast<float>(ypos)) / height;

            RigidBody body(Vec2(ndcX, ndcY), 0.08f, 0.08f, 1.0f);
            body.angularVelocity = ((rand() % 100) / 10.0f) - 5.0f;
            bodies.push_back(body);
        }
        mouseLast = mousePressed;

        // 1. update physics
        for (auto& b : bodies) {
            b.addForce(gravity * b.mass);
            b.update(deltaTime);

            // floor check
            if (b.position.y < -0.9f) {
                b.position.y = -0.9f;
                b.velocity.y *= -0.5f;
                b.angularVelocity *= 0.8f;
            }
        }

        // 2. body vs body collisions (sat + impulse)
        for (size_t i = 0; i < bodies.size(); ++i) {
            for (size_t j = i + 1; j < bodies.size(); ++j) {
                Manifold m = Collision::testBoxBox(bodies[i], bodies[j]);
                if (m.hasCollision) {
                    Collision::resolveCollision(m);
                }
            }
        }

        // render
        glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        glUseProgram(shaderProgram);

        int offsetLoc = glGetUniformLocation(shaderProgram, "uOffset");
        int colourLoc = glGetUniformLocation(shaderProgram, "shapeColour");
        int angleLoc = glGetUniformLocation(shaderProgram, "uAngle");

        glUniform3f(colourLoc, 0.57f, 0.0f, 1.0f);
        glBindVertexArray(bodyVAO);

        // draw each box
        for (const auto& b : bodies) {
            glUniform2f(offsetLoc, b.position.x, b.position.y);
            glUniform1f(angleLoc, b.orientation); // send rotation angle
            glDrawArrays(GL_TRIANGLES, 0, 6);
        }

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    // cleanup
    glDeleteVertexArrays(1, &bodyVAO);
    glDeleteBuffers(1, &bodyVBO);
    glDeleteProgram(shaderProgram);
    glfwTerminate();

    return 0;
}