// WinMain.cpp : Defines the entry point for the application.
#include "stdafx.h"
#include "WinMain.h"
#include "mmsystem.h"
#include "Main.h"
#include <algorithm>

#define MAX_LOADSTRING 100 
#define	WM_MMTIMERTICK	(WM_USER+1)	// mm timer for game to hang off!

// Global Variables:
HINSTANCE hInst; // current instance
WCHAR szTitle[MAX_LOADSTRING]; // The title bar text
WCHAR szWindowClass[MAX_LOADSTRING]; // the main window class name
HWND hWnd;
unsigned int mmTimerDelay = 1000 / 50; //50Hz = 20ms per frame
unsigned int mmTimerId;
bool mmTimerInitialised = false;
// keyboard state stored here 256 bits = 32 unsigned chars
static unsigned char keyState[32]{ 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };
static unsigned char masks[8]{ 0x01, 0x02, 0x04, 0x08, 0x10, 0x20, 0x40, 0x80 };

// Forward declarations of functions included in this code module:
ATOM MyRegisterClass(HINSTANCE hInstance);
BOOL InitInstance(HINSTANCE, int);
LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK About(HWND, UINT, WPARAM, LPARAM);

void DealWithKeyPress(HWND hWnd, WPARAM wParam, bool isKeydown);
bool InitialiseMMTimer();
void ShutdownMMTimer();
void CALLBACK GetTimerEvent(UINT IDEvent, UINT reserved, DWORD dwUser, DWORD	dwReserved1, DWORD	dwReserved2);

/**
* WinMain Class is where all the application code goes.
*/

Main main;

int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
	_In_opt_ HINSTANCE hPrevInstance,
	_In_ LPWSTR    lpCmdLine,
	_In_ int       nCmdShow)
{
	UNREFERENCED_PARAMETER(hPrevInstance);
	UNREFERENCED_PARAMETER(lpCmdLine);

	// Initialize global strings
	LoadStringW(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
	LoadStringW(hInstance, IDC_IMAT2902_EX02, szWindowClass, MAX_LOADSTRING);
	MyRegisterClass(hInstance);

	// Perform application initialization:
	if (!InitInstance(hInstance, nCmdShow))
	{
		return FALSE;
	}

	if (!InitialiseMMTimer())
	{
		return FALSE;
	}

	//HACCEL hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_WIN32GL_EX1));

	MSG msg;
	// Main message loop:
	while (GetMessage(&msg, nullptr, 0, 0))
	{
		//if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
	}
	return (int)msg.wParam;
}

//
//  FUNCTION: MyRegisterClass()
//
//  PURPOSE: Registers the window class.
//
ATOM MyRegisterClass(HINSTANCE hInstance)
{
	WNDCLASSEXW wcex;

	wcex.cbSize = sizeof(WNDCLASSEX);

	wcex.style = CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc = WndProc;
	wcex.cbClsExtra = 0;
	wcex.cbWndExtra = 0;
	wcex.hInstance = hInstance;
	wcex.hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_IMAT2902_EX02));
	wcex.hCursor = LoadCursor(nullptr, IDC_ARROW);
	wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
	wcex.lpszMenuName = MAKEINTRESOURCEW(IDC_IMAT2902_EX02);
	wcex.lpszClassName = szWindowClass;
	wcex.hIconSm = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

	return RegisterClassExW(&wcex);
}

//
//   FUNCTION: InitInstance(HINSTANCE, int)
//
//   PURPOSE: Saves instance handle and creates main window
//
//   COMMENTS:
//
//   In this function, we save the instance handle and window handle in a global variable and
//   create and display the main program window.
//
BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
	hInst = hInstance; // Store instance handle in our global variable

	hWnd = CreateWindowW(szWindowClass, szTitle, WS_OVERLAPPEDWINDOW,
		CW_USEDEFAULT, 0, CW_USEDEFAULT, 0, nullptr, nullptr, hInstance, nullptr);

	if (!hWnd)
	{
		return FALSE;
	}

	ShowWindow(hWnd, nCmdShow);
	UpdateWindow(hWnd);
	return TRUE;
}

//
//  FUNCTION: WndProc(HWND, UINT, WPARAM, LPARAM)
//
//  PURPOSE:  Processes messages for the main window.
//
//  WM_COMMAND  - process the application menu
//  WM_PAINT    - Paint the main window
//  WM_DESTROY  - post a quit message and return
//
//
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	//int wmId, wmEvent;
	//PAINTSTRUCT ps;
	HDC hdc;
	RECT rect;
	RAWINPUTDEVICE Rid[1];

	Rid[0].usUsagePage = HID_USAGE_PAGE_GENERIC;
	Rid[0].usUsage = HID_USAGE_GENERIC_MOUSE;
	Rid[0].dwFlags = RIDEV_INPUTSINK;
	Rid[0].hwndTarget = hWnd;
	main.m_hWnd = hWnd;
	RegisterRawInputDevices(Rid, 1, sizeof(Rid[0]));

	switch (message)
	{
	case WM_INPUT:
	{
		UINT dwSize = 40;
		static BYTE lpb[40];

		GetRawInputData((HRAWINPUT)lParam, RID_INPUT,
			lpb, &dwSize, sizeof(RAWINPUTHEADER));

		RAWINPUT* raw = (RAWINPUT*)lpb;

		if (raw->header.dwType == RIM_TYPEMOUSE)
		{
			int xPosRelative = raw->data.mouse.lLastX;
			int yPosRelative = raw->data.mouse.lLastY;

			main.HandleMouseInput(xPosRelative, yPosRelative);
		}
		break;
	}
	case WM_MMTIMERTICK:
	
		main.Update();
		//invalidate the window to force a WM_PAINT call
		RedrawWindow(hWnd, NULL, NULL, RDW_INVALIDATE | RDW_UPDATENOW);
		break;
	
	case WM_PAINT:
	{
		PAINTSTRUCT ps;
		HDC hdc = BeginPaint(hWnd, &ps);
		main.Draw();
		SwapBuffers(hdc);
		EndPaint(hWnd, &ps);
	}
	break;

	case WM_CREATE:

		// here is where we create our open gl context
		hdc = GetDC(hWnd);
		GetClientRect(hWnd, &rect);
		main.CreateGLWindow(hdc, rect);

		// prepare vbo's and vao's and setup shader
		main.PrepareToDraw();
		break;

	case WM_COMMAND:
	{
		int wmId = LOWORD(wParam);
		// Parse the menu selections:
		switch (wmId)
		{
		case IDM_ABOUT:
			DialogBox(hInst, MAKEINTRESOURCE(IDD_ABOUTBOX), hWnd, About);
			break;
		case IDM_EXIT:
			DestroyWindow(hWnd);
			break;
		default:
			return DefWindowProc(hWnd, message, wParam, lParam);
		}
	}
	break;

	case WM_MOVE:
		main.m_windowXPos = (int)(short)LOWORD(lParam);   // horizontal position 
		main.m_windowYPos = (int)(short)HIWORD(lParam);   // vertical position 

	case WM_SIZE:
		GetClientRect(hWnd, &rect);
		hdc = GetDC(hWnd);
		// pass to opengl
		main.Resize(hdc, rect);
		break;

	case WM_KEYDOWN:
		DealWithKeyPress(hWnd, wParam, true);
		break;

	case WM_KEYUP:
		DealWithKeyPress(hWnd, wParam, false);
		break;

	case WM_CLOSE:
		//if (MessageBox(hWnd, L"Really quit?", L"My application", MB_OKCANCEL) == IDOK)
		main.DestroyGLWindow();
		DestroyWindow(hWnd);
		return 0;

	case WM_DESTROY:
		PostQuitMessage(0);
		break;
	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
	}
	return 0;
}

// Message handler for about box.
INT_PTR CALLBACK About(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
	UNREFERENCED_PARAMETER(lParam);
	switch (message)
	{
	case WM_INITDIALOG:
		return (INT_PTR)TRUE;

	case WM_COMMAND:
		if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL)
		{
			EndDialog(hDlg, LOWORD(wParam));
			return (INT_PTR)TRUE;
		}
		break;
	}
	return (INT_PTR)FALSE;
}

//---------------------------------------------------------
//
// bool initialiseMMTimer()
//
//---------------------------------------------------------

bool InitialiseMMTimer()
{
	TIMECAPS	tc;

	timeGetDevCaps(&tc, sizeof(TIMECAPS));

	if ((mmTimerDelay < tc.wPeriodMin) || (mmTimerDelay > tc.wPeriodMax))
	{
		return false;
	}
	timeBeginPeriod(mmTimerDelay);

	mmTimerId = timeSetEvent(mmTimerDelay,		//another flash
		mmTimerDelay,
		GetTimerEvent,
		(DWORD)hWnd,
		TIME_PERIODIC);		//TIME_ONESHOT

	if (mmTimerId)			//returns null if fails...
	{
		mmTimerInitialised = true;
	}
	return mmTimerInitialised;
}

//---------------------------------------------------------
//	void	shutdown_MM_timer()
//---------------------------------------------------------
void ShutdownMMTimer()
{
	MMRESULT	err;
	if (mmTimerInitialised)
	{
		err = timeKillEvent(mmTimerId);

		if (err != TIMERR_NOERROR)
		{
			OutputDebugString(L"MM Timer shutdown error\n");
			if (err == MMSYSERR_INVALPARAM)
			{
				OutputDebugString(L"Invalid Paramters\n");
			}
			else
			{
				OutputDebugString(L"Unknown error\n");
			}
		}
		mmTimerInitialised = false;
	}
	err = timeEndPeriod(mmTimerDelay);

	if (err != TIMERR_NOERROR)
	{
		OutputDebugString(L"Timer end period error\n");
	}
}
//---------------------------------------------------------
//	get_MM_timer_event();
//---------------------------------------------------------

// LPTIMECALLBACK

void CALLBACK GetTimerEvent(UINT IDEvent, UINT reserved, DWORD dwUser,			//we store hwnd here
	DWORD	dwReserved1,
	DWORD	dwReserved2)

	//LPTIMECALLBACK GetTimerEvent(UINT IDEvent, UINT reserved, DWORD dwUser,			//we store hwnd here
	//	DWORD	dwReserved1,
	//	DWORD	dwReserved2)

{
	UNREFERENCED_PARAMETER(IDEvent);
	UNREFERENCED_PARAMETER(reserved);
	UNREFERENCED_PARAMETER(dwUser);
	UNREFERENCED_PARAMETER(dwReserved1);		//ams warnings level 4
	UNREFERENCED_PARAMETER(dwReserved2);

	PostMessage((HWND)dwUser, WM_MMTIMERTICK, 0, 0);
}


//-----------------------------------------------------------------------------
//
// void	gameDealWithKeyPress(HWND hWnd,WPARAM wParam)
//
//-----------------------------------------------------------------------------

void	DealWithKeyPress(HWND hWnd, WPARAM wParam, bool isKeydown)
{
	// see https://msdn.microsoft.com/en-us/library/windows/desktop/dd375731(v=vs.85).aspx
	//
	// for the VK_ codes defined in windows.h
	// trap escape to exit program
	if (isKeydown && wParam == VK_ESCAPE)
	{
		PostMessage(hWnd, WM_CLOSE, 0, 0);
	}

	// deal with other keys here
	// we start 256 bits (32 unsigned chars) each holding 1 == pressed 0 == not
	int keyByte = (wParam & 0xF8) >> 3;
	unsigned char keyMask = masks[(wParam & 0x07)];
	if (isKeydown)
	{
		// set the relevant bit
		keyState[keyByte] |= keyMask;
		// pass the keypress to the example object
		main.HandleKeyDown(wParam);
	}
	else
	{
		// clear the bit
		keyState[keyByte] &= (~keyMask);
		main.HandleKeyUp(wParam);
	}
}

bool IsKeyPressed(unsigned char keyCode)
{
	// we could test keys state here...
	// keyCode is in 0-0xFF  - see ms VK_ codes.
	int keyByte = (keyCode & 0xF8) >> 3;
	unsigned char keyMask = masks[(keyCode & 0x07)];
	return ((keyState[keyByte] & keyMask) != 0);
}