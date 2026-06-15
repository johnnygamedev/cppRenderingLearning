#include <iostream>
#include <fstream>
#include <sstream>
#include <glad/glad.h>
#include <GLFW/glfw3.h>

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

// Helper: create a VAO/VBO from a vertex array
// Returns the VAO id, writes VBO id into vbo_out
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

    GLFWwindow* window = glfwCreateWindow(800, 600, "Renderer - Press 1, 2 or 3", NULL, NULL);
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

    // triangle
    float triangle[] = {
        -0.5f, -0.5f, 0.0f,
         0.5f, -0.5f, 0.0f,
         0.0f,  0.5f, 0.0f
    };

    // square
    float square[] = {
        -0.5f,  0.5f, 0.0f,   // top left
         0.5f,  0.5f, 0.0f,   // top right
         0.5f, -0.5f, 0.0f,   // bottom right

        -0.5f,  0.5f, 0.0f,   // top left
         0.5f, -0.5f, 0.0f,   // bottom right
        -0.5f, -0.5f, 0.0f    // bottom left
    };

    // star

    float star[] = {
        // top spike
         0.0f,  0.6f,  0.0f,
        -0.12f, 0.18f, 0.0f,
         0.12f, 0.18f, 0.0f,

         // top right spike
          0.57f,  0.18f, 0.0f,
          0.12f,  0.18f, 0.0f,
          0.22f, -0.1f,  0.0f,

          // bot right spike
           0.35f, -0.55f, 0.0f,
           0.22f, -0.1f,  0.0f,
           0.0f,  -0.22f, 0.0f,

           // bot left spike
           -0.35f, -0.55f, 0.0f,
            0.0f,  -0.22f, 0.0f,
           -0.22f, -0.1f,  0.0f,

           // top left spike
           -0.57f,  0.18f, 0.0f,
           -0.22f, -0.1f,  0.0f,
           -0.12f,  0.18f, 0.0f,

           // middle
           // tri 1
           -0.12f, 0.18f, 0.0f,
            0.12f, 0.18f, 0.0f,
            0.22f, -0.1f, 0.0f,

            // tri 2
            -0.12f, 0.18f, 0.0f,
             0.22f, -0.1f, 0.0f,
             0.0f, -0.22f, 0.0f,

             // tri 3
             -0.12f,  0.18f, 0.0f,
              0.0f,  -0.22f, 0.0f,
             -0.22f, -0.1f,  0.0f,
    };

    // create a VAO for each shape
    unsigned int VBO1, VBO2, VBO3;
    unsigned int VAO1 = createVAO(triangle, sizeof(triangle), VBO1);
    unsigned int VAO2 = createVAO(square, sizeof(square), VBO2);
    unsigned int VAO3 = createVAO(star, sizeof(star), VBO3);

    // track which shape is active and vertex count for each
    int activeShape = 1;
    int vertexCounts[] = { 0, 3, 6, 24 }; 

    // track key state to prevent held-key repeat
    bool key1Last = false, key2Last = false, key3Last = false;

    std::cout << "Press 1, 2 or 3 to switch shapes" << std::endl;

    while (!glfwWindowShouldClose(window)) {
        if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
            glfwSetWindowShouldClose(window, true);

        // switch shape on key press (not hold)
        bool key1 = glfwGetKey(window, GLFW_KEY_1) == GLFW_PRESS;
        bool key2 = glfwGetKey(window, GLFW_KEY_2) == GLFW_PRESS;
        bool key3 = glfwGetKey(window, GLFW_KEY_3) == GLFW_PRESS;

        if (key1 && !key1Last) { activeShape = 1; std::cout << "Shape: Triangle" << std::endl; }
        if (key2 && !key2Last) { activeShape = 2; std::cout << "Shape: Square" << std::endl; }
        if (key3 && !key3Last) { activeShape = 3; std::cout << "Shape: Star" << std::endl; }

        key1Last = key1;
        key2Last = key2;
        key3Last = key3;

        glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        glUseProgram(shaderProgram);

        // bind the correct VAO and draw
        if (activeShape == 1) { glBindVertexArray(VAO1); glDrawArrays(GL_TRIANGLES, 0, vertexCounts[1]); }
        else if (activeShape == 2) { glBindVertexArray(VAO2); glDrawArrays(GL_TRIANGLES, 0, vertexCounts[2]); }
        else if (activeShape == 3) { glBindVertexArray(VAO3); glDrawArrays(GL_TRIANGLES, 0, vertexCounts[3]); }

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    // cleanup
    glDeleteVertexArrays(1, &VAO1);
    glDeleteVertexArrays(1, &VAO2);
    glDeleteVertexArrays(1, &VAO3);
    glDeleteBuffers(1, &VBO1);
    glDeleteBuffers(1, &VBO2);
    glDeleteBuffers(1, &VBO3);
    glDeleteProgram(shaderProgram);
    glfwTerminate();
    return 0;
}