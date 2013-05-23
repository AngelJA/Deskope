
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
	m_csRiftData.Format(_T("Yaw: %.1f° %cPitch: %.1f° %cRoll: %.1f°"), m_fltYaw, char(10), m_fltPitch, char(10), m_fltRoll); 

	if (ImageSeparation)
		m_csImageSeparation.Format(_T("%d"), ImageSeparation.GetPos());
	if (PixelsPerDegree)
		m_csPixelsPerDegree.Format(_T("%d"), PixelsPerDegree.GetPos());
	if (Zoom)
		m_csZoom.Format(_T("%.1f"), Zoom.GetPos() * 0.1);

	DDX_Text(pDX, IDC_RIFT_SENSOR_DATA, m_csRiftData);
	DDX_Text(pDX, IDC_IMAGE_SEPARATION_BOX, m_csImageSeparation);
	DDX_Text(pDX, IDC_PIXELS_PER_DEGREE_BOX, m_csPixelsPerDegree);
	DDX_Text(pDX, IDC_ZOOM_BOX, m_csZoom);
	DDX_Control(pDX, IDC_IMAGESEPARATION, ImageSeparation);
	DDX_Control(pDX, IDC_PIXELSPERDEGREE, PixelsPerDegree);
	DDX_Control(pDX, IDC_RESTRICTCURSOR, RestrictCursor);
	DDX_Control(pDX, IDC_IMAGE_SEPARATION_BOX, ImageSeparationBox);
	DDX_Control(pDX, IDC_PIXELS_PER_DEGREE_BOX, PixelsPerDegreeBox);
	DDX_Control(pDX, IDC_START_STOP_SCOPE, OnOffButton);
	DDX_Control(pDX, IDC_SideBySideCheck, SBS3DMode);
	DDX_Control(pDX, IDC_SideBySideWidth, SideBySideWidth);
	DDX_Control(pDX, IDC_ZOOM, Zoom);
	DDX_Control(pDX, IDC_DISABLE_AERO, DisableAero);
}

BEGIN_MESSAGE_MAP(CDeskopeDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_DESTROY()
	ON_WM_HSCROLL()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDCANCEL, &CDeskopeDlg::OnBnClickedCancel)
	ON_MESSAGE(WM_DESKOPE_SCOPE_HANDLE, ReceiveScopeHandle)
	ON_MESSAGE(WM_RIFT_SENSOR_DATA, ReceiveRiftSensorData)
	ON_BN_CLICKED(IDOK, &CDeskopeDlg::OnBnClickedOk)
	ON_BN_CLICKED(IDC_START_STOP_SCOPE, &CDeskopeDlg::OnBnClickedStartStopScope)
	ON_BN_CLICKED(IDC_RESTRICTCURSOR, &CDeskopeDlg::OnBnClickedRestrictcursor)
	ON_EN_CHANGE(IDC_PIXELS_PER_DEGREE_BOX, &CDeskopeDlg::OnEnChangePixelsPerDegreeBox)
	ON_EN_CHANGE(IDC_IMAGE_SEPARATION_BOX, &CDeskopeDlg::OnEnChangeImageSeparationBox)
	ON_BN_CLICKED(IDC_RESET_ORIENTATION, &CDeskopeDlg::OnBnClickedResetOrientation)
	ON_BN_CLICKED(IDC_SideBySideCheck, &CDeskopeDlg::OnBnClickedSidebysidecheck)
	ON_EN_CHANGE(IDC_SideBySideWidth, &CDeskopeDlg::OnEnChangeSidebysidewidth)
	ON_BN_CLICKED(IDC_DISABLE_AERO, &CDeskopeDlg::OnBnClickedDisableAero)
END_MESSAGE_MAP()


// CDeskopeDlg message handlers

BOOL CDeskopeDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

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

	//Settings Stuff
		//TEST
		CString strSection       = _T("Settings");
		CString strStringItem    = _T("My String Item");
		CString strIntItem       = _T("My Int Item");

		CWinApp* pApp = &theApp;

		pApp->WriteProfileString(_T("Settings") /*strSection*/, strStringItem, _T("test"));

		CString strValue;
		strValue = pApp->GetProfileString(strSection, strStringItem);
		ASSERT(strValue == _T("test"));

		pApp->WriteProfileInt(strSection, strIntItem, 1234);
		int nValue;
		nValue = pApp->GetProfileInt(strSection, strIntItem, 0);
		ASSERT(nValue == 1234);
	//ENDTEST

	// Initialize controls
	Zoom.SetRange(1, 20);
	Zoom.SetPos(10);
	ImageSeparation.SetRange(DEFAULT_SEPARATION - 50, DEFAULT_SEPARATION + 50, TRUE);
	ImageSeparation.SetPos(DEFAULT_SEPARATION);
	PixelsPerDegree.SetRange(10, 40, TRUE);
	PixelsPerDegree.SetPos(DEFAULT_PIXELS_PER_DEGREE);
	RestrictCursor.SetCheck(DEFAULT_CLIP_CURSOR);
	SideBySideWidth.SetWindowTextW(_T("1280"));

	// Start the Scope window in another thread
	m_hScopeThread = AfxBeginThread(Scope::RunScopeWindow, this->GetSafeHwnd())->m_hThread;

	return TRUE;  // return TRUE  unless you set the focus to a control
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

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

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
	return 0;
}

LRESULT CDeskopeDlg::ReceiveRiftSensorData(WPARAM wParam, LPARAM lParam)
{
	switch(wParam)
	{
	case RSD_YAW:
		m_fltYaw = float(reinterpret_cast<float&>(lParam) * 180.0 / 3.14159);
		break;
	case RSD_PITCH:
		m_fltPitch = float(reinterpret_cast<float&>(lParam) * 180.0 / 3.14159);
		break;
	case RSD_ROLL:
		m_fltRoll = float(reinterpret_cast<float&>(lParam) * 180.0 / 3.14159);
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
	// TODO: Add your control notification handler code here
	CDialogEx::OnCancel();
}



void CDeskopeDlg::OnBnClickedOk()
{
	// TODO: Add your control notification handler code here
	CDialogEx::OnOK();
}


void CDeskopeDlg::OnBnClickedStartStopScope()
{
	if (m_hScopeThread)
		Scope::CloseScopeWindow(m_hwndScope, &m_hScopeThread);
	else
		m_hScopeThread = AfxBeginThread(Scope::RunScopeWindow, this->GetSafeHwnd())->m_hThread;
}

void CDeskopeDlg::OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar)
{
	CSliderCtrl* pSlider = reinterpret_cast<CSliderCtrl*>(pScrollBar);
	if (pSlider == &ImageSeparation)
	{
		::PostMessage(m_hwndScope, WM_DESKOPE_VALUE_CHANGE, IMAGE_SEPARATION, pSlider->GetPos());
	}
	else if (pSlider == &PixelsPerDegree)
	{
		::PostMessage(m_hwndScope, WM_DESKOPE_VALUE_CHANGE, PIXELS_PER_DEGREE, pSlider->GetPos());
	}
	else if (pSlider == &Zoom)
	{
		float fltZoom = float(pSlider->GetPos() * 0.1);
		::PostMessage(m_hwndScope, WM_DESKOPE_VALUE_CHANGE, ZOOM, reinterpret_cast<LPARAM&>(fltZoom));
	}
}

void CDeskopeDlg::OnBnClickedRestrictcursor()
{
	::PostMessage(m_hwndScope, WM_DESKOPE_VALUE_CHANGE, CLIP_CURSOR, RestrictCursor.GetCheck());
}


void CDeskopeDlg::OnEnChangePixelsPerDegreeBox()
{
	CString PPDText;
	PixelsPerDegreeBox.GetWindowTextW(PPDText);
	PixelsPerDegree.SetPos(_wtoi(PPDText));
}


void CDeskopeDlg::OnEnChangeImageSeparationBox()
{
	CString ImageSeparationText;
	ImageSeparationBox.GetWindowTextW(ImageSeparationText);
	ImageSeparation.SetPos(_wtoi(ImageSeparationText));
}


void CDeskopeDlg::OnBnClickedResetOrientation()
{
	::PostMessage(m_hwndScope, WM_DESKOPE_VALUE_CHANGE, RSD_RESET, NULL);
}


void CDeskopeDlg::OnBnClickedSidebysidecheck()
{
	SendSBSInfo();
}

void CDeskopeDlg::OnEnChangeSidebysidewidth()
{
	SendSBSInfo();
}

void CDeskopeDlg::SendSBSInfo()
{
	CString SBSOffsetText;
	SideBySideWidth.GetWindowTextW(SBSOffsetText);
	::PostMessage(m_hwndScope, WM_DESKOPE_VALUE_CHANGE, SBS_OFFSET, SBS3DMode.GetCheck() ? _wtoi(SBSOffsetText) / 2 : 0);
}



void CDeskopeDlg::OnBnClickedDisableAero()
{
	if (DisableAero.GetCheck())
		DwmEnableComposition(DWM_EC_DISABLECOMPOSITION);
	else
		DwmEnableComposition(DWM_EC_ENABLECOMPOSITION);
}
