#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <cstdlib>

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include "Vec2.h"
#include "Particle.h"

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
    if (!glfwInit()) {
        std::cerr << "Failed to init GLFW" << std::endl;
        return -1;
    }

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    GLFWwindow* window = glfwCreateWindow(800, 600, "2D Engine - Particles", NULL, NULL);
    if (!window) {
        std::cerr << "Failed to create window" << std::endl;
        glfwTerminate();
        return -1;
    }

    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        std::cerr << "Failed to init GLAD" << std::endl;
        return -1;
    }

    unsigned int shaderProgram = loadShader("shaders/vertex.glsl", "shaders/fragment.glsl");

    // small base mesh for rendering particles
    float particleQuad[] = {
        -0.02f,  0.02f, 0.0f,
         0.02f,  0.02f, 0.0f,
         0.02f, -0.02f, 0.0f,

        -0.02f,  0.02f, 0.0f,
         0.02f, -0.02f, 0.0f,
        -0.02f, -0.02f, 0.0f
    };

    unsigned int particleVBO;
    unsigned int particleVAO = createVAO(particleQuad, sizeof(particleQuad), particleVBO);

    // physics world setup
    std::vector<Particle> particles;
    Vec2 gravity(0.0f, -9.81f);

    // input debounce flags
    bool spaceLast = false;
    bool mouseLast = false;

    std::cout << "Controls:" << std::endl;
    std::cout << " - Press SPACE to launch a particle from top" << std::endl;
    std::cout << " - Left Click anywhere in the window to spawn a particle" << std::endl;

    while (!glfwWindowShouldClose(window)) {
        // calculate delta time & cap max step
        float currentFrame = static_cast<float>(glfwGetTime());
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        if (deltaTime > 0.05f) deltaTime = 0.05f;

        // input checks
        if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
            glfwSetWindowShouldClose(window, true);

        // spawn particle on spacebar
       
        bool spacePressed = glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS;
        if (spacePressed && !spaceLast) {
            Particle p(Vec2(0.0f, -0.5f), 1.0f); 
            float randomX = ((rand() % 200) / 50.0f) - 2.0f;
            p.velocity = Vec2(randomX, 5.0f);
            std::cout << "Spawned Space particle! Initial Velocity: "
                << p.velocity.x << ", " << p.velocity.y << std::endl;
            particles.push_back(p);
        }
        spaceLast = spacePressed;

        // spawn particle on mouse click
        bool mousePressed = glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS;
        if (mousePressed && !mouseLast) {
            double xpos, ypos;
            glfwGetCursorPos(window, &xpos, &ypos);

            int width, height;
            glfwGetWindowSize(window, &width, &height);

            // convert pixel coordinates to opengl ndc
            float ndcX = (2.0f * static_cast<float>(xpos)) / width - 1.0f;
            float ndcY = 1.0f - (2.0f * static_cast<float>(ypos)) / height;

            Particle p(Vec2(ndcX, ndcY), 1.0f);
            particles.push_back(p);
        }
        mouseLast = mousePressed;

        // update physics
        for (auto& p : particles) {
            p.addForce(gravity * p.mass);
            p.update(deltaTime);

            // basic floor response
            if (p.position.y < -0.9f) {
                p.position.y = -0.9f;
                p.velocity.y *= -0.75f;
                p.velocity.x *= 0.98f;
            }
        }
        
        // render
        glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        glUseProgram(shaderProgram);

        int offsetLoc = glGetUniformLocation(shaderProgram, "uOffset");
        int colourLoc = glGetUniformLocation(shaderProgram, "shapeColour");

        //debug check
        if (offsetLoc == -1) {
            std::cout << "ERROR: 'uOffset' uniform not found in vertex shader!" << std::endl;
        }

        glUniform3f(colourLoc, 0.57f, 0.0f, 1.0f);

        glBindVertexArray(particleVAO);

        // draw each particle at its position
        for (const auto& p : particles) {
            glUniform2f(offsetLoc, p.position.x, p.position.y);
            glDrawArrays(GL_TRIANGLES, 0, 6);
        }

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    // cleanup resources
    glDeleteVertexArrays(1, &particleVAO);
    glDeleteBuffers(1, &particleVBO);
    glDeleteProgram(shaderProgram);
    glfwTerminate();

    return 0;
}