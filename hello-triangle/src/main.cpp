#include <iostream>
#include <fstream>
#include <string>
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <util/types.hpp>
#include <util/shaders.hpp>

// Simple exception handling for the program.
enum StatusCode {
    OK = 0,
    GLFW_ERROR,
    GLEW_ERROR,
    SHADER_ERROR,
};

void handle_resize(GLFWwindow* window, i32 width, i32 height)
{
    glViewport(0, 0, width, height);
}

void handle_inputs(GLFWwindow* window)
{
    // Close window when ESC key was pressed.
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);
}

int main()
{
    GLFWwindow* window;

    // Initialize and configure GLFW.
    if (!glfwInit())
    {
        std::cout << "Could not initialize GLFW." << std::endl;
        return StatusCode::GLFW_ERROR;
    }

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    window = glfwCreateWindow(800, 600, "Learn OpenGL", nullptr, nullptr);
    if (window == nullptr)
    {
        std::cout << "Could not create window." << std::endl;
        glfwTerminate();
        return StatusCode::GLFW_ERROR;
    }
    glfwMakeContextCurrent(window);

    // Initialize GLEW. This makes the OpenGL libraries available to the program.
    if (glewInit() != GLEW_OK)
    {
        std::cout << "Could not initialize GLEW." << std::endl;
        glfwTerminate();
        return StatusCode::GLEW_ERROR;
    }

    glViewport(0, 0, 800, 600);
    glfwSetFramebufferSizeCallback(window, handle_resize);

    glClearColor(0.0f, 0.9f, 0.5f, 1.0f);

    /* Load shaders. */

    u32 vertex_shader;
    u32 fragment_shader;
    u32 shader_program;

    i32 compilation_successful;
    char log[512];

    // Load and compile vertex shader.
    vertex_shader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertex_shader, 1, &vertex_shader_source, nullptr);
    glCompileShader(vertex_shader);

    glGetShaderiv(vertex_shader, GL_COMPILE_STATUS, &compilation_successful);
    if (!compilation_successful)
    {
        glGetShaderInfoLog(vertex_shader, 512, nullptr, log);
        std::cout << "Could not compile vertex shader." << std::endl;
        std::cout << log << std::endl;
        return StatusCode::SHADER_ERROR;
    }

    // Load and compile fragment shader.
    fragment_shader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragment_shader, 1, &fragment_shader_source, nullptr);
    glCompileShader(fragment_shader);

    glGetShaderiv(fragment_shader, GL_COMPILE_STATUS, &compilation_successful);
    if (!compilation_successful)
    {
        glGetShaderInfoLog(fragment_shader, 512, nullptr, log);
        std::cout << "Could not compile fragment shader." << std::endl;
        std::cout << log << std::endl;
        return StatusCode::SHADER_ERROR;
    }

    // Create shader program and link the shaders to it.
    shader_program = glCreateProgram();
    glAttachShader(shader_program, vertex_shader);
    glAttachShader(shader_program, fragment_shader);
    glLinkProgram(shader_program);

    glGetShaderiv(shader_program, GL_LINK_STATUS, &compilation_successful);
    if (!compilation_successful)
    {
        glGetProgramInfoLog(shader_program, 512, nullptr, log);
        std::cout << "Could not link shader program." << std::endl;
        std::cout << log << std::endl;
        return StatusCode::SHADER_ERROR;
    }

    // These aren't needed anymore since the shader program was created successfully
    // and can be unloaded.
    glDeleteShader(vertex_shader);
    glDeleteShader(fragment_shader);

    /* Create and populate vertex buffer. */

    float triangle[] = {
        -0.5f, -0.5f, 0.0f, 
         0.5f, -0.5f, 0.0f,
         0.0f,  0.5f, 0.0f 
    };

    GLuint vao, vbo;

    // 1. Create the objects.
    glGenVertexArrays(1, &vao);
    glGenBuffers(1, &vbo);

    // 2. Bind the objects.
    glBindVertexArray(vao);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);

    // 3. Populate buffer object.
    glBufferData(GL_ARRAY_BUFFER, sizeof(triangle), triangle, GL_STATIC_DRAW);

    // 4. Specify the memory layout.
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    // 5. Unbind the objects.
    glBindBuffer(GL_ARRAY_BUFFER, 0); 
    glBindVertexArray(0); 

    /* Main loop */

    while (!glfwWindowShouldClose(window))
    {
        handle_inputs(window);

        glClear(GL_COLOR_BUFFER_BIT);

        // Draw the triangle.
        glUseProgram(shader_program);
        glBindVertexArray(vao);
        glDrawArrays(GL_TRIANGLES, 0, 3);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwTerminate();
    return StatusCode::OK;
}
