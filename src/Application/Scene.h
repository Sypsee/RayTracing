#pragma once

#include <glm/glm.hpp>
#include <vector>

struct Material
{
	glm::vec3 Albedo{ 1.0f };
	float Smoothness = 0.0f;
    float refractiveIndex = 0.0f;
    float EmissionPower = 0.0f;
    glm::vec3 EmissionColor{ 0.0f };

    glm::vec3 getEmission() {return EmissionColor * EmissionPower;}
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
