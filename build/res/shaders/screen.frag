#version 460 core
out vec4 FragColor;

in vec3 FragPos;
in vec2 uv;

#define MAX_SPHERES 10
// #define MAX_MATERIALS 10
#define FLT_MAX 69696969
#define PI 3.14159

struct Material
{
	vec3 albedo;
	float smoothness;
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
	bool frontFace;
};

uniform Sphere u_spheres[MAX_SPHERES];
uniform Material u_materials[MAX_SPHERES];
uniform int u_sphereSize;

uniform vec3 u_rayOrigin;
uniform mat4 u_inverseProjection;
uniform mat4 u_inverseView;
uniform vec3 u_camRight;
uniform float u_time;
uniform vec2 u_screenRes;
uniform int u_frames;
uniform bool u_directOutput;
uniform sampler2D u_prevFrame;

uint g_randomSeed = 0u;

const int bounces = 10;
const int samples = 10;

void encryptTea(inout uvec2 arg)
{
	uvec4 key = uvec4(0xa341316c, 0xc8013ea4, 0xad90777d, 0x7e95761e);
	uint v0 = arg[0], v1 = arg[1];
	uint sum = 0u;
	uint delta = 0x9e3779b9u;

	for (int i = 0; i < 32; i++) {
	  sum += delta;
	  v0 += ((v1 << 4) + key[0]) ^ (v1 + sum) ^ ((v1 >> 5) + key[1]);
	  v1 += ((v0 << 4) + key[2]) ^ (v0 + sum) ^ ((v0 >> 5) + key[3]);
	}
	arg[0] = v0;
	arg[1] = v1;
}

uint wang_hash(inout uint seed)
{
    seed = (seed ^ 61) ^ (seed >> 16);
    seed *= 9;
    seed = seed ^ (seed >> 4);
    seed *= 0x27d4eb2d;
    seed = seed ^ (seed >> 15);
    return seed;
}

float getRandom()
{
	g_randomSeed += 1;
    return wang_hash(g_randomSeed) / 4294967295.0;
}

float convertRange(float val, float min, float max)
{
	return val * (max - min) + min;
}

vec3 randomInUnitSphere(const vec3 rnd)
{
	return normalize(vec3
	(
		convertRange(rnd.x, -1.0, 1.0),
		convertRange(rnd.y, -1.0, 1.0),
		convertRange(rnd.z, -1.0, 1.0)
	));
}

vec2 randomInCircle(const float radius)
{
	float angle = getRandom() * 2 * PI;
	vec2 pointOnCircle = vec2(cos(angle), sin(angle)) * radius;
	return pointOnCircle * sqrt(getRandom());
}

vec3 skycolor(const vec3 dir)
{
	float a = 0.5 * (dir.y + 1);
	return (1.0 - a) * vec3(1.0, 0.7, 0.77) + a * vec3(0.7, 0.9, 1.2);
}

float reflectance(float cosine, float refraction_index) {
	// Use Schlick's approximation for reflectance.
	float r0 = (1 - refraction_index) / (1 + refraction_index);
	r0 = r0*r0;
	return r0 + (1-r0)*pow((1 - cosine),5);
}

HitPayload closestSphereIntersection(Ray ray)
{
	HitPayload hitPayload;

	hitPayload.hitDistance = FLT_MAX;
	hitPayload.sphereIndex = -1; // Null check
	for (int i=0; i<u_sphereSize; i++)
	{
		vec3 origin = ray.origin - u_spheres[i].pos;
	
		float a = dot(ray.dir, ray.dir);
		float b = 2.0 * dot(origin, ray.dir);
		float c = dot(origin, origin) - u_spheres[i].radius * u_spheres[i].radius;

		float discriminant = b * b - 4.0 * a * c;
		if (discriminant < 0)
			continue;

		float closestT = (-b - sqrt(discriminant)) / (2.0 * a);
		if (closestT > 0 && closestT < hitPayload.hitDistance)
		{
			hitPayload.hitDistance = closestT;
			hitPayload.sphereIndex = i;

			vec3 origin = ray.origin - u_spheres[hitPayload.sphereIndex].pos;
			hitPayload.worldPos = origin + ray.dir * hitPayload.hitDistance;
			hitPayload.worldNormal = normalize(hitPayload.worldPos);
			hitPayload.worldPos += u_spheres[hitPayload.sphereIndex].pos;

			if (dot(ray.dir, hitPayload.worldNormal) > 0.0)
			{
				hitPayload.frontFace = false;
			}
			else
			{
				hitPayload.frontFace = true;
			}
		}
	}

	return hitPayload;
}

const float defocusStrength = 200.0;
const float divergeStrength = 0.2;
const vec3 camUp = vec3(0, 1, 0);

void main()
{
	Ray ray;
	vec2 defocusOffset = randomInCircle(1.0) * defocusStrength / u_screenRes.x;
	ray.origin = u_rayOrigin + u_camRight * defocusOffset.x + camUp * defocusOffset.y;
    vec2 offset = randomInCircle(1.0) * defocusOffset / u_screenRes.x;
	vec4 target = u_inverseProjection * vec4(vec2(FragPos) + offset, 1, 1);
	ray.dir = vec3(u_inverseView * vec4(normalize(vec3(target) / target.w), 0));
	
	vec3 light = vec3(0.0);
	vec3 contribution = vec3(1.0);

    g_randomSeed = uint((uv.y * u_screenRes.x + uv.x) + u_frames * 719393);

	for (int i=0; i<bounces; i++)
	{
		HitPayload result = closestSphereIntersection(ray);

		if (result.sphereIndex == -1 || result.hitDistance < 0)
		{
			light += skycolor(normalize(ray.dir)) * contribution;
			break;
		}
		
		vec3 rnd = vec3(getRandom(), getRandom(), getRandom());
		vec3 randomInUnitSphere = randomInUnitSphere(rnd);

		ray.origin = result.worldPos + result.worldNormal * 0.1;
		vec3 diffuseDir = vec3(1);
		Material material = u_materials[u_spheres[result.sphereIndex].matIndex];
		bool isSpecularBounce =  3 * material.smoothness >= convertRange(getRandom(), 1, 5);

		light += material.emission * contribution;
		if (material.refractiveIndex <= 0) // Is object diletric?
		{
			contribution *= mix(material.albedo, vec3(1), float(isSpecularBounce));
			diffuseDir = reflect(ray.dir, result.worldNormal + randomInUnitSphere);
		}
		else
		{
			contribution *= vec3(1);
			float ri = result.frontFace ? 1/material.refractiveIndex : material.refractiveIndex;
			float cosTheta = dot(ray.dir, result.worldNormal);
			float sinTheta = sqrt(1 - (cosTheta * cosTheta));

			if (ri * sinTheta > 1.0 || reflectance(cosTheta, ri) > 40 * getRandom())
			{
				diffuseDir = reflect(ray.dir, result.worldNormal + randomInUnitSphere);
			}
			else
			{
				diffuseDir = refract(ray.dir, result.worldNormal, ri);
			}
		}

		vec3 specularDir = reflect(ray.dir, result.worldNormal);
		ray.dir = mix(diffuseDir, specularDir, material.smoothness * float(isSpecularBounce));
	}

	vec3 finalColor = light;

	if (u_directOutput && u_frames > 0)
	{
		finalColor = texture(u_prevFrame, uv).rgb;
		float divider = float(u_frames);
		finalColor.x /= divider;
		finalColor.y /= divider;
		finalColor.z /= divider;
	}
	else if (u_frames > 0)
	{
		finalColor += texture(u_prevFrame, uv).rgb;
	}

	FragColor = vec4(finalColor, 1);
}