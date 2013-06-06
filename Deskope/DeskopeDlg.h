
// DeskopeDlg.h : header file
//

#pragma once
#include "afxcmn.h"
#include "afxwin.h"

#define HK_SBS3D 1
#define HK_CENTER_SCREEN 2
#define HK_RESTRICT_CURSOR 3

// program defaults
#define DEFAULT_SEPARATION 110
#define DEFAULT_SEPARATION_MIN 60
#define DEFAULT_SEPARATION_MAX 160
#define DEFAULT_PPD 20
#define DEFAULT_PPD_MIN 10
#define DEFAULT_PPD_MAX 40
#define DEFAULT_CLIP_CURSOR 0
#define DEFAULT_ZOOM 10
#define DEFAULT_ZOOM_MIN 5
#define DEFAULT_ZOOM_MAX 10
#define DEFAULT_SBS_MODE 0
#define DEFAULT_SBS_WIDTH 1280
#define DEFAULT_DISABLE_COMPOSITION 1
#define DEFAULT_TRACKING_ENABLED 0
#define DEFAULT_SCREEN_CAPTURE_RATE 60

// registry entry names
#define REG_SECTION				L""
#define REG_PPD					L"Rotation Speed"
#define REG_PPD_MIN				L"Rotation Speed Minimum"
#define REG_PPD_MAX				L"Rotation Speed Maximum"
#define REG_ZOOM				L"Zoom"
#define REG_CLIP_CURSOR			L"Restrict Cursor?"
#define REG_SEPARATION			L"Image Separation"
#define REG_SEPARATION_MIN		L"Image Separation Min"
#define REG_SEPARATION_MAX		L"Image Separation Max"
#define REG_DISABLE_COMPOSITION	L"Disable Desktop Composition"
#define REG_ZOOM_MIN			L"Zoom Minimum"
#define REG_ZOOM_MAX			L"Zoom Maximum"
#define REG_SBS_MODE			L"Side by Side Mode?"
#define REG_SBS_WIDTH			L"Side by Side Width"
#define REG_TRACKING_ENABLED	L"Tracking Enabled?"
#define REG_SCREEN_CAP_RATE		L"Screen Capture Rate"

// CDeskopeDlg dialog
class CDeskopeDlg : public CDialogEx
{
// Construction
public:
	CDeskopeDlg(CWnd* pParent = NULL);	// standard constructor

// Dialog Data
	enum { IDD = IDD_DESKOPE_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support


// Implementation
protected:
	HICON m_hIcon;
	// Generated message map functions
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	afx_msg void OnDestroy();
	afx_msg LRESULT ReceiveScopeHandle(WPARAM, LPARAM);
	afx_msg LRESULT ReceiveRiftSensorData(WPARAM, LPARAM);
	afx_msg void OnHScroll(UINT, UINT, CScrollBar*);
	afx_msg void OnHotKey(UINT, UINT, UINT);
	DECLARE_MESSAGE_MAP()

private:
	float m_fltYaw, m_fltPitch, m_fltRoll;
	HANDLE m_hScopeThread;
	HWND m_hwndScope;
	CStringW m_csRiftData;
	CStringW m_csImageSeparation;
	CStringW m_csPixelsPerDegree;
	CStringW m_csZoom;
	CSliderCtrl PixelsPerDegree;
	CSliderCtrl ImageSeparation;
	CButton RestrictCursor;
	CEdit ImageSeparationBox;
	CEdit PixelsPerDegreeBox;
	CSliderCtrl Zoom;
	CButton DisableComposition;
	CButton TrackingEnabled;
	CButton SBS3DMode;
	CEdit SideBySideWidth;
	CEdit ScreenCaptureRate;
	void SendSBSInfo();
	void SendCaptureRate();
	BOOL ToolTipFunction( UINT id, NMHDR * pTTTStruct, LRESULT * pResult );
	// get registry int or write default value to registry and return it
	int GetRegistryInt(CStringW, int);
	afx_msg void OnBnClickedResetOrientation();
	afx_msg void OnBnClickedRestrictcursor();
	afx_msg void OnBnClickedOk();
	afx_msg void OnBnClickedCancel();
	afx_msg void OnBnClickedSidebysidecheck();
	afx_msg void OnEnChangeEdit1();
	afx_msg void OnBnClickedDisableComposition();
	afx_msg void OnBnClickedEnableTracking();
public:
	afx_msg void OnBnClickedApplybutton();
	afx_msg void OnBnClickedHotkeysbutton();
};
