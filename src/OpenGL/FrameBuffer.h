#pragma once

#include <glad/glad.h>

class Framebuffer
{
public:
	Framebuffer(const int width, const int height);
	~Framebuffer();

	void Bind() const;
	void UnBind() const;

private:
	unsigned int m_texID, m_fboID;
};