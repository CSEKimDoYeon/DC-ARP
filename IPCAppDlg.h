// IPCAppDlg.h : header file
//

#if !defined(AFX_IPCAPPDLG_H__FE9C37CB_4C90_4772_99AD_50A9B24BCE62__INCLUDED_)
#define AFX_IPCAPPDLG_H__FE9C37CB_4C90_4772_99AD_50A9B24BCE62__INCLUDED_

#include "LayerManager.h"
#include "EthernetLayer.h"	
#include "IPLayer.h"
#include "NILayer.h"
#include "ARPLayer.h"
#include "ProxyTable.h"
#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

////////////////////////////////////////////////////////////////////////////
// CIPCAppDlg dialog

class CIPCAppDlg 
: public CDialog, 
public CBaseLayer
{
	// Construction
public:
	CIPCAppDlg(CWnd* pParent = NULL);	// standard constructor
	
	CWinThread*	m_ReceiveThread;//ARP 메시지를 받는 쓰레드변수
	CWinThread*	m_printThread;//cache table을 주기적으로 갱신하는 쓰레드변수
	static UINT ReceiveThread(LPVOID pParam);//ARP받는 쓰레드
	static UINT PrintThread(LPVOID pParam);//화면 갱신 쓰레드

	// Dialog Data
	//{{AFX_DATA(CIPCAppDlg)
	enum { IDD = IDD_IPC_DIALOG };

	CComboBox	m_NICSet;//랜카드를 가져오는 변수
	CListBox	m_list_arp;//Cache Table에서 List를 갖는 변수
	CListBox	m_list_proxy;//Proxy Table에서 List를 갖는 변수
	CString	m_stSrcAdd;//선택한 랜 카드의 Source Mac Address
	CString	Gratuitous_ARP;//Gratuitous로 전달할 변경된 MAC주소를 갖는 변수
	CIPAddressCtrl m_dst;//ARP할 IP Address
	CIPAddressCtrl m_src;//내 IP Address
	
	//}}AFX_DATA
	
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CIPCAppDlg)
	
protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support
	//}}AFX_VIRTUAL
	
	// Implementation
protected:
	HICON m_hIcon;
	
	// Generated message map functions
	//{{AFX_MSG(CIPCAppDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	afx_msg void OnButtonAddrSet();

	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
		
public:
	void			OnTimer(UINT nIDEvent);//시간 초과시 발생시킬 이벤트
	BOOL			ConvertHex(CString cs, unsigned char* hex);//입력받은 MAC주소를 Cstring->char으로 변환
	void PrintCacheTable();//Cache Table 출력
	void PrintProxyTable();//Proxy Table 출력
	int				check;//ARP를 보냈는지 체크
	unsigned char*	temp;//임시로 IP를 저장할 변수

private:	
	CLayerManager	m_LayerMgr;
	int				m_nAckReady;
	enum {			IPC_INITIALIZING, 
		IPC_WAITFORACK,
		IPC_ERROR,
		IPC_ADDR_SET,
		IPC_ADDR_RESET,
		NOTSETADDRESS,
		FILE_TRANS,
		FILE_CALCEL} ;
	
	void			SetDlgState( int state );
	inline void		EndofProcess( );
	inline void		SetRegstryMessage( );
	
	BOOL			m_bSendReady;
	BOOL			Receive(unsigned char* ppayload1,unsigned char* ppayload2);
	unsigned char*	m_SrcMacAddress;	// 시작지의 Mac주소를 가리키는 포인터
	unsigned char	m_GraMacAddress[6];	// 목적지의 Mac주소
	unsigned char	m_ProxyMacAddress[6];//Proxy Mac 저장 변수
	unsigned char	m_ProxyAddress[4];//Proxy IP 저장 변수
	
	// 각 Layer를 가르키는 객체
	CNILayer* m_NI ;	
	CEthernetLayer* m_Ether;
	CIPLayer* m_IP;
	CARPLayer* m_ARP;

public:
//	afx_msg void OnBnClickedButton3();
//	afx_msg void OnDropdownTestPacket(NMHDR *pNMHDR, LRESULT *pResult);
//	afx_msg void OnBnClickedTestPacket();

	afx_msg void OnBnClickedSend();
	afx_msg void OnComboNIC(); //수정
	afx_msg void OnBnClickeddelete();
	afx_msg void OnBnClickedalldelete();
	afx_msg void OnBnClickedOk();
	afx_msg void OnBnClickedGratuitousArp();
	afx_msg void OnBnClickedProxyAdd();
	afx_msg void OnBnClickedProxyDelete();
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately pre the previous line.

#endif // !defined(AFX_IPCAPPDLG_H__FE9C37CB_4C90_4772_99AD_50A9B24BCE62__INCLUDED_)
