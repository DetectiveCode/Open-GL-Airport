#pragma once
#include "Win32OpenGL.h"
#include "resource.h"
#include "Model.h"
#include "Camera.h"
#include "ModelHandler.h"

/** Main Class */
class Main
{
	Win32OpenGL m_win32OpenGL; ///< OpenGL helper class.

	mat4 m_projectionMatrix;

	vec3 m_lightColourSpecular{ 1.0f, 1.0f, 1.0f };
	vec3 m_lightColourDiffuse{ 0.8f, 0.8f, 0.8f };
	vec3 m_lightColourAmbient{ 0.3f, 0.3f, 0.3f };
	vec3 m_lightPosition{ 0.0f, 0.0f, 5.0f };

	float m_aspectRatio;

	bool stopUpdate = false;

	ModelHandler m_modelHandler;
	Camera m_camera;
	RECT m_rect;

public:
	Main();
	~Main();

	void CreateGLWindow(HDC hdc, RECT rect);
	void DestroyGLWindow();
	void PrepareToDraw();
	void Draw();
	void Update();
	void HandleKeyDown(unsigned char virtualKeyCode);
	void HandleKeyUp(unsigned char virtualKeyCode);
	void HandleMouseInput(int xPosRelative, int yPosRelative);
	void Resize(HDC hdc, RECT rect);
	void ComputeProjectionMatrix();

	HWND m_hWnd;
	
	short m_windowXPos;
	short m_windowYPos;
};

