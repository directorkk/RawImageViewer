#ifndef _OPENGL_APPLICATION_H_
#define _OPENGL_APPLICATION_H_


#include <Windows.h>
#define IDC_MAIN_BUTTON	101			// Button identifier


#include <iostream>
#include <string>


#include "OGLHeader.h"

#include "Util.h"


class OGLApp{
public:

	OGLApp(UINT WndWidth, UINT WndHeight, UINT FramePerSecond, std::string WndTitle);
	~OGLApp();

	virtual bool RunLiveLoop();

	virtual bool Init();
	virtual void IdleUpdate(float dt) = 0;
	virtual void Render(float dt) = 0;
	virtual LRESULT MsgProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

private:
	bool InitWindow();
	char AtoZInOtherInputMethod(LPARAM lParam);
protected:
	HWND			m_hAppWnd;
	HINSTANCE		m_hAppInstance;
	UINT			m_ClientWidth;
	UINT			m_ClientHeight;
	FLOAT			m_ClientRatio;
	INT				m_ClientPosX;
	INT				m_ClientPosY;
	RECT			m_WndBorder;
	std::string		m_AppTitle;
	DWORD			m_WndStyle;
	UINT			m_ControlFps;
	HDC				m_hDevice;
	HGLRC			m_hGLRender;

public:
	void InitDeviceInputState();
	bool MouseLeftPressed();
	bool MouseRightPressed();
	bool MouseMiddlePressed();
	int GetMouseLeft();
	int GetMouseRight();
	int GetMouseMiddle();
	int GetMouseWheelState();
	POINT GetMouseShiftDistance(POINT pos);
	bool* GetKeyboardState();
	UINT GetClientWidth();
	UINT GetClientHeight();
	void QuitWindow();
private:
	virtual void HandleMouseMsg(HWND hWnd, POINT pos);
	virtual void HandleKeyboardMsg(HWND hWnd);
	virtual void HandleKeyboardUpMsg(HWND hWnd);
public:
	int		mouse_x, 
			mouse_y, 
			old_mouse_x,
			old_mouse_y;
	bool	useMouse;
	bool	stateKeyboard[256];
	int		mouseLeft;
	int		mouseRight;
	int		mouseMiddle;
	int		mouseWheel;

	bool	mouseLeftPressed;
	bool	mouseRightPressed;
	bool	mouseMiddlePressed;
	bool	deviceInput;
	bool	windowQuit;

protected:
	void DisableOpenGL(HWND hWnd, HDC hDC, HGLRC hRC);
	void EnableOpenGL(HWND hWnd, HDC *hDC, HGLRC *hRC);
};

#endif