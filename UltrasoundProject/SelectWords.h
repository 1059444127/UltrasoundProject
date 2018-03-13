#pragma once
#include "afxwin.h"
#include <vector>
#include <string>

using namespace std;
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
	afx_msg void OnBnClickedButton1();
	string strSelect;
	vector<string> add_vector;
	afx_msg void OnBnClickedButton2();
};
