#include <iostream>
#include <string>
#include <cstdio>
#include <math.h>
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <util/types.hpp>

static const char* WINDOW_TITLE = "Learn OpenGL";

// Simple exception handling for the program.
enum StatusCode {
    OK = 0,
    GLFW_ERROR,
    GLEW_ERROR,
    SHADER_ERROR,
};

struct Vec3D {
    float x, y, z;
};

struct Color {
    float r, g, b, a;
};

char* load_shader_source(const char* file_name)
{
    FILE* shader_file = fopen(file_name, "r");

    if (!shader_file)
        return nullptr;

    // Get file size in bytes.
    fseek(shader_file, 0L, SEEK_END);
    u64 file_size = ftell(shader_file);
    rewind(shader_file);

    if (!file_size)
        return nullptr;

    char* shader_source = (char*)malloc(file_size);
    fread(shader_source, file_size, 1, shader_file);
    fclose(shader_file);

    return shader_source;
}

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

    window = glfwCreateWindow(800, 600, WINDOW_TITLE, nullptr, nullptr);
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
    
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

    /* Load shaders. */

    GLuint vertex_shader;
    GLuint fragment_shader;
    GLuint shader_program;

    i32 compilation_successful;
    char log[512];

    vertex_shader = glCreateShader(GL_VERTEX_SHADER);

    char* vertex_shader_source = load_shader_source("src/shaders/vertex_shader.glsl");
    if (!vertex_shader_source)
    {
        std::cout << "Could not load vertex shader." << std::endl;
        return StatusCode::SHADER_ERROR;
    }

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

    fragment_shader = glCreateShader(GL_FRAGMENT_SHADER);

    char* fragment_shader_source = load_shader_source("src/shaders/fragment_shader.glsl");
    if (!fragment_shader_source)
    {
        std::cout << "Could not load fragment shader." << std::endl;
        return StatusCode::SHADER_ERROR;
    }

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
    free(vertex_shader_source);
    glDeleteShader(vertex_shader);
    free(fragment_shader_source);
    glDeleteShader(fragment_shader);

    /* Create and populate vertex buffer. */    

    Vec3D vertices[] = {
        { -0.5f, -0.5f, 0.0f }, // left
        {  0.5f, -0.5f, 0.0f }, // right
        {  0.0f,  0.5f, 0.0f }, // top
    };

    GLuint vao, vbo;

    // 1. Create the objects.
    glGenVertexArrays(1, &vao);
    glGenBuffers(1, &vbo);

    // 2. Bind the objects.
    glBindVertexArray(vao);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);

    // 3. Populate buffer object.
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    // 4. Specify the memory layout.
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vec3D), (void*)0);
    glEnableVertexAttribArray(0);

    // 5. Unbind the objects.
    glBindVertexArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    /* Main loop */

    while (!glfwWindowShouldClose(window))
    {
        handle_inputs(window);

        glClear(GL_COLOR_BUFFER_BIT);

        // Note:
        // As the program gets more sophisticated and more shader programs and buffer objects are used,
        // glUseProgram() and glDrawArrays() allow us to swap these around as needed. In this example,
        // there is really no need to call these functions in each iteration of the rendering loop.

        glUseProgram(shader_program);
        
        float time_value = glfwGetTime();
        float green_value = (sin(time_value) * 0.5f) + 0.5f;
        // Finding the uniform location doesn't require a shader program to be in use ...
        i32 vertex_color_location = glGetUniformLocation(shader_program, "ourColor");
        // ... but updating the uniform does.
        // However, both can be done after the shader program is specified with glUseProgram().
        glUniform4f(vertex_color_location, 0.0f, green_value, 0.0f, 1.0f);

        glBindVertexArray(vao);
        glDrawArrays(GL_TRIANGLES, 0, 3);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwTerminate();
    return StatusCode::OK;
}
