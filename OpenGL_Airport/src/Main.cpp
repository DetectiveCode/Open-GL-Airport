#include "stdafx.h"
#include "Main.h"

Main::Main()
{
}

Main::~Main()
{
}

void Main::CreateGLWindow(HDC hdc, RECT rect)
{
	m_win32OpenGL.CreateGLContext(hdc);	// may throw an exception!!!
	//MessageBoxA(0, (char*)glGetString(GL_VERSION), "OPENGL VERSION", 0);
	m_aspectRatio = static_cast<float>(rect.right / rect.bottom);
	m_rect = rect;

	/* Shaders */
	//m_win32OpenGL.CreateShadersAndProgram("flatFixedColour");
	//m_win32OpenGL.CreateShadersAndProgram("flat");
	//m_win32OpenGL.CreateShadersAndProgram("flatVerticesAsColours");
	
	/* Lighting Shaders */
	//m_win32OpenGL.CreateShadersAndProgram("phong"); //Does not scale normals correctly
	//m_win32OpenGL.CreateShadersAndProgram("phongScaled");
	//m_win32OpenGL.CreateShadersAndProgram("diffuseOnly");

	/* Texture Shaders */
	//m_win32OpenGL.CreateShadersAndProgram("phongTexture2"); //Does not scale normals correctly
	//m_win32OpenGL.CreateShadersAndProgram("phongTextureScaled");
	m_win32OpenGL.CreateShadersAndProgram("texture");
}

void Main::DestroyGLWindow()
{
	m_win32OpenGL.TearDownGLContext();
}

void Main::PrepareToDraw()
{
	///< Compute the model matrix in the draw routine so we can animate the triangle
	ComputeProjectionMatrix();

	m_camera.ComputeDirectionVector();
	m_camera.ComputeViewMatrixUsingLookAt();

	///< Send the matrices to the shader
	GLuint program = m_win32OpenGL.GetShaderProgram();
	m_camera.SetViewMatrix(program);

	Win32OpenGL::SendUniformMatrixToShader(program, m_projectionMatrix.m, "projection_matrix");
	Win32OpenGL::SendUniformMatrixToShader(program, m_camera.m_viewMatrix.m, "view_matrix");

	///< Send lighting info t shader.
	Win32OpenGL::SendUniformVector3ToShader(program, m_lightPosition.v, "light_position_world");
	Win32OpenGL::SendUniformVector3ToShader(program, m_lightColourSpecular.v, "light_colour_specular");
	Win32OpenGL::SendUniformVector3ToShader(program, m_lightColourDiffuse.v, "light_colour_diffuse");
	Win32OpenGL::SendUniformVector3ToShader(program, m_lightColourAmbient.v, "light_colour_ambient");

	///< Load models here.
	m_modelHandler.Load();
}

void Main::Draw()
{
	m_win32OpenGL.ClearGLDisplay();

	GLuint program = m_win32OpenGL.GetShaderProgram();

	//float colour[] = { 1.0f, 0.0f, 0.5f };
	//Win32OpenGL::SendUniformVector3ToShader(program, colour, "surface_colour");

	m_modelHandler.Draw(program);
	m_win32OpenGL.FinishedDrawing();
}

void Main::Update()
{
	if (!stopUpdate && GetForegroundWindow() == m_hWnd)
	{
		m_modelHandler.Update();
	}
	
	ShowCursor(stopUpdate);
}

void Main::HandleKeyDown(unsigned char virtualKeyCode)
{
	if (GetForegroundWindow() == m_hWnd)
	{
		m_camera.KeyDown(virtualKeyCode);

		GLuint program = m_win32OpenGL.GetShaderProgram();
		m_camera.SetViewMatrix(program);

		// add code for interaction here
		if (virtualKeyCode == VK_SPACE)
		{
			stopUpdate = !stopUpdate;
		}
	}
}

void Main::HandleKeyUp(unsigned char virtualKeyCode)
{
	m_camera.KeyUp(virtualKeyCode);
}

void Main::HandleMouseInput(int xPosRelative, int yPosRelative)
{
	if (!stopUpdate && GetForegroundWindow() == m_hWnd)
	{
		// most of input moves camera to pass to camera class
		m_camera.HandleMouseInput(xPosRelative, yPosRelative);

		// camera class doesn't manage shader
		GLuint program = m_win32OpenGL.GetShaderProgram();
		m_camera.SetViewMatrix(program);

 		SetCursorPos(m_windowXPos + (m_rect.right - m_rect.left) / 2, m_windowYPos + (m_rect.bottom - m_rect.top) / 2);
	}
}

void Main::Resize(HDC hdc, RECT rect)
{
	// if the display is resized then OpenGL needs to know about the apect ratio
	// to compute the correct projection matrix

	m_win32OpenGL.Reshape(hdc, rect.right, rect.bottom);
	m_aspectRatio = (float)rect.right / rect.bottom;
	m_rect = rect;
	ComputeProjectionMatrix();
	GLuint program = m_win32OpenGL.GetShaderProgram();
	Win32OpenGL::SendUniformMatrixToShader(program, m_projectionMatrix.m, "projection_matrix");
}

void Main::ComputeProjectionMatrix()
{
	// in Modern OpenGL we must supply a projection matrix

	#define ONE_DEG_IN_RAD (2.0 * M_PI) / 360.0 // 0.017444444

	// input variables
	float zNear = 1.0f;	// Near clipping plane - 0.1f does not work on AMD cards apparently.
	float zFar = 100000.0f;	// Far clipping plane
	float fov = static_cast<float>(67.0f * ONE_DEG_IN_RAD); // convert 67 degrees to radians

	float range = tan(fov * 0.5f) * zNear;
	float Sx = (2.0f * zNear) / (range * m_aspectRatio + range * m_aspectRatio);
	float Sy = zNear / range;
	float Sz = -(zFar + zNear) / (zFar - zNear);
	float Pz = -(2.0f * zFar * zNear) / (zFar - zNear);

	GLfloat proj_mat[] = {
		Sx, 0.0f, 0.0f, 0.0f,
		0.0f, Sy, 0.0f, 0.0f,
		0.0f, 0.0f, Sz, -1.0f,
		0.0f, 0.0f, Pz, 0.0f
	};

	for (int i = 0; i < 16; i++)
	{
		m_projectionMatrix.m[i] = proj_mat[i];
	}
}