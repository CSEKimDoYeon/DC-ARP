// ProxyTable.cpp : 구현 파일입니다.

#include "stdafx.h"
#include "ipc.h"
#include "ProxyTable.h"
#include "afxdialogex.h"


// ProxyTable 대화 상자입니다.

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

// ProxyTable 메시지 처리기입니다.
//16진수 변환
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
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	UpdateData( TRUE ) ;
	CString m_EtherAddr;

	Proxy_ARP.MakeLower();	// 소문자로 변환
	ConvertHex(Proxy_ARP, m_DstMacAddress);

	m_dst.GetAddress(temp[0],temp[1],temp[2],temp[3]);

	UpdateData( FALSE ) ;
}

void ProxyTable::OnBnClickedOk()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	CDialogEx::OnOK();
	Change();
}
