#include <iostream>
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <GLFW/glfw3.h>

enum ErrorTypes {
    OK,
    GLFW_ERROR,
    GLEW_ERROR,
};

void handle_resize(GLFWwindow* window, int width, int height)
{
    glViewport(0, 0, width, height);
}

void handle_inputs(GLFWwindow* window)
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);
}

int main()
{
    std::cout << "Hello, window!" << std::endl;

    GLFWwindow* window;

    if(!glfwInit())
    {
        std::cout << "Could not initialize GLFW." << std::endl;
        return -GLFW_ERROR;
    }

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    window = glfwCreateWindow(800, 600, "Learn OpenGL", nullptr, nullptr);
    if (window == nullptr)
    {
        std::cout << "Could not create window." << std::endl;
        glfwTerminate();
        return -GLFW_ERROR;
    }
    glfwMakeContextCurrent(window);

    if (!glewInit != GLEW_OK)
    {
        std::cout << "Could not initialize GLEW." << std::endl;
        return -GLEW_ERROR;
    }

    glViewport(0, 0, 800, 600);
    glfwSetFramebufferSizeCallback(window, handle_resize);

    glClearColor(0.0f, 0.9f, 0.5f, 1.0f);

    while (!glfwWindowShouldClose(window))
    {
        handle_inputs(window);

        /* === render in here === */

        glClear(GL_COLOR_BUFFER_BIT);

        /* ====================== */

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwTerminate();
    return OK;
}
