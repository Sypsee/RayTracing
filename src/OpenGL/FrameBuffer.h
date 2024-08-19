#pragma once

#include <glad/glad.h>

class Framebuffer
{
public:
	Framebuffer(const int width, const int height);
	~Framebuffer();

	void Bind() const;
	void UnBind() const;
	void WriteTex(const int width, const int height);
	void ClearTex(const int width, const int height);

private:
	unsigned int m_texID, m_fboID;
};