// PlayVideo.cpp : implementation file
//

#include "stdafx.h"
#include "UltrasoundProject.h"
#include "PlayVideo.h"
#include "afxdialogex.h"


// PlayVideo dialog

IMPLEMENT_DYNAMIC(PlayVideo, CDialogEx)

PlayVideo::PlayVideo(CWnd* pParent /*=NULL*/)
	: CDialogEx(PlayVideo::IDD, pParent)
{

}

PlayVideo::~PlayVideo()
{
}

void PlayVideo::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_OCX1, m_Player);
}


BEGIN_MESSAGE_MAP(PlayVideo, CDialogEx)
END_MESSAGE_MAP()


BOOL PlayVideo::OnInitDialog()
{
	CDialogEx::OnInitDialog();
    // TODO: Add extra initialization here
//	m_Player.put_URL(_T("F:\\FullSizeRender.avi"));
	if(strPath.compare("NULL")!=0)
	{
	    m_Player.put_URL(CA2W(strPath.c_str()));
	}
    return TRUE;
}
// PlayVideo message handlers
