#include "FrameBuffer.h"

#include <iostream>

Framebuffer::Framebuffer(const int width, const int height)
{
	glGenFramebuffers(1, &m_fboID);
	glBindFramebuffer(GL_FRAMEBUFFER, m_fboID);

	glGenTextures(1, &m_texID);
	glBindTexture(GL_TEXTURE_2D, m_texID);

	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, width, height, 0, GL_RGBA, GL_FLOAT, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glBindTexture(GL_TEXTURE_2D, 0);

	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_texID, 0);

	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
	{
		std::cerr << "Error while creating frame buffer!\n";
	}

	glBindFramebuffer(m_fboID, 0);
}

Framebuffer::~Framebuffer()
{
	glDeleteFramebuffers(1, &m_fboID);
	glDeleteTextures(1, &m_texID);
}

void Framebuffer::Bind() const
{
	glBindFramebuffer(GL_FRAMEBUFFER, m_fboID);
}

void Framebuffer::UnBind() const
{
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}
