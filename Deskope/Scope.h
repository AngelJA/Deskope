#pragma once

#define WM_DESKOPE_SCOPE_HANDLE (WM_APP + 0)
#define WM_DESKOPE_VALUE_CHANGE (WM_APP + 1)
#define WM_RIFT_SENSOR_DATA (WM_APP + 1)

#define RSD_RESET 0
#define RSD_YAW 1
#define RSD_PITCH 2
#define RSD_ROLL 3
#define IMAGE_SEPARATION 4
#define PIXELS_PER_DEGREE 5
#define CLIP_CURSOR 6
#define SBS_OFFSET 7
#define ZOOM 8

#define DEFAULT_DESKTOPX 0
#define DEFAULT_DESKTOPY 0
#define DEFAULT_SCREEN_WIDTH 800
#define DEFAULT_SCREEN_HEIGHT 200
#define DEFAULT_SEPARATION 110
#define DEFAULT_PIXELS_PER_DEGREE 20
#define DEFAULT_CLIP_CURSOR FALSE
#define DEFAULT_ZOOM 1.0

#include <OVR.h>	//Oculus SDK

using namespace OVR;

class Scope
{
public:
	Scope();
	~Scope();    
	
	int Initialize();
	static UINT RunScopeWindow(LPVOID pParam);
	static DWORD CloseScopeWindow(HWND, PHANDLE);

	//delegator windows procedure
	static LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);
	//timer delegator
	static VOID CALLBACK TimerProc(HWND, UINT, UINT_PTR, DWORD);

private:
    void RunMessageLoop();
	int SetUpRift();
	int SetUpWindow();
	int SetUpTimers();
	int GetRiftDisplayInfo();
	void ReleaseRiftResources();
	void AllocateWindowsResources();
	void ReleaseWindowsResources();
	void ResizeSource();
	void KillTimers();
	void DrawScope();
	void SendRiftSensorData();

    // member function message handler, called by static message handler
	LRESULT HandleMessage(UINT message, WPARAM wParam, LPARAM lParam);

	Ptr<DeviceManager> m_pManager;
	Ptr<HMDDevice> m_pHMD;
	Ptr<SensorDevice> m_pSensor;
	SensorFusion m_sFusion;
	HMDInfo m_hmdInfo;

	int m_intSrcWidth;
	int m_intSrcHeight;
	bool m_boolClipCursor;		//whether to restrict cursor
	int m_intSBSOffset;			//offset for side-by-side 3D mode
	float m_fltZoom;			//Zoom factor
	float m_fltYaw;				//yaw from Rift sensor in radians
	float m_fltPitch;			//pitch from Rift sensor in radians
	float m_fltRoll;			//roll from Rift sensor in radians
	int m_intPixelsPerDegree;	//pixels of screen movement per degree of head movement
	int m_intImageSeparation;	//pixels to shift right eye image (IPD adjustment)
	
	UINT_PTR m_uipDrawTimer;	//ID of timer that draws the Scope window
	UINT_PTR m_uipSendRSDTimer;	//ID of timer that sends Rift sensor data to caller window
	HWND m_hwnd;				//hwnd for Scope window
	HWND m_hwndCaller;			//hwnd to caller window
	CURSORINFO m_GlobalCursor;	//mouse cursor info
	DEVMODE m_RiftDisplayInfo;
	HDC m_winDC;				//screen DC
	HDC m_winCopyDC;			//copy of screen for drawing on
	HBITMAP m_winCopyBM;		//copy of screen for drawing on
	HDC m_hdc;					//Scope window DC
	HDC m_BackDC;				//back buffer DC
	HBITMAP m_BackBM;			//back buffer BM
};