#version 330 core
out vec4 FragColor;

in vec3 FragPos;

uniform vec3 rayOrigin;
uniform mat4 inverseProjection;
uniform mat4 inverseView;

vec3 rayDir;
float radius = 0.5;

vec3 lightDir = normalize(vec3(1, 1, -1));

void main()
{
	vec4 target = inverseProjection * vec4(FragPos.x, FragPos.y, 1, 1);
	rayDir = vec3(inverseView * vec4(normalize(vec3(target) / target.w), 0));
	float a = dot(rayDir, rayDir);
	float b = 2.0 * dot(rayOrigin, rayDir);
	float c = dot(rayOrigin, rayOrigin) - radius;

	float discriminant = b * b - 4.0 * a * c;
	float closestT = (-b - sqrt(discriminant)) / (2.0 * a);
	
	vec3 h0 = rayOrigin + rayDir * closestT;
	vec3 normal = normalize(h0);

	float diffuse = max(dot(normal, -lightDir), 0);
	vec3 sphereColor = vec3(1, 0.5, 0.2) * diffuse;

	if (discriminant >= 0.0)
	{
		FragColor = vec4(sphereColor,1);
	}
	else
	{
		FragColor = vec4(0,0,0,1);
	}
}