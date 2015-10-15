#pragma once

#include <glm\glm.hpp>
#include <glm\gtx\transform.hpp>
#include <glm\gtc\matrix_transform.hpp>

using namespace glm;
using namespace std;

struct RayFrustum
{
	vec3 ray10;
	vec3 ray11;
	vec3 ray01;
	vec3 ray00;
};

class Camera
{
public:
	Camera(vec3 target, vec3 up, vec3 position);
	~Camera();
	
	void Update();

	mat4 GetView();
	mat4 GetProj();
	mat4 GetViewProj();

	vec3 m_target;
	vec3 m_position;
	RayFrustum m_frustum;



private:
	//vec3 m_target;
	vec3 m_up;
	//vec3 m_position;

	mat4 m_view;
	mat4 m_projection;

	float m_rotateAngle;

};

