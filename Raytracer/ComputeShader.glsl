#version 430

uniform mat4 viewProj;
uniform vec4 sphere;
//writeonly uniform image2D destTex;
layout(binding=0,rgba8) uniform image2D destTex;
layout (local_size_x = 16, local_size_y = 16) in;

float RayIntersectSphere(vec3 ray, vec3 dir, vec3 center, float radius)
{
	vec3 rc = ray-center;
	float c = dot(rc, rc) - (radius*radius);
	float b = dot(dir, rc);
	float d = b*b - c;
	float t = -b - sqrt(abs(d));

	float st = step(0.0, min(t,d));
	return mix(-1.0, t, st);
	
	//if (d < 0.0 || t < 0.0 || t > closestHit) 
	//{
	//	return closestHit; // Didn't hit, or wasn't the closest hit.
	//}
	// 
	//else 
	//{
	//	return t;
	//}
}

void main()
{
	vec3 position = vec3((gl_GlobalInvocationID.xy), 0);
	vec4 direction4 = viewProj * vec4(0,0,1,0);
	vec3 direction = direction4.xyz;
	ivec2 storePos = ivec2(gl_GlobalInvocationID.xy);

	float t = RayIntersectSphere(position, direction, sphere.xyz, sphere.w);
	imageStore(destTex, storePos, vec4(1,1,0,1));
}