#pragma once
class LiveConnection: public CDialogEx
{
protected:
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnButtonClick(UINT uID);
	LiveConnection(void);
	virtual ~LiveConnection(void);
};

