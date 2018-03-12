#pragma once
#include "CWMPPlayer4.h"


// PlayVideo dialog

class PlayVideo : public CDialogEx
{
	DECLARE_DYNAMIC(PlayVideo)

public:
	PlayVideo(CWnd* pParent = NULL);   // standard constructor
	virtual ~PlayVideo();

// Dialog Data
	enum { IDD = IDD_PLAYVIDEO };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual BOOL OnInitDialog();

	DECLARE_MESSAGE_MAP()
public:
	CWMPPlayer4 m_Player;
};
