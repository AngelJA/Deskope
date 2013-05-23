
// DeskopeDlg.h : header file
//

#pragma once
#include "afxcmn.h"
#include "afxwin.h"


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
	DECLARE_MESSAGE_MAP()

private:
	float m_fltYaw, m_fltPitch, m_fltRoll;
	HANDLE m_hScopeThread;
	HWND m_hwndScope;
	CString m_csRiftData;
	CString m_csImageSeparation;
	CString m_csPixelsPerDegree;
	CString m_csZoom;
	CSliderCtrl PixelsPerDegree;
	CSliderCtrl ImageSeparation;
	CButton RestrictCursor;
	CEdit ImageSeparationBox;
	CEdit PixelsPerDegreeBox;
	CButton OnOffButton;
	CSliderCtrl Zoom;
	CButton DisableAero;
	CButton SBS3DMode;
	CEdit SideBySideWidth;
	void SendSBSInfo();
	afx_msg void OnEnChangeImageSeparationBox();
	afx_msg void OnBnClickedResetOrientation();
	afx_msg void OnBnClickedRestrictcursor();
	afx_msg void OnEnChangePixelsPerDegreeBox();
	afx_msg void OnBnClickedOk();
	afx_msg void OnBnClickedStartStopScope();
	afx_msg void OnBnClickedCancel();
	afx_msg void OnBnClickedSidebysidecheck();
	afx_msg void OnEnChangeEdit1();
	afx_msg void OnBnClickedDisableAero();
	afx_msg void OnEnChangeSidebysidewidth();
};
