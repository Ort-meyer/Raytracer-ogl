#pragma once

#include <glm\glm.hpp>
#include <glm\gtx\transform.hpp>
#include <glm\gtc\matrix_transform.hpp>
#include <GL\freeglut.h>
#define WIDTH 1024
#define HEIGHT 768

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

	void GetMouse(int x, int y);

private:
	//vec3 m_target;
	vec3 m_up;
	//vec3 m_position;

	mat4 m_view;
	mat4 m_projection;

	float m_rotateAngle, m_horizonalAngle, m_verticalAngle;
};

