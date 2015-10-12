#include "Camera.h"



Camera::Camera(vec3 target, vec3 up, vec3 position)
{
	m_target = target;
	m_up = up;
	m_position = position;

	m_view = lookAt(m_position, m_target + m_position, m_up);
	m_projection = perspectiveFov(90, 800, 600, 0, 10000);
}


Camera::~Camera()
{
}

mat4 Camera::GetView() 
{
	return m_view;
}
mat4 Camera::GetProj()
{
	return m_projection;
}
mat4 Camera::GetViewProj()
{
	return m_projection * m_view;
}

