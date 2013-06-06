
// DeskopeDlg.cpp : implementation file
//

#include "stdafx.h"
#include "Deskope.h"
#include "DeskopeDlg.h"
#include "afxdialogex.h"
#include "Scope.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CAboutDlg dialog used for App About

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

// Dialog Data
	enum { IDD = IDD_ABOUTBOX };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

// Implementation
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialogEx(CAboutDlg::IDD)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialogEx)
END_MESSAGE_MAP()


// CDeskopeDlg dialog



CDeskopeDlg::CDeskopeDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(CDeskopeDlg::IDD, pParent), m_fltYaw(0), m_fltPitch(0), m_fltRoll(0)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CDeskopeDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	m_csRiftData.Format(L"Yaw: %.1f° %cPitch: %.1f° %cRoll: %.1f°", m_fltYaw, char(10), m_fltPitch, char(10), m_fltRoll); 

	if (ImageSeparation)
		m_csImageSeparation.Format(L"%d", ImageSeparation.GetPos());
	if (PixelsPerDegree)
		m_csPixelsPerDegree.Format(L"%d", PixelsPerDegree.GetPos());
	if (Zoom)
		m_csZoom.Format(L"%.1f", Zoom.GetPos() * 0.1);

	DDX_Text(pDX, IDC_RIFT_SENSOR_DATA, m_csRiftData);
	DDX_Text(pDX, IDC_IMAGE_SEPARATION_BOX, m_csImageSeparation);
	DDX_Text(pDX, IDC_PIXELS_PER_DEGREE_BOX, m_csPixelsPerDegree);
	DDX_Text(pDX, IDC_ZOOM_BOX, m_csZoom);
	DDX_Control(pDX, IDC_IMAGESEPARATION, ImageSeparation);
	DDX_Control(pDX, IDC_PIXELSPERDEGREE, PixelsPerDegree);
	DDX_Control(pDX, IDC_RESTRICTCURSOR, RestrictCursor);
	DDX_Control(pDX, IDC_IMAGE_SEPARATION_BOX, ImageSeparationBox);
	DDX_Control(pDX, IDC_PIXELS_PER_DEGREE_BOX, PixelsPerDegreeBox);
	DDX_Control(pDX, IDC_SideBySideCheck, SBS3DMode);
	DDX_Control(pDX, IDC_SideBySideWidth, SideBySideWidth);
	DDX_Control(pDX, IDC_SCREENCAPTURERATE_BOX, ScreenCaptureRate);
	DDX_Control(pDX, IDC_ZOOM, Zoom);
	DDX_Control(pDX, IDC_DISABLE_COMPOSITION, DisableComposition);
	DDX_Control(pDX, IDC_ENABLE_TRACKING, TrackingEnabled);
}

BEGIN_MESSAGE_MAP(CDeskopeDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_HOTKEY()
	ON_WM_DESTROY()
	ON_WM_HSCROLL()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDCANCEL, &CDeskopeDlg::OnBnClickedCancel)
	ON_MESSAGE(WM_DESKOPE_SCOPE_HANDLE, ReceiveScopeHandle)
	ON_MESSAGE(WM_RIFT_SENSOR_DATA, ReceiveRiftSensorData)
	ON_BN_CLICKED(IDOK, &CDeskopeDlg::OnBnClickedOk)
	ON_BN_CLICKED(IDC_RESTRICTCURSOR, &CDeskopeDlg::OnBnClickedRestrictcursor)
	ON_BN_CLICKED(IDC_RESET_ORIENTATION, &CDeskopeDlg::OnBnClickedResetOrientation)
	ON_BN_CLICKED(IDC_SideBySideCheck, &CDeskopeDlg::OnBnClickedSidebysidecheck)
	ON_BN_CLICKED(IDC_DISABLE_COMPOSITION, &CDeskopeDlg::OnBnClickedDisableComposition)
	ON_BN_CLICKED(IDC_ENABLE_TRACKING, &CDeskopeDlg::OnBnClickedEnableTracking)
	ON_NOTIFY_EX( TTN_NEEDTEXTW, 0, ToolTipFunction )
	ON_BN_CLICKED(IDC_APPLYBUTTON, &CDeskopeDlg::OnBnClickedApplybutton)
	ON_BN_CLICKED(IDC_HOTKEYSBUTTON, &CDeskopeDlg::OnBnClickedHotkeysbutton)
END_MESSAGE_MAP()


// CDeskopeDlg message handlers

BOOL CDeskopeDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	EnableToolTips();

	// Add "About..." menu item to system menu.

	// IDM_ABOUTBOX must be in the system command range.
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		BOOL bNameValid;
		CString strAboutMenu;
		bNameValid = strAboutMenu.LoadString(IDS_ABOUTBOX);
		ASSERT(bNameValid);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dialog
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon

	// Setup hotkeys
	RegisterHotKey(GetSafeHwnd(), HK_SBS3D, MOD_NOREPEAT | MOD_WIN, 0x5A); // Windows + Z
	RegisterHotKey(GetSafeHwnd(), HK_CENTER_SCREEN, MOD_NOREPEAT | MOD_WIN, 0X43); // Windows + C
	RegisterHotKey(GetSafeHwnd(), HK_RESTRICT_CURSOR, MOD_NOREPEAT | MOD_WIN, 0X58); // Windows + X

	// Initialize controls with settings from registry
	Zoom.SetRange(GetRegistryInt(REG_ZOOM_MIN, DEFAULT_ZOOM_MIN),
				  GetRegistryInt(REG_ZOOM_MAX, DEFAULT_ZOOM_MAX));
	Zoom.SetPos(GetRegistryInt(REG_ZOOM, DEFAULT_ZOOM));
	
	ImageSeparation.SetRange(GetRegistryInt(REG_SEPARATION_MIN, DEFAULT_SEPARATION_MIN),
							 GetRegistryInt(REG_SEPARATION_MAX, DEFAULT_SEPARATION_MAX));
	ImageSeparation.SetPos(GetRegistryInt(REG_SEPARATION, DEFAULT_SEPARATION));

	PixelsPerDegree.SetRange(GetRegistryInt(REG_PPD_MIN, DEFAULT_PPD_MIN),
							 GetRegistryInt(REG_PPD_MAX, DEFAULT_PPD_MAX));
	PixelsPerDegree.SetPos(GetRegistryInt(REG_PPD, DEFAULT_PPD));
	
	RestrictCursor.SetCheck(GetRegistryInt(REG_CLIP_CURSOR, DEFAULT_CLIP_CURSOR));
	SBS3DMode.SetCheck(GetRegistryInt(REG_SBS_MODE, DEFAULT_SBS_MODE));
	DisableComposition.SetCheck(GetRegistryInt(REG_DISABLE_COMPOSITION, DEFAULT_DISABLE_COMPOSITION));
	TrackingEnabled.SetCheck(GetRegistryInt(REG_TRACKING_ENABLED, DEFAULT_TRACKING_ENABLED));

	CStringW strSBSWidth;
	strSBSWidth.Format(L"%d", GetRegistryInt(REG_SBS_WIDTH, DEFAULT_SBS_WIDTH));
	SideBySideWidth.SetWindowTextW(strSBSWidth);

	CStringW strCaptureRate;
	strCaptureRate.Format(L"%d", GetRegistryInt(REG_SCREEN_CAP_RATE, DEFAULT_SCREEN_CAPTURE_RATE));
	ScreenCaptureRate.SetWindowTextW(strCaptureRate);

	// Start the Scope window in another thread
	m_hScopeThread = AfxBeginThread(Scope::RunScopeWindow, this->GetSafeHwnd())->m_hThread;

	return TRUE;  // return TRUE  unless you set the focus to a control
}


// get registry int or write default value to registry and return it
int CDeskopeDlg::GetRegistryInt(CStringW strEntry, int intDefault)
{
	int intValue = theApp.GetProfileIntW(REG_SECTION, strEntry, -1);
	if (intValue < 0)
	{
		theApp.WriteProfileInt(REG_SECTION, strEntry, intDefault);
		return intDefault;
	}
	return intValue;
}

void CDeskopeDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialogEx::OnSysCommand(nID, lParam);
	}
}

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void CDeskopeDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // device context for painting

		SendMessageW(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// Center icon in client rectangle
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// Draw the icon
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}

//Close Scope Window when dialog is closed
void CDeskopeDlg::OnDestroy()
{	
	if (m_hScopeThread)
		Scope::CloseScopeWindow(m_hwndScope, &m_hScopeThread);

	CDialogEx::OnDestroy();
}
// Receive handle to scope window
LRESULT CDeskopeDlg::ReceiveScopeHandle(WPARAM wParam, LPARAM lParam)
{
	m_hwndScope = (HWND)wParam;
	::PostMessageW(m_hwndScope, WM_DESKOPE_VALUE_CHANGE, IMAGE_SEPARATION, ImageSeparation.GetPos());
	::PostMessageW(m_hwndScope, WM_DESKOPE_VALUE_CHANGE, PIXELS_PER_DEGREE, PixelsPerDegree.GetPos());
	float fltZoom = float(Zoom.GetPos() * 0.1);
	::PostMessageW(m_hwndScope, WM_DESKOPE_VALUE_CHANGE, ZOOM, reinterpret_cast<LPARAM&>(fltZoom));	
	OnBnClickedRestrictcursor();
	OnBnClickedSidebysidecheck();
	OnBnClickedDisableComposition();
	OnBnClickedEnableTracking();
	SendSBSInfo();
	SendCaptureRate();
	return 0;
}

LRESULT CDeskopeDlg::ReceiveRiftSensorData(WPARAM wParam, LPARAM lParam)
{
	switch(wParam)
	{
	case RSD_YAW:
		m_fltYaw = float(reinterpret_cast<float&>(lParam) * 180.0 / PI);
		break;
	case RSD_PITCH:
		m_fltPitch = float(reinterpret_cast<float&>(lParam) * 180.0 / PI);
		break;
	case RSD_ROLL:
		m_fltRoll = float(reinterpret_cast<float&>(lParam) * 180.0 / PI);
		break;
	}
	
	UpdateData(FALSE);
	return 0;
}

// The system calls this function to obtain the cursor to display while the user drags
//  the minimized window.
HCURSOR CDeskopeDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

void CDeskopeDlg::OnBnClickedCancel()
{
	// save settings
	CStringW strSBSOffset;
	SideBySideWidth.GetWindowTextW(strSBSOffset);
	CStringW strCaptureRate;
	ScreenCaptureRate.GetWindowTextW(strCaptureRate);
	theApp.WriteProfileInt(REG_SECTION, REG_SEPARATION, ImageSeparation.GetPos());
	theApp.WriteProfileInt(REG_SECTION, REG_PPD, PixelsPerDegree.GetPos());
	theApp.WriteProfileInt(REG_SECTION, REG_ZOOM, Zoom.GetPos());
	theApp.WriteProfileInt(REG_SECTION, REG_SBS_MODE, SBS3DMode.GetCheck());
	theApp.WriteProfileInt(REG_SECTION, REG_SBS_WIDTH, _wtoi(strSBSOffset));
	theApp.WriteProfileInt(REG_SECTION, REG_SCREEN_CAP_RATE, _wtoi(strCaptureRate));
	theApp.WriteProfileInt(REG_SECTION, REG_CLIP_CURSOR, RestrictCursor.GetCheck());
	theApp.WriteProfileInt(REG_SECTION, REG_DISABLE_COMPOSITION, DisableComposition.GetCheck());
	theApp.WriteProfileInt(REG_SECTION, REG_TRACKING_ENABLED, TrackingEnabled.GetCheck());

	CDialogEx::OnCancel();
}

void CDeskopeDlg::OnBnClickedOk()
{
	OnBnClickedApplybutton();
}

void CDeskopeDlg::OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar)
{
	CSliderCtrl* pSlider = reinterpret_cast<CSliderCtrl*>(pScrollBar);
	if (pSlider == &ImageSeparation)
	{
		::PostMessageW(m_hwndScope, WM_DESKOPE_VALUE_CHANGE, IMAGE_SEPARATION, pSlider->GetPos());
	}
	else if (pSlider == &PixelsPerDegree)
	{
		::PostMessageW(m_hwndScope, WM_DESKOPE_VALUE_CHANGE, PIXELS_PER_DEGREE, pSlider->GetPos());
	}
	else if (pSlider == &Zoom)
	{
		float fltZoom = float(pSlider->GetPos() * 0.1);
		::PostMessageW(m_hwndScope, WM_DESKOPE_VALUE_CHANGE, ZOOM, reinterpret_cast<LPARAM&>(fltZoom));
	}
}

void CDeskopeDlg::OnBnClickedRestrictcursor()
{
	::PostMessageW(m_hwndScope, WM_DESKOPE_VALUE_CHANGE, CLIP_CURSOR, RestrictCursor.GetCheck());
}

void CDeskopeDlg::OnBnClickedResetOrientation()
{
	::PostMessageW(m_hwndScope, WM_DESKOPE_VALUE_CHANGE, RSD_RESET, NULL);
}


void CDeskopeDlg::OnBnClickedSidebysidecheck()
{
	SendSBSInfo();
}

void CDeskopeDlg::SendSBSInfo()
{
	CStringW strSBSOffset;
	SideBySideWidth.GetWindowTextW(strSBSOffset);
	::PostMessageW(m_hwndScope, WM_DESKOPE_VALUE_CHANGE, SBS_OFFSET, SBS3DMode.GetCheck() ? _wtoi(strSBSOffset) / 2 : 0);
}

void CDeskopeDlg::SendCaptureRate()
{
	CStringW strCaptureRate;
	ScreenCaptureRate.GetWindowTextW(strCaptureRate);
	int intCaptureRate = _wtoi(strCaptureRate);
	if (intCaptureRate < 1 || intCaptureRate > 1000) {
		MessageBoxW(L"Invalid frame rate.", L"Error", MB_OK | MB_ICONERROR | MB_SYSTEMMODAL);
	}
	else
		::PostMessage(m_hwndScope, WM_DESKOPE_VALUE_CHANGE, SCREENCAPTURERATE, intCaptureRate);
}

void CDeskopeDlg::OnBnClickedDisableComposition()
{
	if (DisableComposition.GetCheck())
		DwmEnableComposition(DWM_EC_DISABLECOMPOSITION);
	else
		DwmEnableComposition(DWM_EC_ENABLECOMPOSITION);
}


void CDeskopeDlg::OnBnClickedEnableTracking()
{
	::PostMessageW(m_hwndScope, WM_DESKOPE_VALUE_CHANGE, TRACKING, TrackingEnabled.GetCheck());
}

// set tool tip text depending on the control being hovered over
BOOL CDeskopeDlg::ToolTipFunction(UINT id, NMHDR* pTTTStruct, LRESULT* pResult)
{
	TOOLTIPTEXT *pTTT = (TOOLTIPTEXT *)pTTTStruct;
	
	// for multi-line tooltips
	::SendMessageW(pTTT->hdr.hwndFrom, TTM_SETMAXTIPWIDTH, 0, 150);

	if (pTTT->uFlags & TTF_IDISHWND)
	{
		UINT_PTR ControlID = ::GetDlgCtrlID((HWND)pTTTStruct->idFrom);

		PWCHAR OutputString = L"";
		if (ControlID == TrackingEnabled.GetDlgCtrlID())
			OutputString = L"Turn tracking off before launching a Rift app to avoid conflicts.";
		if (ControlID == RestrictCursor.GetDlgCtrlID())
			OutputString = L"Restrict the mouse cursor so that it stays within your field of view.";
		if (ControlID == SBS3DMode.GetDlgCtrlID())
			OutputString = L"View side by side 3D content. Make sure to set the image width.";
		if (ControlID == DisableComposition.GetDlgCtrlID())
			OutputString = L"Deskope works best with desktop composition off.\n(Vista and 7 only)";
		if (ControlID == ImageSeparation.GetDlgCtrlID())
			OutputString = L"Adjust to match your interpupillary distance (IPD).";
		if (ControlID == PixelsPerDegree.GetDlgCtrlID())
			OutputString = L"Pixels of screen movement per degree of head rotation.";
		if (ControlID == ScreenCaptureRate.GetDlgCtrlID())
			OutputString = L"Experimental\nRate at which the screen is captured (Deskope always tries to redraw at 60fps).";
		if (ControlID == SideBySideWidth.GetDlgCtrlID())
			OutputString = L"Set this to the total width (including left and right image) of SBS 3D image.";
		pTTT->lpszText = OutputString;
		return(TRUE);
	}
	return(FALSE);
}

void CDeskopeDlg::OnBnClickedApplybutton()
{
	SendSBSInfo();
	SendCaptureRate();
}

void CDeskopeDlg::OnHotKey(UINT nHotKeyId, UINT nKey1, UINT nKey2)
{
	if (nHotKeyId == HK_SBS3D) {
		SBS3DMode.SetCheck(!SBS3DMode.GetCheck());
		OnBnClickedSidebysidecheck();
	}
	if (nHotKeyId == HK_CENTER_SCREEN) {
		OnBnClickedResetOrientation();
	}
	if (nHotKeyId == HK_RESTRICT_CURSOR) {
		RestrictCursor.SetCheck(!RestrictCursor.GetCheck());
		OnBnClickedRestrictcursor();
	}
}

void CDeskopeDlg::OnBnClickedHotkeysbutton()
{
	MessageBoxW(L"Pressing the Windows Key and a letter will trigger the following actions: \nZ : Toggle SBS 3D Mode\nX : Toggle Restricted Cursor\nC : Recenter Screen", L"Hotkeys", MB_ICONQUESTION | MB_OK | MB_SYSTEMMODAL);
}
