#include <iostream>
#include <string>
#include <cstdio>
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <util/types.hpp>

static const char* WINDOW_TITLE = "Learn OpenGL";

/* Exercises */
#define BEFORE_EXERCISES 0 // Code as it was before doing the exercises.
#define FIRST_EXERCISE   1 // Vertices for a second triangle added.
#define SECOND_EXERCISE  2 // Additional VAO and VBO added for the second triangle.
#define THIRD_EXERCISE   3 // Additional shader added for the second triangle.

// Switch between exercises here.
#define EXERCISE SECOND_EXERCISE

// Simple exception handling for the program.
enum StatusCode {
    OK = 0,
    GLFW_ERROR,
    GLEW_ERROR,
    SHADER_ERROR,
};

enum Shape {
    TRIANGLE = 0,
    HEXAGON,
    DEFAULT = TRIANGLE,
};

// Type safety for OpenGL shader enums.
enum ShaderType {
    VERTEX_SHADER = GL_VERTEX_SHADER,
    FRAGMENT_SHADER = GL_FRAGMENT_SHADER,
};

struct Vec3D {
    float x, y, z;
};

struct Color {
    float r, g, b, a;
};

// There probably is a better way to hold the program's state than through
// global variables. But for now, this suffices.

// Global state variable used to determine which shape should be drawn.
Shape g_drawn_shape = Shape::DEFAULT;

// Global state variables used for handling wireframe mode.
bool g_wire_mode = false, g_poll_w_key = true;

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

// FIXME: Using this function leads to an error while linking the shader program.
bool load_shader(GLuint* shader, const char* file_name, ShaderType shader_type)
{
    *shader = glCreateShader(shader_type);
    
    char* shader_source = load_shader_source(file_name);

    if (!shader_source)
    {
        free(shader_source);
        return false;
    }

    i32 compilation_successful;

    glShaderSource(*shader, 1, &shader_source, nullptr);
    glCompileShader(*shader);
    glGetShaderiv(*shader, GL_COMPILE_STATUS, &compilation_successful);
    free(shader_source);
    
    return compilation_successful;
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

    if (glfwGetKey(window, GLFW_KEY_1) == GLFW_PRESS)
        g_drawn_shape = Shape::TRIANGLE;

    if (glfwGetKey(window, GLFW_KEY_2) == GLFW_PRESS)
        g_drawn_shape = Shape::HEXAGON;

    // Toggle logic for enabling / disabling wireframe mode.
    u16 w_key_status = glfwGetKey(window, GLFW_KEY_W);
    if (g_poll_w_key && w_key_status == GLFW_PRESS)
    {
        g_poll_w_key = false;
        g_wire_mode = !g_wire_mode;
        glPolygonMode(GL_FRONT_AND_BACK, g_wire_mode ? GL_LINE : GL_FILL);
    }
    else if (w_key_status == GLFW_RELEASE)
        g_poll_w_key = true;
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

    // Load and compile vertex shader.
    // if (!load_shader(&vertex_shader, "src/shaders/vertex_shader.glsl", ShaderType::VERTEX_SHADER))
    // {
    //     glGetShaderInfoLog(vertex_shader, 512, nullptr, log);
    //     std::cout << "Could not load vertex shader." << std::endl;
    //     std::cout << log << std::endl;
    //     return StatusCode::SHADER_ERROR;
    // }

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

    // Load and compile fragment shader.
    // if (!load_shader(&fragment_shader, "src/shaders/fragment_shader.glsl", ShaderType::FRAGMENT_SHADER))
    // {
    //     glGetShaderInfoLog(fragment_shader, 512, nullptr, log);
    //     std::cout << "Could not load fragment shader." << std::endl;
    //     std::cout << log << std::endl;
    //     return StatusCode::SHADER_ERROR;
    // }

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

#if EXERCISE == 3

    fragment_shader_source = load_shader_source("src/shaders/fragment_shader_yellow.glsl");
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

    u32 shader_yellow = glCreateProgram();
    glAttachShader(shader_yellow, vertex_shader);
    glAttachShader(shader_yellow, fragment_shader);

    glLinkProgram(shader_yellow);
    glGetShaderiv(shader_program, GL_LINK_STATUS, &compilation_successful);
    if (!compilation_successful)
    {
        glGetProgramInfoLog(shader_program, 512, nullptr, log);
        std::cout << "Could not link shader program." << std::endl;
        std::cout << log << std::endl;
        return StatusCode::SHADER_ERROR;
    }

#endif

    // These aren't needed anymore since the shader program was created successfully
    // and can be unloaded.
    free(vertex_shader_source);
    glDeleteShader(vertex_shader);
    free(fragment_shader_source);
    glDeleteShader(fragment_shader);

    /* Create and populate vertex buffer. */    

    Vec3D vertices[] = {
        // hexagon
        {  0.0f,   0.0f,  0.0f }, // center
        { -0.66f,  0.0f,  0.0f }, // left
        { -0.33f,  0.75f, 0.0f }, // top left
        {  0.33f,  0.75f, 0.0f }, // top right
        {  0.66f,  0.0f,  0.0f }, // right
        {  0.33f, -0.75f, 0.0f }, // bottom right
        { -0.33f, -0.75f, 0.0f }, // bottom left
        // first triangle
        { -0.5f,  -0.5f,  0.0f }, // left
        {  0.5f,  -0.5f,  0.0f }, // right
        {  0.0f,   0.5f,  0.0f }, // top
        // second triangle
        {  0.3f,   0.3f,  0.0f }, // left
        {  0.9f,   0.3f,  0.0f }, // right
        {  0.6f,   0.8f,  0.0f }, // top
    };

    GLuint hexagon[] = {
        0, 1, 2,
        0, 2, 3,
        0, 3, 4,
        0, 4, 5,
        0, 5, 6,
        0, 6, 1,
    };

#if EXERCISE == 0 || EXERCISE == 1 || EXERCISE == 3

    GLuint vao, vbo, ebo;

    // 1. Create the objects.
    glGenVertexArrays(1, &vao);
    glGenBuffers(1, &vbo);
    glGenBuffers(1, &ebo);

    // 2. Bind the objects.
    glBindVertexArray(vao);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);

    // 3. Populate buffer object.
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(hexagon), hexagon, GL_STATIC_DRAW);

    // 4. Specify the memory layout.
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vec3D), (void*)0);
    glEnableVertexAttribArray(0);

    // 5. Unbind the objects.
    glBindVertexArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

#elif EXERCISE == 2

    GLuint vbo_array[2];
    GLuint* vbo_triangle1 = &vbo_array[0];
    GLuint* vbo_triangle2 = &vbo_array[1];

    GLuint vao_array[2];
    GLuint* vao_triangle1 = &vao_array[0];
    GLuint* vao_triangle2 = &vao_array[1];

    glGenVertexArrays(2, vao_array);
    glGenBuffers(2, vbo_array);

    glBindVertexArray(*vao_triangle1);
    glBindBuffer(GL_ARRAY_BUFFER, *vbo_triangle1);

    glBufferData(GL_ARRAY_BUFFER, 3 * sizeof(Vec3D), &vertices[7], GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vec3D), (void*)0);
    glEnableVertexAttribArray(0);

    glBindVertexArray(*vao_triangle2);
    glBindBuffer(GL_ARRAY_BUFFER, *vbo_triangle2);

    glBufferData(GL_ARRAY_BUFFER, 3 * sizeof(Vec3D), &vertices[10], GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vec3D), (void*)0);
    glEnableVertexAttribArray(0);

    glBindVertexArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

#endif

    /* Main loop */

    while (!glfwWindowShouldClose(window))
    {
        handle_inputs(window);

        glClear(GL_COLOR_BUFFER_BIT);

        // As the program gets more sophisticated and more shader programs and buffer objects are used,
        // glUseProgram() and glDrawArrays() allow us to swap these around as needed. In this example,
        // there is really no need to call these functions in each iteration of the rendering loop.
        glUseProgram(shader_program);

#if EXERCISE == 0

        glBindVertexArray(vao);

        switch(g_drawn_shape)
        {
            case Shape::TRIANGLE:
                // Draw the triangle.
                glDrawArrays(GL_TRIANGLES, 7, 3);
                break;
            case Shape::HEXAGON:
                // Draw the hexagon.
                glDrawElements(GL_TRIANGLES, 18, GL_UNSIGNED_INT, 0);
                break;
        }

#elif EXERCISE == 1

        glBindVertexArray(vao);

        switch(g_drawn_shape)
        {
            case Shape::TRIANGLE:
                // Draw the triangles.
                glDrawArrays(GL_TRIANGLES, 7, 3);
                glDrawArrays(GL_TRIANGLES, 10, 3);
                break;
            case Shape::HEXAGON:
                // Draw the hexagon.
                glDrawElements(GL_TRIANGLES, 18, GL_UNSIGNED_INT, 0);
                break;
        }

#elif EXERCISE == 2

            // Draw the triangles.
            glBindVertexArray(*vao_triangle1);
            glDrawArrays(GL_TRIANGLES, 0, 3);
            glBindVertexArray(*vao_triangle2);
            glDrawArrays(GL_TRIANGLES, 0, 3);

#elif EXERCISE == 3

        glBindVertexArray(vao);

        switch(g_drawn_shape)
        {
            case Shape::TRIANGLE:
                // Draw the triangles.
                glDrawArrays(GL_TRIANGLES, 7, 3);
                glUseProgram(shader_yellow);                
                glDrawArrays(GL_TRIANGLES, 10, 3);
                break;
            case Shape::HEXAGON:
                // Draw the hexagon.
                glDrawElements(GL_TRIANGLES, 18, GL_UNSIGNED_INT, 0);
                break;
        }

#endif

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwTerminate();
    return StatusCode::OK;
}
