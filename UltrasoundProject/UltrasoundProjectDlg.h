
// UltrasoundProjectDlg.h : header file
//
#include <iostream> 
#include <vector>
#include <string>
#pragma once


using namespace std;
extern	vector<string> wordString;

// CUltrasoundProjectDlg dialog
class CUltrasoundProjectDlg : public CDialogEx
{
// Construction
public:
	CUltrasoundProjectDlg(CWnd* pParent = NULL);	// standard constructor

// Dialog Data
	enum { IDD = IDD_ULTRASOUNDPROJECT_DIALOG };

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
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedOk();
};
