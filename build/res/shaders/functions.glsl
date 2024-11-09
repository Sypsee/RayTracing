float random(vec2 st)
{
    return fract(sin(dot(st.xy, vec2(12.9898,78.233))) * 43758.5453123);
}

uint pcg_hash(uint input)
{
    uint state = input * 747796405u + 2891336453u;
    uint word = ((state >> ((state >> 28u) + 4u)) ^ state) * 277803737u;
    return (word >> 22u) ^ word;
}

float convertRange(float val, float min, float max)
{
	return val * (max - min) + min;
}

vec3 randomInUnitSphere(const vec3 rnd)
{
	return vec3
	(
		convertRange(rnd.x, -1.0, 1.0),
		convertRange(rnd.y, -1.0, 1.0),
		convertRange(rnd.z, -1.0, 1.0)
	);
}

vec3 skycolor(const vec3 dir)
{
	float a = 0.5 * (dir.y + 1);
	return (1.0 - a) * vec3(1.0, 1.0, 1.0) + a * vec3(0.5, 0.7, 1.0);
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
		}
	}

	return hitPayload;
}