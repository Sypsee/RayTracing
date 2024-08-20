#include "FrameBuffer.h"

#include <iostream>

#define ASSERT(x) if (!(x)) __debugbreak();
#define GLCall(x) GLClearError();\
	x;\
	ASSERT(GLLogCall(#x, __FILE__, __LINE__))

static void GLClearError()
{
	while (glGetError() != GL_NO_ERROR);
}

static bool GLLogCall(const char* function, const char* file, int line)
{
	while (GLenum error = glGetError())
	{
		std::cout << "[OpenGL Error] (" << error << "): " << function << " " << file << "\n";
		return false;
	}

	return true;
}

Framebuffer::Framebuffer(const int width, const int height)
{
	GLCall(glGenFramebuffers(1, &m_fboID));
	GLCall(glBindFramebuffer(GL_FRAMEBUFFER, m_fboID));

	GLCall(glGenTextures(1, &m_texID));
	GLCall(glBindTexture(GL_TEXTURE_2D, m_texID));

	GLCall(glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, width, height, 0, GL_RGBA, GL_FLOAT, NULL));
	GLCall(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR));
	GLCall(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR));
	GLCall(glBindTexture(GL_TEXTURE_2D, 0));

	GLCall(glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_texID, 0));

	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
	{
		std::cerr << "Error while creating frame buffer!\n";
	}

	GLCall(glBindFramebuffer(GL_FRAMEBUFFER, 0));
}

Framebuffer::~Framebuffer()
{
	glDeleteFramebuffers(1, &m_fboID);
	glDeleteTextures(1, &m_texID);
}

void Framebuffer::Bind() const
{
	GLCall(glBindTexture(GL_TEXTURE_2D, m_texID));
	GLCall(glBindFramebuffer(GL_FRAMEBUFFER, m_fboID));
}

void Framebuffer::UnBind() const
{
	GLCall(glBindTexture(GL_TEXTURE_2D, 0));
	GLCall(glBindFramebuffer(GL_FRAMEBUFFER, 0));
}
