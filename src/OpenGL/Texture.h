#pragma once

#include <glad/glad.h>
#include <stb_image/stb_image.h>
#include <iostream>


class Texture
{
public:
	Texture(const char* path=NULL, GLenum textureIndex=GL_TEXTURE0, GLenum targetTexType=GL_TEXTURE_2D);
	void Bind() const;

private:
	unsigned int texId;
	GLenum targetTexType;
	GLenum textureIndex;
};