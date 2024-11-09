struct Material
{
	vec3 albedo;
	float roughness;
	float metallic;
	float refractiveIndex;
	vec3 emission;
};

struct Sphere
{
	vec3 pos;
	float radius;

	int matIndex;
};

struct Ray
{
	vec3 dir;
	vec3 origin;
};

struct HitPayload
{
	int sphereIndex;
	float hitDistance;

	vec3 worldPos;
	vec3 worldNormal;
};