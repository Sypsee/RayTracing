#version 330 core
out vec4 FragColor;

in vec3 FragPos;

#define MAX_SPHERES 10
// #define MAX_MATERIALS 10
#define FLT_MAX 69696969

struct Material
{
	vec3 albedo;
	float roughness;
	float metallic;
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

uniform Sphere spheres[MAX_SPHERES];
uniform Material materials[MAX_SPHERES];
uniform int sphereSize;

uniform vec3 rayOrigin;
uniform mat4 inverseProjection;
uniform mat4 inverseView;
uniform float time;
uniform vec2 screenRes;
uniform int num_accumulated_frames;
uniform sampler2D prevFrame;

vec3 lightDir = normalize(vec3(1, -1, -1));

float random (vec2 st) {
    return fract(sin(dot(st.xy, vec2(12.9898,78.233)))* 43758.5453123);
}

HitPayload closestSphereIntersection(Ray ray)
{
	HitPayload hitPayload;

	hitPayload.hitDistance = FLT_MAX;
	hitPayload.sphereIndex = -1; // Null check
	for (int i=0; i<sphereSize; i++)
	{
		vec3 origin = ray.origin - spheres[i].pos;
	
		float a = dot(ray.dir, ray.dir);
		float b = 2.0 * dot(origin, ray.dir);
		float c = dot(origin, origin) - spheres[i].radius * spheres[i].radius;

		float discriminant = b * b - 4.0 * a * c;
		if (discriminant < 0)
			continue;

		float closestT = (-b - sqrt(discriminant)) / (2.0 * a);
		if (closestT > 0 && closestT < hitPayload.hitDistance)
		{
			hitPayload.hitDistance = closestT;
			hitPayload.sphereIndex = i;

			vec3 origin = ray.origin - spheres[hitPayload.sphereIndex].pos;
			hitPayload.worldPos = origin + ray.dir * hitPayload.hitDistance;
			hitPayload.worldNormal = normalize(hitPayload.worldPos);
			hitPayload.worldPos += spheres[hitPayload.sphereIndex].pos;
		}
	}

	return hitPayload;
}

void main()
{
	Ray ray;
	ray.origin = rayOrigin;
	vec4 target = inverseProjection * vec4(FragPos.x, FragPos.y, 1, 1);
	ray.dir = vec3(inverseView * vec4(normalize(vec3(target) / target.w), 0));
	
	vec3 finalColor;

	float multiplier = 1.0;
	int bounces = 5;
	for (int i=0; i<bounces; i++)
	{
		HitPayload result = closestSphereIntersection(ray);

		if (result.sphereIndex == -1 || result.hitDistance < 0)
		{
			finalColor += vec3(0,0,0) * multiplier;
			break;
		}

		float diffuse = max(dot(result.worldNormal, -lightDir), 0);
		vec3 sphereColor = materials[spheres[result.sphereIndex].matIndex].albedo * diffuse;
		finalColor += sphereColor * multiplier;

		multiplier *= 0.5;

		float rnd = random(vec2(FragPos.x + time * 0.5, FragPos.y + time * 0.5));
		rnd = rnd * 1 - 0.5;

		ray.origin = result.worldPos + result.worldNormal * 0.0001;
		ray.dir = reflect(ray.dir, result.worldNormal + materials[spheres[result.sphereIndex].matIndex].roughness * rnd);
	}

	if (num_accumulated_frames > 0)
	{
		finalColor = mix(texture(prevFrame, FragPos.xy / screenRes).xyz, finalColor, 1/num_accumulated_frames);
	}

	FragColor = texture(prevFrame, FragPos.xy / screenRes);
}