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
	
	CWinThread*	m_ReceiveThread;//ARP �޽����� �޴� �����庯��
	CWinThread*	m_printThread;//cache table�� �ֱ������� �����ϴ� �����庯��
	static UINT ReceiveThread(LPVOID pParam);//ARP�޴� ������
	static UINT PrintThread(LPVOID pParam);//ȭ�� ���� ������

	// Dialog Data
	//{{AFX_DATA(CIPCAppDlg)
	enum { IDD = IDD_IPC_DIALOG };

	CComboBox	m_NICSet;//��ī�带 �������� ����
	CListBox	m_list_arp;//Cache Table���� List�� ���� ����
	CListBox	m_list_proxy;//Proxy Table���� List�� ���� ����
	CString	m_stSrcAdd;//������ �� ī���� Source Mac Address
	CString	Gratuitous_ARP;//Gratuitous�� ������ ����� MAC�ּҸ� ���� ����
	CIPAddressCtrl m_dst;//ARP�� IP Address
	CIPAddressCtrl m_src;//�� IP Address
	
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
	void			OnTimer(UINT nIDEvent);//�ð� �ʰ��� �߻���ų �̺�Ʈ
	BOOL			ConvertHex(CString cs, unsigned char* hex);//�Է¹��� MAC�ּҸ� Cstring->char���� ��ȯ
	void PrintCacheTable();//Cache Table ���
	void PrintProxyTable();//Proxy Table ���
	int				check;//ARP�� ���´��� üũ
	unsigned char*	temp;//�ӽ÷� IP�� ������ ����

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
	unsigned char*	m_SrcMacAddress;	// �������� Mac�ּҸ� ����Ű�� ������
	unsigned char	m_GraMacAddress[6];	// �������� Mac�ּ�
	unsigned char	m_ProxyMacAddress[6];//Proxy Mac ���� ����
	unsigned char	m_ProxyAddress[4];//Proxy IP ���� ����
	
	// �� Layer�� ����Ű�� ��ü
	CNILayer* m_NI ;	
	CEthernetLayer* m_Ether;
	CIPLayer* m_IP;
	CARPLayer* m_ARP;

public:
//	afx_msg void OnBnClickedButton3();
//	afx_msg void OnDropdownTestPacket(NMHDR *pNMHDR, LRESULT *pResult);
//	afx_msg void OnBnClickedTestPacket();

	afx_msg void OnBnClickedSend();
	afx_msg void OnComboNIC(); //����
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
