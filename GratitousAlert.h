#pragma once

// GratitousAlert ��ȭ �����Դϴ�.

class GratitousAlert : public CDialogEx
{
	DECLARE_DYNAMIC(GratitousAlert)

public:
	GratitousAlert(CWnd* pParent = NULL);   // ǥ�� �������Դϴ�.
	virtual ~GratitousAlert();

// ��ȭ ���� �������Դϴ�.
	enum { IDD = IDD_DIALOG1 };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV �����Դϴ�.

	DECLARE_MESSAGE_MAP()
};
