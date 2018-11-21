#pragma once

// GratitousAlert 대화 상자입니다.

class GratitousAlert : public CDialogEx
{
	DECLARE_DYNAMIC(GratitousAlert)

public:
	GratitousAlert(CWnd* pParent = NULL);   // 표준 생성자입니다.
	virtual ~GratitousAlert();

// 대화 상자 데이터입니다.
	enum { IDD = IDD_DIALOG1 };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	DECLARE_MESSAGE_MAP()
};
