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

Framebuffer::Framebuffer(const int width, const int height, const int colAttIndex, const int texIndex)
	:m_texIndex(texIndex)
{
	GLCall(glGenFramebuffers(1, &m_fboID));
	GLCall(glBindFramebuffer(GL_FRAMEBUFFER, m_fboID));

	GLCall(glGenTextures(1, &m_texID));
	glActiveTexture(GL_TEXTURE0 + texIndex);
	GLCall(glBindTexture(GL_TEXTURE_2D, m_texID));

	GLCall(glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, width, height, 0, GL_RGBA, GL_FLOAT, NULL));
	GLCall(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR));
	GLCall(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR));
	GLCall(glBindTexture(GL_TEXTURE_2D, 0));

	GLCall(glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + colAttIndex, GL_TEXTURE_2D, m_texID, 0));

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
	GLCall(glBindFramebuffer(GL_FRAMEBUFFER, m_fboID));
}

void Framebuffer::UnBind() const
{
	GLCall(glBindFramebuffer(GL_FRAMEBUFFER, 0));
}

void Framebuffer::BindTex() const
{
	glActiveTexture(GL_TEXTURE0 + m_texIndex);
	GLCall(glBindTexture(GL_TEXTURE_2D, m_texID));
}

void Framebuffer::UnBindTex() const
{
	GLCall(glBindTexture(GL_TEXTURE_2D, 0));
}

void Framebuffer::copyBufferTex(unsigned int fboOut, unsigned int texOut, const int width, const int height)
{
	glBindFramebuffer(GL_READ_FRAMEBUFFER, m_fboID);
	glFramebufferTexture2D(GL_READ_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_texID, 0);
	glReadBuffer(GL_COLOR_ATTACHMENT0);

	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, fboOut);
	glFramebufferTexture2D(GL_DRAW_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D, texOut, 0);
	glDrawBuffer(GL_COLOR_ATTACHMENT1);

	glBlitFramebuffer(0, 0, width, height, 0, 0, width, height, GL_COLOR_BUFFER_BIT, GL_NEAREST);

	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D, 0, 0);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, 0, 0);
}