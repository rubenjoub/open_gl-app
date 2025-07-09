#include <glad/glad.h>      // OpenGL function loader
#include <GLFW/glfw3.h>     // Window and input management
#include <iostream>         // For console output
#include <glm/glm.hpp>       // GLM for vector and matrix math
#include <glm/gtc/matrix_transform.hpp> // GLM for transformations
#include <glm/gtc/type_ptr.hpp> // GLM for converting to pointer types
#include <fstream>
#include <sstream>
#include <string>

// Callback to adjust the OpenGL viewport when the window is resized
void framebuffer_size_callback(GLFWwindow* window, int width, int height) {
    glViewport(0, 0, width, height);
}

// Function to read a file and return its contents as a string
std::string readFile(const char* filePath) {
    std::ifstream file(filePath);
    std::stringstream buffer;
    buffer << file.rdbuf();
    return buffer.str();
}

glm::vec3 cameraPos = glm::vec3(0.0f, 0.0f, 3.0f);
glm::vec3 cameraFront = glm::vec3(0.0f, 0.0f, -1.0f);
glm::vec3 cameraUp = glm::vec3(0.0f, 1.0f, 0.0f);

// Global flags for toggling shape and wireframe mode
bool drawTriangle = true;
bool wireframeMode = false;
bool drawCube = false;

// Camera parameters
float yaw = -90.0f;   // Yaw is initialized to -90.0 degrees to look along the negative Z
float pitch = 0.0f;
float lastX = 400.0f; // Center of your window
float lastY = 300.0f;
bool firstMouse = true;

void mouse_callback(GLFWwindow* window, double xpos, double ypos) {
    static float sensitivity = 0.1f;
    static bool dragging = false;

    // Check if left mouse button is pressed
    if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS) {
        if (!dragging) {
            // Start drag: initialize lastX/lastY
            lastX = (float)xpos;
            lastY = (float)ypos;
            firstMouse = false;
            dragging = true;
        }

        float xoffset = (float)xpos - lastX;
        float yoffset = lastY - (float)ypos; // reversed: y ranges bottom to top
        lastX = (float)xpos;
        lastY = (float)ypos;

        xoffset *= sensitivity;
        yoffset *= sensitivity;

        yaw += xoffset;
        pitch += yoffset;

        // Constrain pitch
        if (pitch > 89.0f) pitch = 89.0f;
        if (pitch < -89.0f) pitch = -89.0f;

        glm::vec3 front;
        front.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
        front.y = sin(glm::radians(pitch));
        front.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
        cameraFront = glm::normalize(front);
    } else {
        dragging = false;
        firstMouse = true;
    }
}

// Handles keyboard input for toggling shape and wireframe mode
void processInput(GLFWwindow* window)
{
    static bool spacePressedLastFrame = false;
    static bool wPressedLastFrame = false;
    static bool cPressedLastFrame = false;

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

    // Toggle wireframe mode with Q (debounced)
    if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS) {
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

    // Toggle cube drawing with C (debounced)
    if (glfwGetKey(window, GLFW_KEY_C) == GLFW_PRESS) {
        if (!cPressedLastFrame) {
            drawCube = !drawCube;
        }
        cPressedLastFrame = true;
    } else {
        cPressedLastFrame = false;
    }

    float cameraSpeed = 0.05f; // adjust as needed
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        cameraPos += cameraSpeed * cameraFront;
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        cameraPos -= cameraSpeed * cameraFront;
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        cameraPos -= glm::normalize(glm::cross(cameraFront, cameraUp)) * cameraSpeed;
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        cameraPos += glm::normalize(glm::cross(cameraFront, cameraUp)) * cameraSpeed;
}

void setupVAO(unsigned int& VAO, unsigned int& VBO, float* vertices, size_t vertSize, unsigned int* EBO = nullptr, unsigned int* indices = nullptr, size_t indSize = 0) {
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, vertSize, vertices, GL_STATIC_DRAW);
    if (EBO && indices && indSize > 0) {
        glGenBuffers(1, EBO);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, *EBO);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, indSize, indices, GL_STATIC_DRAW);
    }
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
}

unsigned int createShaderProgram(const char* vSource, const char* fSource) {
    unsigned int vShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vShader, 1, &vSource, nullptr);
    glCompileShader(vShader);

    unsigned int fShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fShader, 1, &fSource, nullptr);
    glCompileShader(fShader);

    unsigned int program = glCreateProgram();
    glAttachShader(program, vShader);
    glAttachShader(program, fShader);
    glLinkProgram(program);

    glDeleteShader(vShader);
    glDeleteShader(fShader);
    return program;
}

int main() {

    std::cout << "Information\n\n";
	std::cout << "Controls:\n";
	std::cout << "Press SPACE to toggle between triangle and square.\n";
	std::cout << "Press Q to toggle wireframe mode.\n";
	std::cout << "Press ESC to exit the application.\n";
    std::cout << "Use mouse drag (left button) to look around.\n";
    std::cout << "Use W/A/S/D to move the camera.\n";

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

    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL); // Hide and capture cursor
    glfwSetCursorPosCallback(window, mouse_callback);

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

    // Vertex data for a cube (36 vertices, each with x, y, z)
    float cubeVertices[] = {
        // positions          
        -0.5f, -0.5f, -0.5f,  
         0.5f, -0.5f, -0.5f,  
         0.5f,  0.5f, -0.5f,  
         0.5f,  0.5f, -0.5f,  
        -0.5f,  0.5f, -0.5f,  
        -0.5f, -0.5f, -0.5f,  

        -0.5f, -0.5f,  0.5f,  
         0.5f, -0.5f,  0.5f,  
         0.5f,  0.5f,  0.5f,  
         0.5f,  0.5f,  0.5f,  
        -0.5f,  0.5f,  0.5f,  
        -0.5f, -0.5f,  0.5f,  

        -0.5f,  0.5f,  0.5f,  
        -0.5f,  0.5f, -0.5f,  
        -0.5f, -0.5f, -0.5f,  
        -0.5f, -0.5f, -0.5f,  
        -0.5f, -0.5f,  0.5f,  
        -0.5f,  0.5f,  0.5f,  

         0.5f,  0.5f,  0.5f,  
         0.5f,  0.5f, -0.5f,  
         0.5f, -0.5f, -0.5f,  
         0.5f, -0.5f, -0.5f,  
         0.5f, -0.5f,  0.5f,  
         0.5f,  0.5f,  0.5f,  

        -0.5f, -0.5f, -0.5f,  
         0.5f, -0.5f, -0.5f,  
         0.5f, -0.5f,  0.5f,  
         0.5f, -0.5f,  0.5f,  
        -0.5f, -0.5f,  0.5f,  
        -0.5f, -0.5f, -0.5f,  

        -0.5f,  0.5f, -0.5f,  
         0.5f,  0.5f, -0.5f,  
         0.5f,  0.5f,  0.5f,  
         0.5f,  0.5f,  0.5f,  
        -0.5f,  0.5f,  0.5f,  
        -0.5f,  0.5f, -0.5f,  
    };

    // Vertex Array and Buffer Objects for triangle, square, and cube
    unsigned int triangleVAO, triangleVBO;
    unsigned int squareVAO, squareVBO, squareEBO;
    unsigned int cubeVAO, cubeVBO;

    setupVAO(triangleVAO, triangleVBO, triangleVertices, sizeof(triangleVertices));
    setupVAO(squareVAO, squareVBO, squareVertices, sizeof(squareVertices), &squareEBO, squareIndices, sizeof(squareIndices));
    setupVAO(cubeVAO, cubeVBO, cubeVertices, sizeof(cubeVertices));

    std::string vertexSource = readFile("shaders/vertex.glsl");
    std::string fragmentSource = readFile("shaders/fragment.glsl");
    unsigned int shaderProgram = createShaderProgram(vertexSource.c_str(), fragmentSource.c_str());

    // --- Render loop ---
    while (!glfwWindowShouldClose(window)) {
        processInput(window); // Handle input

        glClearColor(0.2f, 0.3f, 0.3f, 1.0f); // Set background color
        glClear(GL_COLOR_BUFFER_BIT);          // Clear the screen

        glUseProgram(shaderProgram);           // Use the shader program

        glm::mat4 view = glm::lookAt(cameraPos, cameraPos + cameraFront, cameraUp);
        int viewLoc = glGetUniformLocation(shaderProgram, "view");
        glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));

        glm::mat4 projection = glm::perspective(glm::radians(45.0f), 800.0f / 600.0f, 0.1f, 100.0f);
        int projLoc = glGetUniformLocation(shaderProgram, "projection");
        glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(projection));

        if (drawCube) {
            glBindVertexArray(cubeVAO);
            glDrawArrays(GL_TRIANGLES, 0, 36);
        } else if (drawTriangle) {
            glBindVertexArray(triangleVAO);
            glDrawArrays(GL_TRIANGLES, 0, 3);
        } else {
            glBindVertexArray(squareVAO);
            glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
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
    glDeleteVertexArrays(1, &cubeVAO);
    glDeleteBuffers(1, &cubeVBO);

    glfwTerminate(); // Terminate GLFW
    return 0;
}