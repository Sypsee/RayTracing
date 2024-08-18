#include "FrameBuffer.h"

#include <iostream>

Framebuffer::Framebuffer(const int widht, const int height)
{
	glGenFramebuffers(1, &m_fboID);
	glBindFramebuffer(GL_FRAMEBUFFER, m_fboID);

	glGenTextures(1, &m_texID);
	glBindTexture(GL_TEXTURE_2D, m_texID);

	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, 800, 600, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_TEXTURE_2D, m_texID, 0);

	glGenRenderbuffers(1, &m_RboID);
	glBindRenderbuffer(GL_RENDERBUFFER, m_RboID);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, 800, 600);
	glBindRenderbuffer(GL_RENDERBUFFER, 0);

	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, m_RboID);

	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
	{
		std::cerr << "Error while creating frame buffer!\n";
	}
}

Framebuffer::~Framebuffer()
{
	glDeleteFramebuffers(1, &m_fboID);
	glDeleteRenderbuffers(1, &m_RboID);
	glDeleteTextures(1, &m_texID);
}

void Framebuffer::Bind() const
{
	glBindFramebuffer(GL_FRAMEBUFFER, m_fboID);
	glBindTexture(GL_TEXTURE_2D, m_texID);
}

void Framebuffer::UnBind() const
{
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}
