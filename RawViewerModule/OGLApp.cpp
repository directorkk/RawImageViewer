#include "OGLApp.h"


namespace{
	// USED TO FORWORD MSGS TO USER DEFINED PROC FUNCTION
	OGLApp* g_pApp = nullptr;
}

LRESULT CALLBACK MainWndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	if(g_pApp) return g_pApp->MsgProc(hWnd, msg, wParam, lParam);
	else return DefWindowProc(hWnd, msg, wParam, lParam);
}


OGLApp::OGLApp(UINT WndWidth, UINT WndHeight, UINT FramePerSecond, std::string WndTitle)
{
	if(WndWidth == 0)
		WndWidth = 1;
	if(WndHeight == 0)
		WndHeight = 1;

	windowQuit = false;

	m_hAppWnd = NULL;
	m_hAppInstance = GetModuleHandle( NULL );
	m_WndStyle = WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU | WS_THICKFRAME | WS_MINIMIZEBOX;
	m_ClientWidth = WndWidth;
	m_ClientHeight = WndHeight;
	m_ClientRatio = (float)WndWidth/(float)WndHeight;
	m_ClientPosX = 0;
	m_ClientPosY = 0;
	m_ControlFps = FramePerSecond;
	m_AppTitle = WndTitle;
	g_pApp = this;
}

OGLApp::~OGLApp()
{

}

bool OGLApp::RunLiveLoop()
{
	MSG msg = {0};

	while( !windowQuit ){
		if( PeekMessage( &msg, NULL, 0, 0, PM_REMOVE ) ){
			if( msg.message == WM_QUIT ){
				QuitWindow();
			}
			else{
				DispatchMessage( &msg );
			}
		} else {
			IdleUpdate(0.0f);
			Render(0.0f);
			SwapBuffers(m_hDevice);
		}
	}

	return true;
}

void OGLApp::InitDeviceInputState()
{
	mouse_x = mouse_y = old_mouse_x = old_mouse_y = 0;
	useMouse = true;
	memset(stateKeyboard, false, 256);
	mouseLeft = 0;
	mouseRight = 0;
	mouseMiddle = 0;
	mouseWheel = 0;
	mouseLeftPressed = false;
	mouseRightPressed = false;
	mouseMiddlePressed = false;
	deviceInput = false;
}

int OGLApp::GetMouseLeft()
{
	return mouseLeft;
}

int OGLApp::GetMouseRight()
{
	return mouseRight;
}

int OGLApp::GetMouseWheelState()
{
	return mouseWheel;
}

int OGLApp::GetMouseMiddle()
{
	return mouseMiddle;
}

POINT OGLApp::GetMouseShiftDistance(POINT pos)
{
	POINT Output;

	Output.x = pos.x - old_mouse_x;
	Output.y = pos.y - old_mouse_y;

	old_mouse_x = pos.x;
	old_mouse_y = pos.y;

	return Output;
}

bool* OGLApp::GetKeyboardState()
{
	return stateKeyboard;
}

UINT OGLApp::GetClientWidth()
{
	return m_ClientWidth;
}

UINT OGLApp::GetClientHeight()
{
	return m_ClientHeight;
}

LRESULT OGLApp::MsgProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	bool shiftPressed = GetKeyState( VK_SHIFT ) < 0;
	bool capsLockOn = GetKeyState( VK_CAPITAL ) > 0;

	if( msg == WM_CLOSE ){
		PostQuitMessage( 0 );
		return 0;
	}

	POINT pos;
	RECT rect;
	if( GetCursorPos( &pos ) && GetWindowRect( hWnd, &rect ) ){
		if(hWnd == m_hAppWnd){
			m_ClientPosX = rect.left;
			m_ClientPosY = rect.top;

			RECT cli_rect;
			GetClientRect( hWnd, &cli_rect );
			m_ClientWidth = cli_rect.right-cli_rect.left;
			m_ClientHeight = cli_rect.bottom-cli_rect.top;

			m_WndBorder.left = m_WndBorder.right = m_WndBorder.bottom = (rect.right-rect.left)/2-(cli_rect.right-cli_rect.left)/2;
			m_WndBorder.top = (rect.bottom-rect.top)-(cli_rect.bottom-cli_rect.top)-m_WndBorder.bottom;

			pos.x -= (rect.left + m_WndBorder.left);
			pos.y -= (rect.top + m_WndBorder.top);
		}
	}

	// mouse input
	if( msg == WM_LBUTTONDOWN ||
		msg == WM_LBUTTONUP ||
		msg == WM_RBUTTONDOWN ||
		msg == WM_RBUTTONUP ||
		msg == WM_MOUSEWHEEL ||
		msg == WM_MBUTTONDOWN ||
		msg == WM_MBUTTONUP)
	{
		if( msg == WM_LBUTTONDOWN){
			mouseLeftPressed = true;
			mouseLeft = 1;
			old_mouse_x = pos.x; 
			old_mouse_y = pos.y;
		}
		if( msg == WM_LBUTTONUP){
			mouseLeftPressed = false;
			mouseLeft = -1;
			mouse_x = 0; 
			mouse_y = 0; 
		}
		if( msg == WM_RBUTTONDOWN){
			mouseRightPressed = true;
			mouseRight = 1;
			old_mouse_x = pos.x; 
			old_mouse_y = pos.y;
		}
		if( msg == WM_RBUTTONUP){
			mouseRightPressed = false;
			mouseRight = -1;
			mouse_x = 0; 
			mouse_y = 0; 
		}
		if( msg == WM_MBUTTONDOWN){
			mouseMiddlePressed = true;
			mouseMiddle = 1;
			old_mouse_x = pos.x; 
			old_mouse_y = pos.y;
		}
		if( msg == WM_MBUTTONUP){
			mouseMiddlePressed = false;
			mouseMiddle = -1;
			mouse_x = 0; 
			mouse_y = 0; 
		}
		if( msg == WM_MOUSEWHEEL ){
			if( wParam == 0x780000 )
				mouseWheel = 1;
			else if( wParam == 0xFF880000 )
				mouseWheel = -1;
		}
	}
	if( !useMouse ){
		old_mouse_x = pos.x; 
		old_mouse_y = pos.y;
	}
	HandleMouseMsg( hWnd, pos );
	mouseLeft = 0;
	mouseRight = 0;
	mouseMiddle = 0;
	mouseWheel = 0;

	// keyboard
	if( msg == WM_KEYDOWN ||
		msg == WM_KEYUP)
	{
		if( msg == WM_KEYDOWN ){
			//std::cout << msg << ", " << wParam << ", " << lParam << std::endl;
			if( Util::InTheRange( wParam, 'A', 'Z' ) ){
				// wParam�u�|�X�{�j�g���X, �ҥH�n�ۦ�P�_�j�p�g
				if( shiftPressed ^ capsLockOn )
					stateKeyboard[wParam] = true;
				else
					stateKeyboard[wParam+32] = true;
			}
			else if(wParam == 0x00E5){
				// �^��H�~��J�k�� A~Z
				stateKeyboard[AtoZInOtherInputMethod(lParam)] = true;
			}
			else{
				stateKeyboard[wParam] = true;
			}
			HandleKeyboardMsg(hWnd);
		}
		if( msg == WM_KEYUP ){
			HandleKeyboardUpMsg(hWnd);
			if( Util::InTheRange( wParam, 'A', 'Z' ) ){
				// �����N�~�����j�p�g, �r�������}�l�P�������j�p�g���䪬�A
				stateKeyboard[wParam] = false;
				stateKeyboard[wParam+32] = false;
			}
			else if(wParam == 0x00E5){
				stateKeyboard[AtoZInOtherInputMethod(lParam)] = false;
			}
			else{
				stateKeyboard[wParam] = false;
			}
		}
	}

	return DefWindowProc( hWnd, msg, wParam, lParam );
}

bool OGLApp::Init()
{
	InitDeviceInputState();

	if(!InitWindow())
		return false;

	RECT rect;
	GetWindowRect(m_hAppWnd, &rect);
	m_ClientRatio = (float)(rect.right-rect.left)/(float)(rect.bottom-rect.top);

	return true;
}

bool OGLApp::InitWindow()
{
	WNDCLASSEX wcex;
	ZeroMemory(&wcex, sizeof(WNDCLASSEX));
	wcex.cbClsExtra = 0;
	wcex.cbWndExtra = 0;
	wcex.cbSize = sizeof(WNDCLASSEX);
	wcex.style = CS_HREDRAW | CS_VREDRAW;
	wcex.hInstance = m_hAppInstance;
	wcex.lpfnWndProc = MainWndProc;
	wcex.hIcon = LoadIcon(NULL, IDI_APPLICATION);
	wcex.hCursor = LoadCursor(NULL, IDC_ARROW);
	wcex.hbrBackground = CreateSolidBrush(RGB(130, 130, 130));
	wcex.lpszMenuName = NULL;
	wcex.lpszClassName = L"OGLAppWNDCLASS";
	wcex.hIconSm = LoadIcon(NULL, IDI_APPLICATION);

	if(!RegisterClassEx(&wcex)){
		std::cout << ("Failed to create window class.");
		return false;
	}

	RECT r = {0, 0, m_ClientWidth, m_ClientHeight};
	AdjustWindowRect(&r, m_WndStyle, FALSE);
	UINT width = r.right - r.left;
	UINT height = r.bottom - r.top;

	UINT x = GetSystemMetrics(SM_CXSCREEN)/2 - width/2;
	UINT y = GetSystemMetrics(SM_CYSCREEN)/2 - height/2;

	m_hAppWnd = CreateWindow(
		wcex.lpszClassName, 
		Util::stows(m_AppTitle.c_str()),
		m_WndStyle, 
		x, y, 
		width, height, 
		NULL, 
		NULL, 
		m_hAppInstance,
		NULL);
	if(!m_hAppWnd){
		std::cout << ("Failed to create window.");
		return false;
	}

	EnableOpenGL( m_hAppWnd, &m_hDevice, &m_hGLRender);
	ShowWindow(m_hAppWnd, SW_SHOW);

	return true;
}

void OGLApp::HandleMouseMsg(HWND hWnd, POINT pos)
{
	switch(GetMouseLeft()){
	case 1:
		std::cout << "Left click: (" << pos.x << ", " << pos.y << ")" << std::endl;
		break;
	case -1:
		std::cout << "Left click up: (" << pos.x << ", " << pos.y << ")" << std::endl;
		break;
	}
	if(MouseLeftPressed()){
		std::cout << "Left shifting.." << std::endl;
	}

	switch(GetMouseRight()){
	case 1:
		std::cout << "Right click: (" << pos.x << ", " << pos.y << ")" << std::endl;
		break;
	case -1:
		std::cout << "Right click up: (" << pos.x << ", " << pos.y << ")" << std::endl;
		break;
	}
	if(MouseRightPressed()){
		std::cout << "Right shifting.." << std::endl;
	}

	switch(GetMouseMiddle()){
	case 1:
		std::cout << "Middle click: (" << pos.x << ", " << pos.y << ")" << std::endl;
		break;
	case -1:
		std::cout << "Middle click up: (" << pos.x << ", " << pos.y << ")" << std::endl;
		break;
	}
	if(MouseMiddlePressed()){
		std::cout << "Middle shifting.." << std::endl;
	}

	switch(GetMouseWheelState()){
	case 1:
		std::cout << "Wheel up: (" << pos.x << ", " << pos.y << ")" << std::endl;
		break;
	case -1:
		std::cout << "Wheel down: (" << pos.x << ", " << pos.y << ")" << std::endl;
		break;
	}
}

void OGLApp::HandleKeyboardMsg(HWND hWnd)
{
	if( stateKeyboard[27] ){
		// press ESC
		windowQuit = true;
		return;
	}

	for(UINT i=0 ; i<256 ; i++)
		if(stateKeyboard[i])
			std::cout << "press " << (char)i << " key" << std::endl;
}

void OGLApp::HandleKeyboardUpMsg(HWND hWnd)
{

}
void OGLApp::QuitWindow()
{
	windowQuit = true;
}

bool OGLApp::MouseLeftPressed()
{
	return mouseLeftPressed;
}

bool OGLApp::MouseRightPressed()
{
	return mouseRightPressed;
}

bool OGLApp::MouseMiddlePressed()
{
	return mouseMiddlePressed;
}

char OGLApp::AtoZInOtherInputMethod(LPARAM lParam)
{
	switch(lParam)
	{
	case 0x1e0001:
		return 'a';
		break;
	case 0x300001:
		return 'b';
		break;
	case 0x2e0001:
		return 'c';
		break;
	case 0x200001:
		return 'd';
		break;
	case 0x120001:
		return 'e';
		break;
	case 0x210001:
		return 'f';
		break;
	case 0x220001:
		return 'g';
		break;
	case 0x230001:
		return 'h';
		break;
	case 0x170001:
		return 'i';
		break;
	case 0x240001:
		return 'j';
		break;
	case 0x250001:
		return 'k';
		break;
	case 0x260001:
		return 'l';
		break;
	case 0x320001:
		return 'm';
		break;
	case 0x310001:
		return 'n';
		break;
	case 0x180001:
		return 'o';
		break;
	case 0x190001:
		return 'p';
		break;
	case 0x100001:
		return 'q';
		break;
	case 0x130001:
		return 'r';
		break;
	case 0x1f0001:
		return 's';
		break;
	case 0x140001:
		return 't';
		break;
	case 0x160001:
		return 'u';
		break;
	case 0x2f0001:
		return 'v';
		break;
	case 0x110001:
		return 'w';
		break;
	case 0x2d0001:
		return 'x';
		break;
	case 0x150001:
		return 'y';
		break;
	case 0x2c0001:
		return 'z';
		break;
	}
	return ' ';
}

void OGLApp::EnableOpenGL(HWND hWnd, HDC *hDC, HGLRC *hRC) 
{ 
	PIXELFORMATDESCRIPTOR pfd; 
	int iFormat; 
	*hDC = GetDC( hWnd ); 

	// set the pixel format for the DC 
	ZeroMemory (&pfd, sizeof( pfd )); 
	pfd.nSize = sizeof( pfd ); 
	pfd.nVersion = 1; 
	pfd.dwFlags = PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER;
	pfd.iPixelType = PFD_TYPE_RGBA; 
	pfd.cColorBits = 24;
	pfd.cAlphaBits = 8;  
	pfd.cDepthBits = 16; 
	pfd.iLayerType = PFD_MAIN_PLANE; 
	iFormat = ChoosePixelFormat ( *hDC, &pfd );   //��WINDOWS���A��A�X��pixel format 
	SetPixelFormat ( *hDC, iFormat, &pfd ); 

	// create and enable the render context (RC)
	*hRC = wglCreateContext( *hDC );         //wgl�}�Y�����OWINDOWS�Ψӳs��OpenGL�Ϊ� 
	wglMakeCurrent( *hDC, *hRC ); 
} 

void OGLApp::DisableOpenGL(HWND hWnd, HDC hDC, HGLRC hRC) 
{ 
	wglMakeCurrent( NULL, NULL ); 
	wglDeleteContext( hRC ); 
	ReleaseDC( hWnd, hDC ); 
}