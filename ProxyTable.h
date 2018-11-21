#pragma once


// ProxyTable 대화 상자입니다.

class ProxyTable : public CDialogEx
{
	DECLARE_DYNAMIC(ProxyTable)

public:
	ProxyTable(CWnd* pParent = NULL);   // 표준 생성자입니다.
	virtual ~ProxyTable();

	CIPAddressCtrl m_dst;
	CString	Proxy_ARP;
	BOOL ConvertHex(CString cs, unsigned char* hex);//16진수변환
	void Change();
	unsigned char temp[4];
	unsigned char	m_DstMacAddress[6];
	
// 대화 상자 데이터입니다.
	enum { IDD = IDD_Proxy };
//
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedOk();
};
