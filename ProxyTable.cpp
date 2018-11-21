// ProxyTable.cpp : ���� �����Դϴ�.

#include "stdafx.h"
#include "ipc.h"
#include "ProxyTable.h"
#include "afxdialogex.h"


// ProxyTable ��ȭ �����Դϴ�.

IMPLEMENT_DYNAMIC(ProxyTable, CDialogEx)

ProxyTable::ProxyTable(CWnd* pParent /*=NULL*/)
	: CDialogEx(ProxyTable::IDD, pParent)
{

}

ProxyTable::~ProxyTable()
{
}

void ProxyTable::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_Proxy_IP, m_dst);
	DDX_Text(pDX, IDC_Proxy_Ether, Proxy_ARP);
}

BEGIN_MESSAGE_MAP(ProxyTable, CDialogEx)
	ON_BN_CLICKED(IDOK, &ProxyTable::OnBnClickedOk)
END_MESSAGE_MAP()

// ProxyTable �޽��� ó�����Դϴ�.
//16���� ��ȯ
BOOL ProxyTable::ConvertHex(CString cs, unsigned char* hex)//change string to hex
{
	int i;
	char* srcStr = cs.GetBuffer(0);

	for (i = 0; i<12; i++){
		// error
		if (srcStr[i] < '0' || (srcStr[i] > '9' && srcStr[i] < 'a') || srcStr[i] > 'f')
			return FALSE;
	}
	for (i = 0; i<12; i = i + 2){
		hex[i / 2] = (((srcStr[i] > '9') ? (srcStr[i] - 87) : (srcStr[i] - '0')) << 4 |
			((srcStr[i + 1] > '9') ? (srcStr[i + 1] - 87) : (srcStr[i + 1] - '0')));
	}
	return TRUE;
}

void ProxyTable::Change()
{
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
	UpdateData( TRUE ) ;
	CString m_EtherAddr;

	Proxy_ARP.MakeLower();	// �ҹ��ڷ� ��ȯ
	ConvertHex(Proxy_ARP, m_DstMacAddress);

	m_dst.GetAddress(temp[0],temp[1],temp[2],temp[3]);

	UpdateData( FALSE ) ;
}

void ProxyTable::OnBnClickedOk()
{
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
	CDialogEx::OnOK();
	Change();
}
