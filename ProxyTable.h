#pragma once


// ProxyTable ��ȭ �����Դϴ�.

class ProxyTable : public CDialogEx
{
	DECLARE_DYNAMIC(ProxyTable)

public:
	ProxyTable(CWnd* pParent = NULL);   // ǥ�� �������Դϴ�.
	virtual ~ProxyTable();

	CIPAddressCtrl m_dst;
	CString	Proxy_ARP;
	BOOL ConvertHex(CString cs, unsigned char* hex);//16������ȯ
	void Change();
	unsigned char temp[4];
	unsigned char	m_DstMacAddress[6];
	
// ��ȭ ���� �������Դϴ�.
	enum { IDD = IDD_Proxy };
//
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV �����Դϴ�.

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedOk();
};
