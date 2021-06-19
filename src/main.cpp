#include <iostream>
#include <string>
#include <vector>
#include <cstddef>
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <shaders/defaultshaders.hpp>
#include <shaders/shader.hpp>
#include <shaders/shaderprogram.hpp>

int main()
{
	GLFWwindow *window;
	if (!glfwInit())
	{
		printf("Could not init GLFW!\n");
		return 1;
	}

	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);

	window = glfwCreateWindow(640, 480, "Learn OpenGL", nullptr, nullptr);
	glfwMakeContextCurrent(window);

	if (glewInit() != GLEW_OK)
	{
		printf("Could not init GLEW!\n");
		return 1;
	}

	Shader default_vertex_shader{
			ShaderType::VertexShader,
			std::string{reinterpret_cast<char const *>(DEFAULT_VERTEX_SHADER_copy())},
			"default_vertex_shader"};

	Shader default_fragment_shader{
			ShaderType::FragmentShader,
			std::string{reinterpret_cast<char const *>(DEFAULT_FRAGMENT_SHADER_copy())},
			"default_fragment_shader"};

	ShaderProgram default_program{"default_shader_program"};
	default_vertex_shader.init();
	default_fragment_shader.init();
	default_program.init(default_vertex_shader.get_id(), default_fragment_shader.get_id());

	default_program.use();

	glfwSwapInterval(1); // vsync
	glClearColor(0.66, 0.66, 0.33, 1.0);

	GLuint vbo; // vertex buffer object (essentially an array of vertices)
	glGenBuffers(1, &vbo);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);

	/* Standard representation of a vertex */
	struct Vertex
	{
		f32 pos_x, pos_y; // X- and Y-Position
		f32 u, v;					// Texture coords
		u8 r, g, b, a;		// Color in RGBA
	};

	GLuint vao; // Vertex array object
	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);

	/* Tell OpenGL the layout of our Vertex struct, since OpenGL does not know by default how we represent our Vertex. */
	/* This is dependent on how the shader is implemented: layout (location = <index>) in <attribute_name> */
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void *)0);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void *)offsetof(Vertex, u));
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(2, 4, GL_UNSIGNED_BYTE, GL_TRUE, sizeof(Vertex), (void *)offsetof(Vertex, r));
	glEnableVertexAttribArray(2);

	std::vector<Vertex> triangle{
		{ 50.0f, 50.0f, 0.0f, 0.0f, 0, 0, 255, 255 },
		{ 50.0f, 100.0f, 0.0f, 0.0f, 0, 255, 0, 255 },
		{ 100.0f, 100.0f, 0.0f, 0.0f, 255, 0, 0, 255 }
	};

	glBufferData(GL_ARRAY_BUFFER, triangle.size() * sizeof(Vertex), triangle.data(), GL_STREAM_DRAW);

	while (!glfwWindowShouldClose(window))
	{
		int width, height;

		glfwPollEvents();
		glfwGetFramebufferSize(window, &width, &height);
		glViewport(0, 0, width, height);
		glClear(GL_COLOR_BUFFER_BIT);
		
		f32 left = 0.0f;
		f32 right = width;
		f32 top = 0.0;
		f32 bottom = height;

		f32 ortho[16] = {
				2.0f / (right - left), 0.0f, 0.0f, 0.0f,
				0.0f, 2.0f / (top - bottom), 0.0f, 0.0f,
				0.0f, 0.0f, -1.0f, 0.0f,
				-((right + left) / (right - left)), -((top + bottom) / (top - bottom)), 0.0f, 1.0f};

		glUniformMatrix4fv(glGetUniformLocation(default_program.get_id(), "our_proj"), 1, GL_FALSE, ortho);
		glDrawArrays(GL_TRIANGLES, 0, 3);

		glfwSwapBuffers(window);
	}

	glfwDestroyWindow(window);
	glfwTerminate();
}