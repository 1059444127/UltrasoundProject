#pragma once

#include <opencv2/opencv.hpp>
#include "afxwin.h"
// LiveCam dialog

class LiveCam : public CDialogEx
{
	DECLARE_DYNAMIC(LiveCam)

public:
	LiveCam(CWnd* pParent = NULL);   // standard constructor
	virtual ~LiveCam();

// Dialog Data
	enum { IDD = IDD_LIVECAM };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual BOOL OnInitDialog();
	DECLARE_MESSAGE_MAP()
public:
	bool flag;
	afx_msg void OnBnClickedOpen();
	afx_msg void OnBnClickedClose();
	afx_msg void OnBnClickedMove();
	afx_msg void OnBnClickedScale();
	CEdit m_Edit_Scale;
	CEdit m_Edit_Rotate;
	afx_msg void OnBnClickedRotate();
};
