#include "Texture.h"
#include <string>

Texture::Texture(const char* path, GLenum textureIndex, GLenum targetTexType)
	:targetTexType(targetTexType), textureIndex(textureIndex)
{
	glGenTextures(1, &texId);
	glActiveTexture(textureIndex);
	glBindTexture(targetTexType, texId);

	glTexParameteri(targetTexType, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(targetTexType, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(targetTexType, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(targetTexType, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	
	int width, height, nrChannels;
	stbi_set_flip_vertically_on_load(true);
	unsigned char* data = stbi_load(path, &width, &height, &nrChannels, 0);
	if (data || data == NULL)
	{
		glTexImage2D(targetTexType, 0, GL_RGB32F, width, height, 0, GL_RGB, GL_FLOAT, data);
		glGenerateMipmap(targetTexType);
	}
	else
	{
		std::cout << "Failed to load texture" << std::endl;
	}
	stbi_image_free(data);
}

void Texture::Bind() const
{
	glActiveTexture(textureIndex);
	glBindTexture(targetTexType, texId);
}