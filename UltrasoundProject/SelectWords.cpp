// SelectWords.cpp : implementation file
//

#include "stdafx.h"
#include "UltrasoundProject.h"
#include "SelectWords.h"
#include "afxdialogex.h"
#include "UltrasoundProjectDlg.h"
#include "PlayVideo.h"


// SelectWords dialog

IMPLEMENT_DYNAMIC(SelectWords, CDialogEx)

SelectWords::SelectWords(CWnd* pParent /*=NULL*/)
	: CDialogEx(SelectWords::IDD, pParent)
{
}

SelectWords::~SelectWords()
{
}

void SelectWords::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST1, m_listBox);
}

BEGIN_MESSAGE_MAP(SelectWords, CDialogEx)
	ON_LBN_SELCHANGE(IDC_LIST1, &SelectWords::OnLbnSelchangeList1)
	ON_BN_CLICKED(IDC_BUTTON1, &SelectWords::OnBnClickedButton1)
END_MESSAGE_MAP()

BOOL SelectWords::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	for (int index = 0; index < wordString.size(); index++) {
		m_listBox.AddString(CA2W(wordString[index].c_str()));         
       }
    // TODO: Add extra initialization here
	//m_listBox.AddString(_T("Korea Career"));         
 //   m_listBox.AddString(_T("Pull Pool"));       
 //   m_listBox.AddString(_T("Virtual")); 
 //
    return TRUE;
}

// SelectWords message handlers


void SelectWords::OnLbnSelchangeList1()
{
	// TODO: Add your control notification handler code here
	CString strText;   
    int nCurSel;   
        
    nCurSel = m_listBox.GetCurSel();            
    m_listBox.GetText(nCurSel, strText);       
    SetDlgItemText(IDC_EDIT1, strText);
}

//Next
void SelectWords::OnBnClickedButton1()
{
	// TODO: Add your control notification handler code here
	PlayVideo pv_dlg;
 //  ShowWindow(SW_HIDE);
	 pv_dlg.DoModal(); 
}
