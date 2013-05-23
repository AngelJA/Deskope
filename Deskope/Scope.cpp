#include "stdafx.h"
#include "Scope.h"

Scope::Scope() : m_intPixelsPerDegree(DEFAULT_PIXELS_PER_DEGREE),
				 m_intImageSeparation(DEFAULT_SEPARATION),
				 m_boolClipCursor(DEFAULT_CLIP_CURSOR),
				 m_intSBSOffset(0),
				 m_fltZoom(DEFAULT_ZOOM)
{
}

Scope::~Scope()
{
	ReleaseRiftResources();
	ReleaseWindowsResources();
	KillTimers();
}

int Scope::Initialize()
{
	if (SetUpRift() < 0)
	{
		MessageBox(m_hwnd, _T("Error setting up Rift."), _T("Error"), MB_OK | MB_ICONERROR);
		return -1;
	}
	if (GetRiftDisplayInfo() < 0)
	{
		MessageBox(m_hwnd, _T("Error getting Rift display info."), _T("Error"), MB_OK | MB_ICONERROR);
		return -1;
	}
	ResizeSource();
	if (SetUpWindow() < 0)		
	{
		MessageBox(m_hwnd, _T("Windows error."), _T("Error"), MB_OK | MB_ICONERROR);
		return -1;
	}
	AllocateWindowsResources();
	if (SetUpTimers() < 0)		
	{
		MessageBox(m_hwnd, _T("Windows timer error."), _T("Error"), MB_OK | MB_ICONERROR);
		return -1;
	}

	return 0;
}

int Scope::SetUpRift()
{
	System::Init(Log::ConfigureDefaultLog(LogMask_All));
	m_pManager = *DeviceManager::Create();
	m_pHMD = *m_pManager->EnumerateDevices<HMDDevice>().CreateDevice();
	
	if (!m_pHMD)
		return -1;
	m_pSensor = *m_pHMD->GetSensor();
	m_sFusion.AttachToSensor(m_pSensor);
	m_pHMD->GetDeviceInfo(&m_hmdInfo);
	
	return 0;
}

int Scope::SetUpWindow()
{	
	WNDCLASS wc = {0};
	wc.hInstance = GetModuleHandle(NULL);
	wc.lpfnWndProc = Scope::WndProc;
	wc.lpszClassName = _T("Deskope Window Class");
	wc.hCursor = LoadCursor(NULL, IDC_ARROW);
	
	RegisterClass(&wc);
	m_hwnd = CreateWindowEx(WS_EX_TOOLWINDOW | WS_EX_TRANSPARENT,
							_T("Deskope Window Class"),
							_T("Deskope Window"),
							WS_POPUP,
							m_RiftDisplayInfo.dmPosition.x, m_RiftDisplayInfo.dmPosition.y,
							m_RiftDisplayInfo.dmPelsWidth, m_RiftDisplayInfo.dmPelsHeight,
							NULL,
							NULL,
							GetModuleHandle(NULL),
							this);	//pass the window a pointer to this Scope object
	if (!m_hwnd)
		return -1;

	ShowWindow(m_hwnd, SW_SHOWNORMAL);
	return 0;
}

int Scope::SetUpTimers()
{
	m_uipDrawTimer = SetTimer(m_hwnd, 1, 16, Scope::TimerProc);
	if (!m_uipDrawTimer)
		return -1;
	m_uipSendRSDTimer = SetTimer(m_hwnd, 2, 100, Scope::TimerProc);
	if (!m_uipSendRSDTimer)
		return -1;
	return 0;
}

void Scope::RunMessageLoop()
{
    MSG msg;

	BOOL fDone = FALSE;
	while (!fDone)
	{
		while (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
			SleepEx(0, true);
		}
		if (msg.message == WM_QUIT)
			fDone = TRUE;
	}
}

void Scope::ReleaseRiftResources()
{
	m_pManager.Clear();
	m_pHMD.Clear();
	m_pSensor.Clear();
	System::Destroy();
}

void Scope::AllocateWindowsResources()
{
	m_hdc = GetDC(m_hwnd);
	m_winDC = GetDC(NULL);
	m_winCopyDC = CreateCompatibleDC(m_winDC);
	m_winCopyBM = CreateCompatibleBitmap(m_winDC, m_RiftDisplayInfo.dmPelsWidth / 2 + m_intSBSOffset + m_intImageSeparation, m_RiftDisplayInfo.dmPelsHeight);
	SelectObject(m_winCopyDC, m_winCopyBM);
	m_BackDC = CreateCompatibleDC(m_hdc);
	m_BackBM = CreateCompatibleBitmap(m_hdc, m_RiftDisplayInfo.dmPelsWidth, m_RiftDisplayInfo.dmPelsHeight);
	SelectObject(m_BackDC, m_BackBM);

	SetStretchBltMode(m_winCopyDC, HALFTONE);
	SetBrushOrgEx(m_winCopyDC, 0, 0, NULL);
}

void Scope::ReleaseWindowsResources()
{
	ReleaseDC(m_hwnd, m_hdc);
	DeleteDC(m_hdc);
	ReleaseDC(m_hwnd, m_winDC);
	DeleteDC(m_winDC);
	ReleaseDC(m_hwnd, m_winCopyDC);
	DeleteDC(m_winCopyDC);
	ReleaseDC(m_hwnd, m_BackDC);
	DeleteDC(m_BackDC);
	DeleteObject(m_winCopyBM);
	DeleteObject(m_BackBM);
}

void Scope::ResizeSource()
{
	m_intSrcWidth = int(m_RiftDisplayInfo.dmPelsWidth / 2.0 / m_fltZoom);
	m_intSrcHeight = int(m_RiftDisplayInfo.dmPelsHeight / m_fltZoom);

	DeleteObject(m_winCopyBM);
	m_winCopyBM = CreateCompatibleBitmap(m_winDC, m_RiftDisplayInfo.dmPelsWidth / 2 + m_intSBSOffset + m_intImageSeparation, m_RiftDisplayInfo.dmPelsHeight);
	SelectObject(m_winCopyDC, m_winCopyBM);
}

void Scope::KillTimers()
{	
	KillTimer(m_hwnd, m_uipDrawTimer);
	KillTimer(m_hwnd, m_uipSendRSDTimer);
}

//Delegate to member function message handler (because WndProc can't be a member function, must be static)
LRESULT CALLBACK Scope::WndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	Scope * pThis = NULL;
    if (message == WM_CREATE)
    {
        CREATESTRUCT* pCreate = (CREATESTRUCT*)lParam;
        pThis = (Scope*)pCreate->lpCreateParams;
        SetWindowLongPtr(hwnd, GWLP_USERDATA, (LONG_PTR)pThis);

        pThis->m_hwnd = hwnd;
    }
    else
    {
        pThis = (Scope*)GetWindowLongPtr(hwnd, GWLP_USERDATA);
    }
    if (pThis)
    {
        return pThis->HandleMessage(message, wParam, lParam);
    }
    else
    {
        return DefWindowProc(hwnd, message, wParam, lParam);
    }
}

//Delegate to timer methods
VOID CALLBACK Scope::TimerProc(HWND hwnd, UINT, UINT_PTR timer_id, DWORD)
{
	Scope * pThis = (Scope*)GetWindowLongPtr(hwnd, GWLP_USERDATA);
    if (pThis)
	{
		if (timer_id == pThis->m_uipDrawTimer)
		{
			pThis->DrawScope();
			return;
		}
		if (timer_id == pThis->m_uipSendRSDTimer)
		{
			pThis ->SendRiftSensorData();
			return;
		}
	}
}

LRESULT Scope::HandleMessage(UINT message, WPARAM wParam, LPARAM lParam)
{	
	switch (message)
	{	
	//Change settings through Windows messages
	case WM_DESKOPE_VALUE_CHANGE:
		switch(wParam)
		{
		case IMAGE_SEPARATION:
			m_intImageSeparation = lParam;
			ResizeSource();
			break;
		case PIXELS_PER_DEGREE:
			m_intPixelsPerDegree = lParam;
			break;
		case CLIP_CURSOR:
			m_boolClipCursor = (lParam != 0);
			break;
		case RSD_RESET:
			m_sFusion.Reset();
			break;
		case SBS_OFFSET:
			m_intSBSOffset = lParam;
			ResizeSource();
			break;
		case ZOOM:
			m_fltZoom = reinterpret_cast<float&>(lParam);
			ResizeSource();
			break;
		}
		break;
	case WM_DESTROY:
		PostQuitMessage(0);
		break;
	}
	return DefWindowProc(m_hwnd, message, wParam, lParam);
}

void Scope::DrawScope()
{
	BITMAP WinCopyBMInfo;
	GetObject(m_winCopyBM, sizeof(BITMAP), &WinCopyBMInfo);

	//Always on top
	SetWindowPos(m_hwnd, HWND_TOPMOST, 0, 0, 0, 0, SWP_NOACTIVATE | SWP_NOMOVE | SWP_NOSIZE );
	m_sFusion.GetOrientation().GetEulerAngles<Axis_Y, Axis_X, Axis_Z>(&m_fltYaw, &m_fltPitch, &m_fltRoll);

	//Calculate top-left point of source rectangle
	RECT SourceRect;
	SourceRect.left = long(GetSystemMetrics(SM_CXSCREEN) / 2 - (m_intSrcWidth + m_intSBSOffset) / 2.0 + (m_fltYaw * -180.0 / 3.14159) * m_intPixelsPerDegree);
	SourceRect.top = long(GetSystemMetrics(SM_CYSCREEN) / 2 - m_intSrcHeight / 2.0 + (m_fltPitch *  -180.0 / 3.14159) * m_intPixelsPerDegree);

	//Uncomment to control view with mouse
	//POINT p;
	//GetCursorPos(&p);
	//SourceRect.left = p.x - (m_intSrcWidth + m_intSBSOffset) / 2;
	//SourceRect.top = p.y - m_intSrcHeight / 2;
	/////////

	SourceRect.right = SourceRect.left + m_intSrcWidth * 2 + long((m_intSBSOffset + m_intImageSeparation) / m_fltZoom);
	SourceRect.bottom = SourceRect.top + m_intSrcHeight;

	//Restrict the cursor
	if (m_boolClipCursor)
	{
		RECT clip;
		clip.left = long(SourceRect.left + m_RiftDisplayInfo.dmPelsWidth * 0.1);
		clip.right = long(SourceRect.left + m_intSBSOffset + m_RiftDisplayInfo.dmPelsWidth * 0.45);
		clip.top = long(SourceRect.top + m_RiftDisplayInfo.dmPelsHeight * 0.25);
		clip.bottom = long(SourceRect.top + m_RiftDisplayInfo.dmPelsHeight * 0.75);
		ClipCursor(&clip);
	}
	else
	{
		ClipCursor(NULL);
	}

	//Black out copy of the screen
	RECT WinCopyRect;
	SetRect(&WinCopyRect, 0, 0, WinCopyBMInfo.bmWidth, WinCopyBMInfo.bmHeight);
	FillRect(m_winCopyDC, &WinCopyRect, (HBRUSH)COLOR_WINDOWTEXT);

	//Blit a copy of the screen so you don't have to draw on the screen DC
	StretchBlt(m_winCopyDC, 0, 0, WinCopyBMInfo.bmWidth, WinCopyBMInfo.bmHeight, m_winDC, SourceRect.left, SourceRect.top, m_intSrcWidth + int((m_intSBSOffset + m_intImageSeparation) / m_fltZoom), m_intSrcHeight, SRCCOPY);

	//fix recursive display when looking at Scope window
	RECT ScopeRect;
	GetWindowRect(m_hwnd, &ScopeRect);
	RECT Intersection;
	IntersectRect(&Intersection, &SourceRect, &ScopeRect);
	if (!IsRectEmpty(&Intersection))
	{
		LONG width = Intersection.right - Intersection.left;
		LONG height = Intersection.bottom - Intersection.top;
		Intersection.left = abs(Intersection.left - SourceRect.left);
		Intersection.top = abs(Intersection.top - SourceRect.top);
		Intersection.right = Intersection.left + width;
		Intersection.bottom = Intersection.top + height;
		FillRect(m_winCopyDC, &Intersection, (HBRUSH)COLOR_WINDOWTEXT);
	}

	//Draw the cursor
	m_GlobalCursor.cbSize = sizeof(CURSORINFO);
	GetCursorInfo(&m_GlobalCursor);
	if (m_GlobalCursor.flags == CURSOR_SHOWING)
	{
		ICONINFO CursorInfo;
		GetIconInfo((HICON)m_GlobalCursor.hCursor, &CursorInfo);
		DrawIcon(m_winCopyDC, m_GlobalCursor.ptScreenPos.x - SourceRect.left - CursorInfo.xHotspot, m_GlobalCursor.ptScreenPos.y - SourceRect.top - CursorInfo.yHotspot, m_GlobalCursor.hCursor);		
		DeleteObject(CursorInfo.hbmColor);
		DeleteObject(CursorInfo.hbmMask);
	}
	
	//Blit to the back buffer
	BitBlt(m_BackDC, 0, 0, m_RiftDisplayInfo.dmPelsWidth / 2, m_RiftDisplayInfo.dmPelsHeight, m_winCopyDC, 0, 0, SRCCOPY);
	BitBlt(m_BackDC, m_RiftDisplayInfo.dmPelsWidth / 2, 0, m_RiftDisplayInfo.dmPelsWidth / 2, m_RiftDisplayInfo.dmPelsHeight, m_winCopyDC, m_intSBSOffset + m_intImageSeparation, 0, SRCCOPY);

	//Blit to the screen
	BitBlt(m_hdc, 0, 0, m_RiftDisplayInfo.dmPelsWidth, m_RiftDisplayInfo.dmPelsHeight, m_BackDC, 0, 0, SRCCOPY);
	UpdateWindow(m_hwnd);
}

void Scope::SendRiftSensorData()
{
	PostMessage(m_hwndCaller, WM_RIFT_SENSOR_DATA, RSD_YAW, reinterpret_cast<LPARAM&>(m_fltYaw));
	PostMessage(m_hwndCaller, WM_RIFT_SENSOR_DATA, RSD_PITCH, reinterpret_cast<LPARAM&>(m_fltPitch));
	PostMessage(m_hwndCaller, WM_RIFT_SENSOR_DATA, RSD_ROLL, reinterpret_cast<LPARAM&>(m_fltRoll));
}

UINT Scope::RunScopeWindow(LPVOID pParam)
{
	//pParam is the handle to the calling window
	Scope scope;
	if (SUCCEEDED(scope.Initialize()))
	{
		scope.m_hwndCaller = (HWND)pParam;
		//Send the calling window a handle to the scope window
		PostMessage((HWND)pParam, WM_DESKOPE_SCOPE_HANDLE, (WPARAM)scope.m_hwnd, NULL);
		scope.RunMessageLoop();
		ClipCursor(NULL);
		return 0;
	}
	else
		return -1;
}

DWORD Scope::CloseScopeWindow(HWND hwndScope, PHANDLE hScopeThread)
{
	//Send close message to Scope window then wait for thread to end
	::SendMessage(hwndScope, WM_SYSCOMMAND, SC_CLOSE, 0);
	DWORD result = MsgWaitForMultipleObjects(1, hScopeThread, TRUE, INFINITE, QS_ALLEVENTS);
	*hScopeThread = NULL;
	return result;
}

int Scope::GetRiftDisplayInfo()
{	
	//remove last '\\Monitor#' part from hmdInfo.DisplayDeviceName
	CString DisplayDeviceName(m_hmdInfo.DisplayDeviceName);
	DisplayDeviceName = DisplayDeviceName.Left(DisplayDeviceName.ReverseFind('\\'));

	m_RiftDisplayInfo.dmSize = sizeof(DEVMODE);
	int result = EnumDisplaySettings(DisplayDeviceName, ENUM_CURRENT_SETTINGS, &m_RiftDisplayInfo);
	
	if (!result)
	{
		m_RiftDisplayInfo.dmPelsWidth = DEFAULT_SCREEN_WIDTH;
		m_RiftDisplayInfo.dmPelsHeight = DEFAULT_SCREEN_HEIGHT;
		m_RiftDisplayInfo.dmPosition.x = DEFAULT_DESKTOPX;
		m_RiftDisplayInfo.dmPosition.y = DEFAULT_DESKTOPY;
	}
		//m_RiftDisplayInfo.dmPosition.x = 0;
		//m_RiftDisplayInfo.dmPosition.y = 0;
	
	return result;
}
