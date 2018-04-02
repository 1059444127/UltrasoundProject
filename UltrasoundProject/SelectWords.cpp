// SelectWords.cpp : implementation file
//

#include "stdafx.h"
#include "UltrasoundProject.h"
#include "SelectWords.h"
#include "afxdialogex.h"
#include "UltrasoundProjectDlg.h"
#include "PlayVideo.h"


#define IDC_D_BTN 10000

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
}



BEGIN_MESSAGE_MAP(SelectWords, CDialogEx)
	ON_BN_CLICKED(IDC_BUTTON1, &SelectWords::OnBnClickedButton1)
	ON_BN_CLICKED(IDC_BUTTON2, &SelectWords::OnBnClickedButton2)
	ON_COMMAND_RANGE(IDC_D_BTN,IDC_D_BTN+19-1,OnButtonClick)
END_MESSAGE_MAP()

BOOL SelectWords::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	/*
	// TODO: Add extra initialization here
	for (int index = 0; index < wordString.size(); index++) {
		m_listBox.AddString(CA2W(wordString[index].c_str()));  
	//	add_list.AddString(CA2W(pathString[index].c_str()));
		add_vector.push_back(pathString[index]); 
       }
*/
		int buttonSize = wordString.size();

	CButton* btn = new CButton[buttonSize];  
DWORD dwStyle = WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON;  
for(int i = 0; i < buttonSize; i++)
{  
    btn[i].Create(CA2W(wordString[i].c_str()), dwStyle,CRect(20+130*(i%5),20+80*(i/5),140+130*(i%5),60+80*(i/5)),this,IDC_D_BTN+i); 
	add_vector.push_back(pathString[i]); 
} 


    return TRUE;
}

// SelectWords
 void SelectWords::OnButtonClick(UINT uID)
 {
	   int nCursel = uID - IDC_D_BTN;
	   strSelect = add_vector[nCursel];		
	//	but.SetState(!((&but).GetState() & 0x0004));

 }



//Next
void SelectWords::OnBnClickedButton1()
{
	// TODO: Add your control notification handler code here
	PlayVideo pv_dlg;
 //  ShowWindow(SW_HIDE);
	pv_dlg.strPath = strSelect;
	if(strSelect.compare("NULL")!=0)
	{
		pv_dlg.strPath = strSelect;
		pv_dlg.DoModal(); 
	}else{
        MessageBox(_T("Canot find video!"));
	}
}


void SelectWords::OnBnClickedButton2()
{
	// TODO: Add your control notification handler code here
	PostMessage(WM_QUIT,0,0);
}
