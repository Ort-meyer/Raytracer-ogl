#version 430

uniform vec4 sphere;
uniform vec3 target;

//our frustum rays
uniform vec3 ray00;
uniform vec3 ray10;
uniform vec3 ray11;
uniform vec3 ray01;

uniform mat4 view;


layout(binding=0,rgba8) uniform image2D destTex;
layout (local_size_x = 16, local_size_y = 16) in;

struct box 
{
  vec3 min;
  vec3 max;
};

#define NUM_BOXES 2
const box boxes[] = 
{
  /* The ground */
  {vec3(-5.0, -0.1, -5.0), vec3(5.0, 0.0, 5.0)},
  /* Box in the middle */
  {vec3(-0.5, 0.0, -0.5), vec3(0.5, 1.0, 0.5)}
};

vec2 intersectBox(vec3 origin, vec3 dir, const box b) 
{
  vec3 tMin = (b.min - origin) / dir;
  vec3 tMax = (b.max - origin) / dir;
  vec3 t1 = min(tMin, tMax);
  vec3 t2 = max(tMin, tMax);
  float tNear = max(max(t1.x, t1.y), t1.z);
  float tFar = min(min(t2.x, t2.y), t2.z);
  return vec2(tNear, tFar);
}

void main()
{
	vec3 position = vec3((gl_GlobalInvocationID.xy), 0);

	//float t = RayVsSphere(direction, position, sphere);


	vec3 direction;
	float interpolationx = position.x/512;
	direction.x = mix(ray01.x, ray11.x, interpolationx);
	
	float interpolationy = position.y/512;
	direction.y = mix(ray01.y, ray00.y, interpolationy);
	
	float interpolationz = (interpolationx+interpolationy)/2;
	direction.z = target.z;
	//direction.z = 1.0f;
	direction = normalize(direction);


	//vec2 screenPosition = vec2(gl_GlobalInvocationID.x/800, gl_GlobalInvocationID.y/800);
	//
	////float interpolationValue = abs(vec2(0,0)-gl_GlobalInvocationID.xy)
	//vec3 interpolated00 = ray00 * (1/(length(vec2(0,0)-screenPosition)));
	//vec3 interpolated01 = ray01 * (1/(length(vec2(0,1)-screenPosition)));
	//vec3 interpolated11 = ray11 * (1/(length(vec2(1,1)-screenPosition)));
	//vec3 interpolated10 = ray10 * (1/(length(vec2(1,0)-screenPosition)));
	//
	//direction = (1/4) * interpolated00+interpolated01+interpolated11+interpolated10;



	//float t = RayVsSphere(direction, position, sphere);

	vec2 lambda = intersectBox(position, direction, boxes[0]);
	bool hit = lambda.x > 0.0 && lambda.x < lambda.y;

	ivec2 storePos = ivec2(gl_GlobalInvocationID.xy);
	vec4 color = vec4(0,0,direction.z, 0);
	//vec4 color = vec4(direction.x,0,0, 0);
	//vec4 color = vec4(0,direction.y,0, 0);
	imageStore(destTex, storePos, color);
}
























float RayIntersectSphere(vec3 ray, vec3 dir, vec3 center, float radius)
{
	vec3 rc = ray-center;
	float c = dot(rc, rc) - (radius*radius);
	float b = dot(dir, rc);
	float d = b*b - c;
	float t = -b - sqrt(abs(d));

	//float st = step(0.0, min(t,d));
	//return mix(-1.0, t, st);
	
	if (d < 0.0 || t < 0.0) 
	{
		return 1; // Didn't hit, or wasn't the closest hit
	}
	 
	else 
	{
		return t;
	}
}

float RayVsSphere(vec3 ray, vec3 pos, vec4 sphere)
{
	float t1;
	float t2;
	float b = dot(ray, pos-sphere.xyz);
	float c = dot(pos - sphere.xyz, pos - sphere.xyz) - pow(sphere.w, 2);

	bool hit = false;
	float f = pow(b, 2) - c;

	if(f >=0)
		hit = true;
	
	if(hit)
	{
		return 1;
	}
	return 0;


	//float t1;
	//float t2;
	//float b = ray.d.Dot(ray.o - sphere.c);
	//float c = (ray.o - sphere.c).Dot(ray.o - sphere.c) - powf(sphere.r, 2.0f);
	//bool hit;
	//float f = powf(b, 2.0f) - c;
	//hit = f >= 0.0f? true:false;
	//
	//if(hit)
	//{
	//	
	//	t1 = -b - sqrtf(f);
	//	t2 = -b + sqrtf(f);
	//	if(t1 < hitData.t || hitData.t < 0.0f)
	//	{
	//		hitData.t = t1;
	//		hitData.color = sphere.color;
	//	}
	//}
	//
	//return hit;
}

