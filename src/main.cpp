#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include "OpenGL/VertexBuffer.h"
#include "OpenGL/Shader.h"
#include "OpenGL/Framebuffer.h"

#include "Application/Camera.h"
#include "Application/Scene.h"

#include <iostream>
#include <string>

Camera cam(60, 0.1f, 100.0f);

int WIDTH = 1152, HEIGHT = 648;

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
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
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
        -1.0F, -1.0F, 0.0F, 0.0F, 0.0F,
        1.0F, -1.0F, 0.0F,  1.0F, 0.0F,
        1.0F, 1.0F, 0.0F,   1.0F, 1.0F,
        1.0F, 1.0F, 0.0F,   1.0F, 1.0F,
        -1.0F, 1.0F, 0.0F,  0.0F, 1.0F,
        -1.0F, -1.0F, 0.0F, 0.0F, 0.0F
    };

	unsigned int VAO;
	glGenVertexArrays(1, &VAO);
	glBindVertexArray(VAO);

	VertexBuffer VBO(vertices, sizeof(vertices));
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);

	Shader screenShader;
    screenShader.AttachShader({"res/shaders/screen.frag", GL_FRAGMENT_SHADER});
    screenShader.AttachShader({"res/shaders/screen.vert", GL_VERTEX_SHADER});
	screenShader.Bind();
	screenShader.setI("u_prevFrame", 0);

    Framebuffer fb
    (
        Framebuffer::CreateInfo{
            std::span<const Framebuffer::Attachment>{
                std::array<Framebuffer::Attachment, 1>{
                    Framebuffer::Attachment{GL_COLOR_ATTACHMENT0, WIDTH, HEIGHT}
                }
            }
        }
    );
    fb.bindTex(0);

	glfwSetCursorPosCallback(window, mouse_callback);
	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

	int numAccumulatedFrames = 0;

	Sphere sphere;
	sphere.radius = 1;
	sphere.matIndex = 0;
	Sphere sphere2;
	sphere2.matIndex = 1;
	sphere2.Pos = glm::vec3(0, -101, 0);
	sphere2.radius = 100;
	Sphere sphere3;
	sphere3.matIndex = 2;
	sphere3.Pos = glm::vec3(35, 4, -32);
	sphere3.radius = 20;

	Material mat;
	mat.Albedo = glm::vec3(0.6, 0.7, 1);
    mat.refractiveIndex = 1.33;
    // mat.Smoothness = 1.0;

	Material mat2;
	mat2.Albedo = glm::vec3(0.5, 0.3, 0.2);
    mat2.Smoothness = 0.1;

	Material mat3;
	mat3.Albedo = glm::vec3(0.8, 0.5, 0.2);
    mat3.EmissionColor = glm::vec3(0.8, 0.5, 0.2);
    // mat3.EmissionColor = glm::vec3(1.0);
    mat3.EmissionPower = 5;

	Scene scene;
	scene.spheres.push_back(sphere);
	scene.spheres.push_back(sphere2);
	scene.spheres.push_back(sphere3);
	scene.materials.push_back(mat);
	scene.materials.push_back(mat2);
	scene.materials.push_back(mat3);

	float currentFrame = 0.f, lastFrame = 0.f;
	float deltaTime = 0.f;
	while (!glfwWindowShouldClose(window))
	{
		currentFrame = glfwGetTime();
		deltaTime = currentFrame - lastFrame;
		lastFrame = currentFrame;

		// system("clear"); // temp
		// std::cout << deltaTime * 1000 << "ms\n";
        
        screenShader.Bind();
		screenShader.setMat4("u_inverseProjection", cam.GetInverseProjection());
		screenShader.setMat4("u_inverseView", cam.GetInverseView());
		screenShader.setVec3("u_rayOrigin", cam.GetPosition());
        screenShader.setVec3("u_camRight", glm::cross(cam.GetDirection(), glm::vec3(0,1,0)));
		screenShader.setVec2("u_screenRes", glm::vec2(WIDTH, HEIGHT));

		screenShader.setF("u_time", currentFrame);

		// Scene rendering
		screenShader.setI("u_sphereSize", scene.spheres.size());

		for (int i = 0; i < scene.spheres.size(); i++)
		{
			std::string sphereIndex = "u_spheres[";
			sphereIndex += std::to_string(i);
			sphereIndex += "]";

			std::string matIndex = "u_materials[";
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
				std::string matAlbedo = matIndex + ".albedo";
				screenShader.setVec3(matAlbedo.c_str(), scene.materials[i].Albedo);
			}
			{
				std::string matRefractiveIndex = matIndex + ".refractiveIndex";
				screenShader.setF(matRefractiveIndex.c_str(), scene.materials[i].refractiveIndex);
			}
			{
				std::string matEmission = matIndex + ".emission";
				screenShader.setVec3(matEmission.c_str(), scene.materials[i].getEmission());
			}
			{
				std::string matSmoothness = matIndex + ".smoothness";
				screenShader.setF(matSmoothness.c_str(), scene.materials[i].Smoothness);
			}
			{
				std::string sphereRadius = sphereIndex + ".radius";
				screenShader.setF(sphereRadius.c_str(), scene.spheres[i].radius);
			}
		}

        fb.bind();
        screenShader.setI("u_directOutput", 0);
        glBindVertexArray(VAO);
        glDrawArrays(GL_TRIANGLES, 0, 6);
        fb.unbind();

		numAccumulatedFrames++;
		screenShader.setI("u_frames", numAccumulatedFrames);
        screenShader.setI("u_directOutput", 1);

		if (cam.OnUpdate(window, deltaTime))
		{
			numAccumulatedFrames = 0;
			screenShader.setI("u_frames", numAccumulatedFrames);
		}

		glBindVertexArray(VAO);
		glDrawArrays(GL_TRIANGLES, 0, 6);
		
		glfwSwapBuffers(window);
		glfwPollEvents();
	}

    glDeleteVertexArrays(1, &VAO);
 
	glfwDestroyWindow(window);
	// glfwTerminate(); // let openGL objs get destroyed
};
