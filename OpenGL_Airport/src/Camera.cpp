#include "stdafx.h"
#include "Camera.h"
#include <algorithm>

void Camera::ComputeViewMatrixUsingLookAt()
{
	float upX = 0;
	float upY = 1;
	float upZ = 0;

	///< Lock angle in range -180 to +180 to prevent camera flipping.
	if (m_angleYZ < -90)
	{
		upY = -1;
	}

	vec3 forward = vec3(m_directionX, m_directionY, m_directionZ);
	vec3 up = vec3(upX, upY, upZ);
	vec3 forwardNormalised = normalise(forward); ///< normalize(forward);
	vec3 side = vec3(forwardNormalised); ///< copy cross(forward, up, side);
	side = cross(side, up);
	vec3 sideNormalised = normalise(side); ///< normalize(side);
	up = vec3(sideNormalised);
	up = cross(up, forwardNormalised); ///< no need to normalise as generated from 2 normalised vectors of unit length

	/** now we build the matrix... (column major)
	* this is the transpose for the V matrix as  Vinverse == VT for rotation matrices
	*/

	/** row 0 */
	m_viewMatrix.m[0] = sideNormalised.v[0];
	m_viewMatrix.m[4] = sideNormalised.v[1];
	m_viewMatrix.m[8] = sideNormalised.v[2];
	m_viewMatrix.m[12] = 0;

	/** row 1 */
	m_viewMatrix.m[1] = up.v[0];
	m_viewMatrix.m[5] = up.v[1];
	m_viewMatrix.m[9] = up.v[2];
	m_viewMatrix.m[13] = 0;

	/** row 2 */
	m_viewMatrix.m[2] = -(forwardNormalised.v[0]);
	m_viewMatrix.m[6] = -(forwardNormalised.v[1]);
	m_viewMatrix.m[10] = -(forwardNormalised.v[2]);
	m_viewMatrix.m[14] = 0;

	/** row 3 */
	m_viewMatrix.m[3] = 0;
	m_viewMatrix.m[7] = 0;
	m_viewMatrix.m[11] = 0;
	m_viewMatrix.m[15] = 1;

	mat4 cameraTranslation = translate(identity_mat4(), vec3(-m_x, -m_y, -m_z)); ///< now apply translation to camera position
	m_viewMatrix = m_viewMatrix * cameraTranslation;  ///< pre or post multiply - - definitely POST!!!
}

void Camera::ComputeDirectionVector()
{
	float angleYZRadians = 3.14156f * m_angleYZ / 180.0f;
	float angleXZRadians = 3.14156f * m_angleXZ / 180.0f;
	float m_other = cos(angleYZRadians);

	m_directionY = sin(angleYZRadians);
	m_directionX = m_other * sin(angleXZRadians);
	m_directionZ = m_other * -cos(angleXZRadians);
}

void Camera::ComputeRightAngleDirectionVector()
{
	vec3 forward = vec3(m_directionX, m_directionY, m_directionZ);
	vec3 right = cross(forward, vec3(0, 1, 0));

	right = normalise(right);

	m_directionRightX = right.v[0];
	m_directionRightY = right.v[1];
	m_directionRightZ = right.v[2];
}

void Camera::UpdateCameraPosition(float distanceMovedForward, float distanceMovedSide)
{
	// later might use matrices to calculate this....
	// for now use angle in xz and stay on ground....
	// if moved update position

	m_x += (distanceMovedForward * m_directionX) + (distanceMovedSide * m_directionRightX);
	m_y += (distanceMovedForward * m_directionY) + (distanceMovedSide * m_directionRightY);
	m_z += (distanceMovedForward * m_directionZ) + (distanceMovedSide * m_directionRightZ);
}

void Camera::SetViewMatrix(GLuint program)
{
	Win32OpenGL::SendUniformMatrixToShader(program, m_viewMatrix.m, "view_matrix");
}

void Camera::HandleKeyInput()
{
	float distanceMovedForward = 0.0f;
	float distanceMovedSide = 0.0f;
	float angleStep = 2.5f;

	if (m_wKey)
	{
		distanceMovedForward += m_Speed;
	}

	if (m_aKey)
	{
		distanceMovedSide -= m_Speed;
	}

	if (m_sKey)
	{
		distanceMovedForward -= m_Speed;
	}

	if (m_dKey)
	{
		distanceMovedSide += m_Speed;
	}

	// move camera and update view matrix
	ComputeDirectionVector();
	ComputeRightAngleDirectionVector();

	if (distanceMovedForward != 0.0f || distanceMovedSide != 0.0f)
	{
		UpdateCameraPosition(distanceMovedForward, distanceMovedSide);
	}

	ComputeViewMatrixUsingLookAt();
}

void Camera::HandleMouseInput(int xPosRelative, int yPosRelative)
{
	const float inverseSensitivity = 4;

	m_angleXZ += (float)xPosRelative / inverseSensitivity;
	m_angleYZ += -(float)yPosRelative / inverseSensitivity;

	// keep in the range -90 to + 90
	if (m_angleYZ > 89)
	{
		m_angleYZ = 89;
	}

	if (m_angleYZ < -89)
	{
		m_angleYZ = -89;
	}

	ComputeDirectionVector();
	ComputeViewMatrixUsingLookAt();
}

void Camera::KeyDown(unsigned char keyCode)
{
	if (keyCode == 'W')
	{
		m_wKey = true;
	}
	
	if (keyCode == 'A')
	{
		m_aKey = true;
	}

	if (keyCode == 'S')
	{
		m_sKey = true;
	}

	if (keyCode == 'D')
	{
		m_dKey = true;
	}

	HandleKeyInput();
}

void Camera::KeyUp(unsigned char keyCode)
{
	if (keyCode == 'W')
	{
		m_wKey = false;
	}

	if (keyCode == 'A')
	{
		m_aKey = false;
	}

	if (keyCode == 'S')
	{
		m_sKey = false;
	}

	if (keyCode == 'D')
	{
		m_dKey = false;
	}
}
