#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include "OpenGL/VertexBuffer.h"
#include "OpenGL/Texture.h"
#include "OpenGL/Shader.h"

#include "Application/Camera.h"

Camera cam(60, 0.1f, 100.0f);

void mouse_callback(GLFWwindow* m_Window, double xpos, double ypos)
{
	cam.mouse_callback(m_Window, xpos, ypos);
}

void framebuffer_size_callback(GLFWwindow* m_Window, int width, int height)
{
	glViewport(0, 0, width, height);
	cam.OnResize(width, height);
}

int main()
{
	if (!glfwInit())
	{
		std::cerr << "Error while initializing glfw!" << std::endl;
		return -1;
	}
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	GLFWwindow* window = glfwCreateWindow(600, 600, "Ray tracing", NULL, NULL);
	cam.OnResize(600, 600);

	if (!window)
	{
		std::cerr << "Error while creating window!" << std::endl;
		return -1;
	}

	glfwMakeContextCurrent(window);

	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		std::cerr << "Failed to initialize GLAD" << std::endl;
	}
	glViewport(0, 0, 600, 600);

	const float vertices[] = {
		-1.0F, -1.0F, 0.0F,
		1.0F, -1.0F, 0.0F,
		1.0F, 1.0F, 0.0F,
		1.0F, 1.0F, 0.0F,
		-1.0F, 1.0F, 0.0F,
		-1.0F, -1.0F, 0.0F
	};

	unsigned int VAO;
	glGenVertexArrays(1, &VAO);
	glBindVertexArray(VAO);

	VertexBuffer VBO(vertices, sizeof(vertices));
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);
	glEnableVertexAttribArray(0);

	Shader screenShader("res/shaders/screen.frag", "res/shaders/screen.vert");
	screenShader.Bind();

	glfwSetCursorPosCallback(window, mouse_callback);
	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

	float currentFrame = 0.f, lastFrame = 0.f;
	float deltaTime = 0.f;
	while (!glfwWindowShouldClose(window))
	{
		currentFrame = glfwGetTime();
		deltaTime = currentFrame - lastFrame;
		lastFrame = currentFrame;

		glClearColor(0.05f, 0.15f, 0.15f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);

		screenShader.setVec3("rayDirArray", 360000, cam.GetRayDirections()[0]);
		cam.OnUpdate(window, deltaTime);

		glBindVertexArray(VAO);
		glDrawArrays(GL_TRIANGLES, 0, 6);

		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	glfwDestroyWindow(window);
	glfwTerminate();
};