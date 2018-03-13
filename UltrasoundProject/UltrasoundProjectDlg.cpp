
// UltrasoundProjectDlg.cpp : implementation file
//

#include "stdafx.h"
#include "UltrasoundProject.h"
#include "UltrasoundProjectDlg.h"
#include "afxdialogex.h"
#include "SelectWords.h"
#include "LiveConnection.h"
#include <winsock.h> //need to put before mysql.hwordString
#include <mysql.h>//console project need to include <winsock.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


vector<string> wordString;
vector<string> pathString;

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


// CUltrasoundProjectDlg dialog



CUltrasoundProjectDlg::CUltrasoundProjectDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(CUltrasoundProjectDlg::IDD, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CUltrasoundProjectDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CUltrasoundProjectDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDOK, &CUltrasoundProjectDlg::OnBnClickedOk)
END_MESSAGE_MAP()


// CUltrasoundProjectDlg message handlers

BOOL CUltrasoundProjectDlg::OnInitDialog()
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

	// TODO: Add extra initialization here 

	return TRUE;  // return TRUE  unless you set the focus to a control
}

void CUltrasoundProjectDlg::OnSysCommand(UINT nID, LPARAM lParam)
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

void CUltrasoundProjectDlg::OnPaint()
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

// The system calls this function to obtain the cursor to display while the user drags
//  the minimized window.
HCURSOR CUltrasoundProjectDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}



void CUltrasoundProjectDlg::OnBnClickedOk()
{
	// TODO: Add your control notification handler code here
	SelectWords sw_dlg;
   ShowWindow(SW_HIDE);
   //show select words dialog
 //  sw_dlg.DoModal(); 

  // LiveConnection();

   MYSQL *pConn;
 pConn = mysql_init(NULL);
 //pConn, server address, username, password, dabatase name, mysql port number(0 meanas default 3306)
 if(!mysql_real_connect(pConn,"localhost","root","","ultrasound_words",0,NULL,0))
 {  
	   MessageBox(_T("Fail to connet to database!"));

//  printf("cannot connet to database:%s",mysql_error(pConn));
  return;
 }

 if(mysql_query(pConn,"select * from words"))
 {
	  MessageBox(_T("Query failed!"));
 // printf("Query failed:%s",mysql_error(pConn));
  return;
 }else{

  //printf("success");
//  AfxMessageBox(_T("success!"));

 }

 //mysql_store_result是把查询结果一次性取到客户端的离线数据集，当结果比较大时耗内存。
 //mysql_use_result则是查询结果放在服务器上，客户端通过指针逐行读取，节省客户端内存。但是一个MYSQL*连接同时只能有一个未关闭的mysql_use_result查询
 MYSQL_RES *result = mysql_store_result(pConn);
 MYSQL_ROW row;
 while(row = mysql_fetch_row(result))
 {
 // printf("%s %s\n",row[1],row[2]);
	// sw_dlg.m_listBox.AddString(CA2W(row[1]));
	 wordString.push_back(row[1]);
	 char* temp = row[2];
	 if(temp == NULL){
	 pathString.push_back("NULL");
	 }else{
	 pathString.push_back(row[2]);
	 }
	 
 }

 mysql_free_result(result);
 mysql_close(pConn);
    sw_dlg.DoModal(); 

}
