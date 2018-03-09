#pragma once
#include "afxwin.h"


// SelectWords dialog

class SelectWords : public CDialogEx
{
	DECLARE_DYNAMIC(SelectWords)

public:
	SelectWords(CWnd* pParent = NULL);   // standard constructor
	virtual ~SelectWords();

// Dialog Data
	enum { IDD = IDD_SELECTWORDS };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
    virtual BOOL OnInitDialog();
	DECLARE_MESSAGE_MAP()
public:
    CListBox m_listBox;
	afx_msg void OnLbnSelchangeList1();
};
