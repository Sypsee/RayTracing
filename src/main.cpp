#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include "OpenGL/VertexBuffer.h"
#include "OpenGL/Texture.h"
#include "OpenGL/Shader.h"
#include "OpenGL/FrameBuffer.h"

#include "Application/Camera.h"
#include "Application/Scene.h"

Camera cam(60, 0.1f, 100.0f);

int WIDTH = 800, HEIGHT = 600;

void mouse_callback(GLFWwindow* m_Window, double xpos, double ypos)
{
	cam.mouse_callback(m_Window, xpos, ypos);
}

void framebuffer_size_callback(GLFWwindow* m_Window, int width, int height)
{
	glViewport(0, 0, width, height);
	cam.OnResize(width, height);
	WIDTH = width;
	HEIGHT = height;
}

int main()
{
	if (!glfwInit())
	{
		std::cerr << "Error while initializing GLFW!" << std::endl;
		return -1;
	}
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	GLFWwindow* window = glfwCreateWindow(WIDTH, HEIGHT, "Ray tracing", NULL, NULL);
	cam.OnResize(WIDTH, HEIGHT);

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
	glViewport(0, 0, WIDTH, HEIGHT);

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

	Framebuffer fbo(WIDTH, HEIGHT);
	bool isLastFrame=true;
	int numAccumulatedFrames=0;

	Sphere sphere;
	sphere.radius = 1;
	sphere.matIndex = 0;
	Sphere sphere2;
	sphere.matIndex = 1;
	sphere2.Pos = glm::vec3(0, -11, 0);
	sphere2.radius = 10;

	Material mat;
	mat.Albedo = glm::vec3(0.6, 0.7, 1);
	mat.Roughness = 1;

	Material mat2;
	mat2.Albedo = glm::vec3(0.5, 0.3, 0.2);
	mat2.Roughness = 0.1;

	Scene scene;
	scene.spheres.push_back(sphere);
	scene.spheres.push_back(sphere2);
	scene.materials.push_back(mat);
	scene.materials.push_back(mat2);

	float currentFrame = 0.f, lastFrame = 0.f;
	float deltaTime = 0.f;
	while (!glfwWindowShouldClose(window))
	{
		currentFrame = glfwGetTime();
		deltaTime = currentFrame - lastFrame;
		lastFrame = currentFrame;

		std::cout << deltaTime << "\n";

		screenShader.setMat4("inverseProjection", cam.GetInverseProjection());
		screenShader.setMat4("inverseView", cam.GetInverseView());
		screenShader.setVec3("rayOrigin", cam.GetPosition());
		screenShader.setI("num_accumulated_frames", numAccumulatedFrames);
		screenShader.setI("prevFrame", 0);
		screenShader.setVec2("screenRes", glm::vec2(WIDTH, HEIGHT));

		screenShader.setF("time", currentFrame);

		// Scene rendering
		screenShader.setI("sphereSize", scene.spheres.size());

		for (int i = 0; i < scene.spheres.size(); i++)
		{
			std::string sphereIndex = "spheres[";
			sphereIndex += std::to_string(i);
			sphereIndex += "]";

			std::string matIndex = "materials[";
			matIndex += std::to_string(i);
			matIndex += "]";

			{
				std::string spherePos = sphereIndex + ".pos";
				screenShader.setVec3(spherePos.c_str(), scene.spheres[i].Pos);
			}
			{
				std::string matIndex = sphereIndex + ".matIndex";
				screenShader.setI(matIndex.c_str(), scene.spheres[i].matIndex);
			}
			{
				std::string matRoughness = matIndex + ".albedo";
				screenShader.setVec3(matRoughness.c_str(), scene.materials[i].Albedo);
			}
			{
				std::string matRoughness = matIndex + ".roughness";
				screenShader.setF(matRoughness.c_str(), scene.materials[i].Roughness);
			}
			{
				std::string matMetallic = matIndex + ".metallic";
				screenShader.setF(matMetallic.c_str(), scene.materials[i].Metallic);
			}
			{
				std::string sphereRadius = sphereIndex + ".radius";
				screenShader.setF(sphereRadius.c_str(), scene.spheres[i].radius);
			}
		}

		if (isLastFrame)
		{
			isLastFrame = false;
			numAccumulatedFrames++;

			fbo.WriteTex(WIDTH, HEIGHT);

			glBindVertexArray(VAO);
			glDrawArrays(GL_TRIANGLES, 0, 6);

			fbo.UnBind();
		}
		else
		{
			isLastFrame = true;
		}

		glClearColor(0.05f, 0.15f, 0.15f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);

		if (cam.OnUpdate(window, deltaTime))
		{
			numAccumulatedFrames = 1;
			fbo.ClearTex(WIDTH, HEIGHT);
		}

		glBindVertexArray(VAO);
		glDrawArrays(GL_TRIANGLES, 0, 6);

		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	glfwDestroyWindow(window);
	glfwTerminate();
};