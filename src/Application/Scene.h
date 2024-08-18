#pragma once

#include <glm/glm.hpp>
#include <vector>

struct Material
{
	glm::vec3 Albedo{ 1.0f };
	float Roughness = 1.0f;
	float Metallic = 0.0f;
};

struct Sphere
{
	glm::vec3 Pos{ 0.0f };
	float radius = 0.5f;

	int matIndex;
};

struct Scene
{
	std::vector<Sphere> spheres;
	std::vector<Material> materials;
};