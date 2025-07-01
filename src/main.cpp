#include <glad/glad.h>      // OpenGL function loader
#include <GLFW/glfw3.h>     // Window and input management
#include <iostream>         // For console output

// Callback to adjust the OpenGL viewport when the window is resized
void framebuffer_size_callback(GLFWwindow* window, int width, int height) {
    glViewport(0, 0, width, height);
}

// Vertex shader source code (runs on GPU, processes each vertex)
const char* vertexShaderSource = R"(
#version 330 core
layout (location = 0) in vec3 aPos;
void main() {
    gl_Position = vec4(aPos, 1.0);
}
)";

// Fragment shader source code (runs on GPU, colors each pixel)
const char* fragmentShaderSource = R"(
#version 330 core
out vec4 FragColor;
void main() {
    FragColor = vec4(1.0, 0.5, 0.2, 1.0);
}
)";

// Global flags for toggling shape and wireframe mode
bool drawTriangle = true;
bool wireframeMode = false;

// Handles keyboard input for toggling shape and wireframe mode
void processInput(GLFWwindow* window)
{
    static bool spacePressedLastFrame = false;
    static bool wPressedLastFrame = false;

    // Close window if ESC is pressed
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);

    // Toggle between triangle and square with SPACE (debounced)
    if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS) {
        if (!spacePressedLastFrame) {
            drawTriangle = !drawTriangle;
        }
        spacePressedLastFrame = true;
    }
    else {
        spacePressedLastFrame = false;
    }

    // Toggle wireframe mode with W (debounced)
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) {
        if (!wPressedLastFrame) {
            wireframeMode = !wireframeMode;
            if (wireframeMode)
                glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);  // Draw wireframe
            else
                glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);  // Draw solid
        }
        wPressedLastFrame = true;
    }
    else {
        wPressedLastFrame = false;
    }
}

int main() {

    std::cout << "Information\n\n";
	std::cout << "Controls:\n";
	std::cout << "Press SPACE to toggle between triangle and square.\n";
	std::cout << "Press W to toggle wireframe mode.\n";
	std::cout << "Press ESC to exit the application.\n";

    // Initialize GLFW library
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3); // OpenGL version 3.x
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3); // OpenGL version x.3
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE); // Use core profile

    // Create a windowed mode window and its OpenGL context
    GLFWwindow* window = glfwCreateWindow(800, 600, "OpenGL Shapes", nullptr, nullptr);
    if (!window) {
        std::cerr << "Failed to create GLFW window\n";
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window); // Make the window's context current
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback); // Set resize callback

    // Load OpenGL function pointers using GLAD
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        std::cerr << "Failed to initialize GLAD\n";
        glfwTerminate();
        return -1;
    }

    // Vertex data for a triangle (3 vertices, each with x, y, z)
    float triangleVertices[] = {
         0.0f,  0.5f, 0.0f,
        -0.5f, -0.5f, 0.0f,
         0.5f, -0.5f, 0.0f
    };

    // Vertex data for a square (4 vertices, each with x, y, z)
    float squareVertices[] = {
         0.5f,  0.5f, 0.0f,
         0.5f, -0.5f, 0.0f,
        -0.5f, -0.5f, 0.0f,
        -0.5f,  0.5f, 0.0f
    };

    // Indices for drawing the square using two triangles
    unsigned int squareIndices[] = {
        0, 1, 3,
        1, 2, 3
    };

    // Vertex Array and Buffer Objects for triangle and square
    unsigned int triangleVAO, triangleVBO;
    unsigned int squareVAO, squareVBO, squareEBO;

    // --- Triangle setup ---
    glGenVertexArrays(1, &triangleVAO); // Create VAO
    glGenBuffers(1, &triangleVBO);      // Create VBO
    glBindVertexArray(triangleVAO);     // Bind VAO
    glBindBuffer(GL_ARRAY_BUFFER, triangleVBO); // Bind VBO
    glBufferData(GL_ARRAY_BUFFER, sizeof(triangleVertices), triangleVertices, GL_STATIC_DRAW); // Upload data
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0); // Set vertex attribute
    glEnableVertexAttribArray(0);       // Enable attribute
    glBindVertexArray(0);               // Unbind VAO

    // --- Square setup ---
    glGenVertexArrays(1, &squareVAO);   // Create VAO
    glGenBuffers(1, &squareVBO);        // Create VBO
    glGenBuffers(1, &squareEBO);        // Create EBO
    glBindVertexArray(squareVAO);       // Bind VAO
    glBindBuffer(GL_ARRAY_BUFFER, squareVBO); // Bind VBO
    glBufferData(GL_ARRAY_BUFFER, sizeof(squareVertices), squareVertices, GL_STATIC_DRAW); // Upload data
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, squareEBO); // Bind EBO
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(squareIndices), squareIndices, GL_STATIC_DRAW); // Upload indices
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0); // Set vertex attribute
    glEnableVertexAttribArray(0);       // Enable attribute
    glBindVertexArray(0);               // Unbind VAO

    // --- Shader compilation ---
    unsigned int vertexShader = glCreateShader(GL_VERTEX_SHADER); // Create vertex shader
    glShaderSource(vertexShader, 1, &vertexShaderSource, nullptr); // Attach source
    glCompileShader(vertexShader); // Compile

    unsigned int fragmentShader = glCreateShader(GL_FRAGMENT_SHADER); // Create fragment shader
    glShaderSource(fragmentShader, 1, &fragmentShaderSource, nullptr); // Attach source
    glCompileShader(fragmentShader); // Compile

    // --- Shader program linking ---
    unsigned int shaderProgram = glCreateProgram(); // Create program
    glAttachShader(shaderProgram, vertexShader);    // Attach vertex shader
    glAttachShader(shaderProgram, fragmentShader);  // Attach fragment shader
    glLinkProgram(shaderProgram);                   // Link program

    // Delete shader objects after linking (no longer needed)
    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);

    // --- Render loop ---
    while (!glfwWindowShouldClose(window)) {
        processInput(window); // Handle input

        glClearColor(0.2f, 0.3f, 0.3f, 1.0f); // Set background color
        glClear(GL_COLOR_BUFFER_BIT);          // Clear the screen

        glUseProgram(shaderProgram);           // Use the shader program

        if (drawTriangle) {
            glBindVertexArray(triangleVAO);   // Bind triangle VAO
            glDrawArrays(GL_TRIANGLES, 0, 3); // Draw triangle
        } else {
            glBindVertexArray(squareVAO);     // Bind square VAO
            glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0); // Draw square
        }

        glfwSwapBuffers(window); // Swap front and back buffers
        glfwPollEvents();        // Poll for and process events
    }

    // --- Cleanup resources ---
    glDeleteVertexArrays(1, &triangleVAO);
    glDeleteBuffers(1, &triangleVBO);
    glDeleteVertexArrays(1, &squareVAO);
    glDeleteBuffers(1, &squareVBO);
    glDeleteBuffers(1, &squareEBO);

    glfwTerminate(); // Terminate GLFW
    return 0;
}