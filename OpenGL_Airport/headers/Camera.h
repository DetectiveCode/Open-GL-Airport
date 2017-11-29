#pragma once
#include "MathFunc.h"
#include "Win32OpenGL.h"

/** Camera Class */
class Camera
{
	const float startX = 0;
	const float startY = 1.5f;
	const float startZ = 5;

	float m_x{ startX };
	float m_y{ startY };
	float m_z{ startZ };

	float m_angleXY{ 0 };
	float m_angleXZ{ 0 };
	float m_angleYZ{ 0 };

	float m_directionX{ 0 };
	float m_directionY{ 0 };
	float m_directionZ{ 0 };

	float m_directionRightY{ 0 };
	float m_directionRightX{ 0 };
	float m_directionRightZ{ 0 };

	bool m_wKey = false;
	bool m_aKey = false;
	bool m_sKey = false;
	bool m_dKey = false;

	float m_Speed = 2.5f;

public:
	mat4 m_viewMatrix;

	void ComputeViewMatrixUsingLookAt();
	void ComputeDirectionVector();
	void ComputeRightAngleDirectionVector();
	void UpdateCameraPosition(float distanceMovedForward, float distanceMovedSide);
	void SetViewMatrix(GLuint program);
	void HandleKeyInput();
	void HandleMouseInput(int xPosRelative, int yPosRelative);
	void KeyDown(unsigned char keyCode);
	void KeyUp(unsigned char keyCode);
};
