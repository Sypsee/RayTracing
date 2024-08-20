#pragma once

#include <glad/glad.h>

class Framebuffer
{
public:
	Framebuffer(const int width, const int height, const int colAttIndex = 0, const int texIndex = 0);
	~Framebuffer();

	void Bind() const;
	void UnBind() const;
	void BindTex() const;
	void UnBindTex() const;

	void copyBufferTex(unsigned int fboOut, unsigned int texOut, const int width, const int height);

	inline unsigned int getFboID() { return m_fboID; }
	inline unsigned int getTexID() { return m_texID; }

private:
	unsigned int m_texID, m_fboID, m_texIndex;
};