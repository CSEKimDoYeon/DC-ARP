// IPCAppDlg.cpp : implementation file
//

#include "StdAfx.h"
#include "ipc.h"
#include "IPCAppDlg.h"
#include "GratitousAlert.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE [] = __FILE__;
#endif

////////////////////////////////////////////////////////////////////////////
// CAboutDlg dialog used for App About

class CAboutDlg : public CDialog
{
public:
	CAboutDlg();

	// Dialog Data
	//{{AFX_DATA(CAboutDlg)
	enum { IDD = IDD_ABOUTBOX };
	//}}AFX_DATA

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CAboutDlg)
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

	// Implementation
protected:
	//{{AFX_MSG(CAboutDlg)
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialog(CAboutDlg::IDD)
{
	//{{AFX_DATA_INIT(CAboutDlg)
	//}}AFX_DATA_INIT
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CAboutDlg)
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialog)
	//{{AFX_MSG_MAP(CAboutDlg)
	// No message handlers
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CIPCAppDlg dialog

CIPCAppDlg::CIPCAppDlg(CWnd* pParent /*=NULL*/)
: CDialog(CIPCAppDlg::IDD, pParent),
CBaseLayer("ARPDlg"),
m_bSendReady(FALSE),
m_nAckReady(-1)
{
	//{{AFX_DATA_INIT(CIPCAppDlg)
	//�ʱ�ȭ
	m_stSrcAdd = _T("");
	check=0;
	//}}AFX_DATA_INIT
	// Note that LoadIcon does not require a subsequent DestroyIcon in Win32
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);

	//���̾� ����
	m_LayerMgr.AddLayer(new CEthernetLayer("Ethernet"));
	m_LayerMgr.AddLayer(new CIPLayer("IP"));
	m_LayerMgr.AddLayer(new CARPLayer("ARP"));
	m_LayerMgr.AddLayer(new CNILayer("NI"));	
	m_LayerMgr.AddLayer(this);

	//���̾�鰣 ����
	m_LayerMgr.ConnectLayers("NI ( *Ethernet ( *ARP ) )");
	m_LayerMgr.GetLayer( "IP" )->SetUnderLayer(m_LayerMgr.GetLayer( "ARP" ) );
	m_LayerMgr.GetLayer( "Ethernet" )->SetUpperLayer(m_LayerMgr.GetLayer( "IP" ) );
	m_LayerMgr.GetLayer( "IP" )->SetUpperLayer(m_LayerMgr.GetLayer( "ARPDlg" ) );
	m_LayerMgr.GetLayer( "ARPDlg" )->SetUnderLayer(m_LayerMgr.GetLayer( "IP" ) );

	//���̾ ����Ű�� ���� ����
	m_NI = (CNILayer*) m_LayerMgr.GetLayer("NI");
	m_Ether = (CEthernetLayer*) m_LayerMgr.GetLayer("Ethernet");
	m_IP = (CIPLayer*) m_LayerMgr.GetLayer("IP");
	m_ARP = (CARPLayer*) m_LayerMgr.GetLayer("ARP");
}

void CIPCAppDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CIPCAppDlg)
	DDX_Control(pDX, IDC_COMBO_NIC, m_NICSet);
	DDX_Control(pDX, IDC_LIST_CHAT, m_list_arp);
	DDX_Control(pDX, IDC_IP_ADDRESS, m_dst);	
	DDX_Control(pDX, IDC_IP_src_ADDRESS, m_src);
	DDX_Control(pDX, IDC_proxy_LIST, m_list_proxy);
	DDX_Text(pDX, IDC_EDIT_SRC, m_stSrcAdd);
	DDX_Text(pDX, IDC_GRA_MAC, Gratuitous_ARP);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CIPCAppDlg, CDialog)
	//{{AFX_MSG_MAP(CIPCAppDlg)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_BUTTON_ADDR, OnButtonAddrSet)

	ON_WM_TIMER()
	//}}AFX_MSG_MAP
	//ON_NOTIFY(BCN_DROPDOWN, IDC_TEST_PACKET, &CIPCAppDlg::OnDropdownTestPacket)
//	ON_BN_CLICKED(IDC_TEST_PACKET, &CIPCAppDlg::OnBnClickedTestPacket)

ON_BN_CLICKED(IDC_BUTTON4, &CIPCAppDlg::OnBnClickedSend)
ON_BN_CLICKED(IDC_delete, &CIPCAppDlg::OnBnClickeddelete)
ON_BN_CLICKED(IDC_all_delete, &CIPCAppDlg::OnBnClickedalldelete)
ON_BN_CLICKED(IDOK, &CIPCAppDlg::OnBnClickedOk)
ON_BN_CLICKED(IDC_Gratuitous_ARP, &CIPCAppDlg::OnBnClickedGratuitousArp)
ON_BN_CLICKED(IDC_Proxy_Add, &CIPCAppDlg::OnBnClickedProxyAdd)
ON_BN_CLICKED(IDC_Proxy_Delete, &CIPCAppDlg::OnBnClickedProxyDelete)
ON_CBN_SELCHANGE(IDC_COMBO_NIC, OnComboNIC)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CIPCAppDlg message handlers

BOOL CIPCAppDlg::OnInitDialog()
{
	CDialog::OnInitDialog();
	// Add "About..." menu item to system menu.

	// IDM_ABOUTBOX must be in the system command range.
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		CString strAboutMenu;
		strAboutMenu.LoadString(IDS_ABOUTBOX);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dialog
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon

	// TODO: Add extra initialization here
	SetDlgState(IPC_INITIALIZING);

	for (int i = 0; i < m_NI -> getAdapterNum(); i++){
		m_NICSet.AddString(m_NI -> getAdapter(i)->description);
	}
	return TRUE;  // return TRUE  unless you set the focus to a control
}

void CIPCAppDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if (nID == SC_CLOSE)
	{
		if (MessageBox("�����Ͻðڽ��ϱ�?",
			"Question",
			MB_YESNO | MB_ICONQUESTION)
			== IDNO)
			return;
		else EndofProcess();
	}

	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialog::OnSysCommand(nID, lParam);
	}
}

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void CIPCAppDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // device context for painting

		SendMessage(WM_ICONERASEBKGND, (WPARAM) dc.GetSafeHdc(), 0);

		// Center icon in client rectangle
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// Draw the icon
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialog::OnPaint();
	}
}

// The system calls this to obtain the cursor to display while the user drags
//  the minimized window.
HCURSOR CIPCAppDlg::OnQueryDragIcon()
{
	return (HCURSOR) m_hIcon;
}
//event by message Send button

//�ּ� ����
void CIPCAppDlg::OnButtonAddrSet()
{
	UpdateData(TRUE);

	if (!m_bSendReady){
		//������ �������
		m_bSendReady = TRUE;
		SetDlgState(IPC_ADDR_SET);
		//���õ� NIC�� MAC�ּҸ� NI���̾�� ������
		m_NI->setAdapterNum(m_NICSet.GetCurSel());
		m_NI->setMacAddress();//Mac�ּ� ����
		m_SrcMacAddress = m_NI->getMacAddress();//���ּҸ� �����ͼ� SourceMac����
		//Ư�� �������� Mac�� ���
		m_stSrcAdd.Format("%.2X-%.2X-%.2X-%.2X-%.2X-%.2X",
			m_SrcMacAddress[0], m_SrcMacAddress[1], m_SrcMacAddress[2],
			m_SrcMacAddress[3], m_SrcMacAddress[4], m_SrcMacAddress[5]);
		
		//�Է¹��� IP�ּҸ� ������ temp�� ����
		unsigned char temp[4];
		m_src.GetAddress(temp[0],temp[1],temp[2],temp[3]);
		
		//�� ���̾��� �ּ� �����κп� Mac, IP�� ����
		m_Ether->SetSourceAddress(m_SrcMacAddress);
		m_ARP->SetSourceAddress(m_SrcMacAddress);
		m_ARP->SetSourceIPAddress(temp);
		m_IP->SetSourceAddress(temp);

		//������ ����
		m_ReceiveThread = ::AfxBeginThread(CIPCAppDlg::ReceiveThread, this); //ARP�޾ƿ��� ������
		m_printThread = ::AfxBeginThread(CIPCAppDlg::PrintThread, this);	//��� ������
	}
	else {
		// �缳��
		m_bSendReady = FALSE;
		SetDlgState(IPC_ADDR_RESET);
	}
	UpdateData(FALSE);

}
//Cache Table�� �����ϴ� ������
UINT CIPCAppDlg::PrintThread(LPVOID pParam)
{
	CIPCAppDlg* obj = (CIPCAppDlg*) pParam;
	//�ּ� ������ �� ��� �����ϸ�, �缳���� ������ ����
	while (obj->m_bSendReady == TRUE)
	{
		//2�ʿ� �ѹ��� ���̺��� �����Ѵ�.
		obj->PrintCacheTable();
		Sleep(1000);
	}
	return 1;
}
//ARP �޽����� �޾ƿ��� ������
UINT CIPCAppDlg::ReceiveThread(LPVOID pParam)
{
	CIPCAppDlg* obj = (CIPCAppDlg*) pParam;
	while (obj->m_bSendReady == TRUE)
	{
		//IP���̾��� Receive�� ȣ���Ͽ� ��Ŷ�� �����´�.
		CBaseLayer* bLayer;
		bLayer = obj->m_LayerMgr.GetLayer( "IP" );
		bLayer->Receive();

	}
	return 1;
}
//Cstring���� 16������ �ٲپ��ִ� �Լ�
BOOL CIPCAppDlg::ConvertHex(CString cs, unsigned char* hex)
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
//���̾� ���¸� ��ȭ
void CIPCAppDlg::SetDlgState(int state)
{
	UpdateData(TRUE);

	CButton*	pSetAddrButton = (CButton*) GetDlgItem(IDC_BUTTON_ADDR);
	CButton*	pArpItemDeleteButton = (CButton*) GetDlgItem(IDC_delete);
	CButton*	pArpItemAllDeleteButton = (CButton*) GetDlgItem(IDC_all_delete);
	CButton*	pProxyItemDeleteButton = (CButton*) GetDlgItem(IDC_Proxy_Delete);
	CButton*	pProxyItemAddButton = (CButton*) GetDlgItem(IDC_Proxy_Add);
	CButton*	pIPSendButton = (CButton*) GetDlgItem(IDC_BUTTON4);
	CButton*	pGratuitousButton = (CButton*) GetDlgItem(IDC_Gratuitous_ARP);
	CEdit*		pSrcEdit = (CEdit*) GetDlgItem(IDC_EDIT_SRC);
	CEdit*		pGraEdit = (CEdit*) GetDlgItem(IDC_GRA_MAC);
	CIPAddressCtrl*		pARPSendIP = (CIPAddressCtrl*) GetDlgItem(IDC_IP_ADDRESS);
	CIPAddressCtrl*		pSrcIP = (CIPAddressCtrl*) GetDlgItem(IDC_IP_src_ADDRESS);
	CListBox*	pCacheTable = (CListBox*) GetDlgItem(IDC_LIST_CHAT);
	CListBox*	pProxyTable = (CListBox*) GetDlgItem(IDC_proxy_LIST);
	CComboBox*	pNIC = (CComboBox*) GetDlgItem(IDC_COMBO_NIC);

	switch (state)
	{
	case IPC_INITIALIZING:
		pSetAddrButton->EnableWindow(TRUE);
		pArpItemDeleteButton->EnableWindow(FALSE);
		pArpItemAllDeleteButton->EnableWindow(FALSE);
		pProxyItemDeleteButton->EnableWindow(FALSE);
		pProxyItemAddButton->EnableWindow(FALSE);
		pIPSendButton->EnableWindow(FALSE);
		pGratuitousButton->EnableWindow(FALSE);
		pSrcEdit->EnableWindow(FALSE);
		pGraEdit->EnableWindow(FALSE);
		pARPSendIP->EnableWindow(FALSE);
		pSrcIP->EnableWindow(TRUE);
		pCacheTable->EnableWindow(FALSE);
		pProxyTable->EnableWindow(FALSE);
		pNIC->EnableWindow(TRUE);
		break;
	case IPC_WAITFORACK:	break;
	case IPC_ERROR:		break;
	case IPC_ADDR_SET:
		pSetAddrButton->SetWindowText("�缳��(&R)");
		pArpItemDeleteButton->EnableWindow(TRUE);
		pArpItemAllDeleteButton->EnableWindow(TRUE);
		pProxyItemDeleteButton->EnableWindow(TRUE);
		pProxyItemAddButton->EnableWindow(TRUE);
		pIPSendButton->EnableWindow(TRUE);
		pGratuitousButton->EnableWindow(TRUE);
		pGraEdit->EnableWindow(TRUE);
		pARPSendIP->EnableWindow(TRUE);
		pSrcIP->EnableWindow(FALSE);
		pCacheTable->EnableWindow(TRUE);
		pProxyTable->EnableWindow(TRUE);
		pNIC->EnableWindow(FALSE);
		break;
	case IPC_ADDR_RESET:
		pSetAddrButton->SetWindowText("����(&O)");
		pArpItemDeleteButton->EnableWindow(FALSE);
		pArpItemAllDeleteButton->EnableWindow(FALSE);
		pProxyItemDeleteButton->EnableWindow(FALSE);
		pProxyItemAddButton->EnableWindow(FALSE);
		pIPSendButton->EnableWindow(FALSE);
		pGratuitousButton->EnableWindow(FALSE);
		pSrcEdit->EnableWindow(FALSE);
		pGraEdit->EnableWindow(FALSE);
		pARPSendIP->EnableWindow(FALSE);
		pSrcIP->EnableWindow(TRUE);
		pCacheTable->EnableWindow(FALSE);
		pProxyTable->EnableWindow(FALSE);
		pNIC->EnableWindow(TRUE);
		m_stSrcAdd = "";
		break;
	}

	UpdateData(FALSE);
}
//����� �Ҵ� ����
void CIPCAppDlg::EndofProcess()
{
	m_LayerMgr.DeAllocLayer();
}
//ARP������ ���� IP�� �Է��ϰ� send�� ���� ���
void CIPCAppDlg::OnBnClickedSend()
{
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
	BOOL checking=FALSE;
	int i;
	UpdateData( TRUE ) ;
	//�Էµ� IP�� �޾Ƽ� temp�� ����
	unsigned char temp[4];
	m_dst.GetAddress(temp[0],temp[1],temp[2],temp[3]);
	//������ source IP�� ������ temp2�� ����
	unsigned char* temp2;
	temp2=m_IP->GetSourceAddress();
	//���� ������ ARP�� ���� ��� i==4�� �ǰ� �ƹ� �ൿ ���� �ʰ� return
	for(i=0;i<4;i++){
		if(temp[i]!=temp2[i])
			break;
	}
	if(i==4)
		return;

	checking = m_ARP->Listcheck(temp);//ARP���̾��� üũ �Լ��� ȣ���Ͽ� Cache Table�� �̹� ��ϵǾ� �ִ��� Ȯ��
	if(checking == FALSE){//��ϵ��� ���� ��� send
		//IP���̾�� ARP���̾ �������� IP�ּ� ����
		m_IP->SetDestinAddress(temp);
		m_ARP->SetDesIPAddress(temp);
		check = 1;
		this->mp_UnderLayer->Send(1);//send ȣ��
		SetTimer( 1, 4000, NULL ) ;//Ÿ�� üũ �ִ� 4�� ��ٸ�
	}
	UpdateData( FALSE ) ;
	
}
void CIPCAppDlg::OnComboNIC() 
{
	UpdateData(TRUE);

	m_NI->setAdapterNum(m_NICSet.GetCurSel());
	m_NI->setMacAddress();//Mac�ּ� ����
	m_SrcMacAddress = m_NI->getMacAddress();//���ּҸ� �����ͼ� SourceMac����								
	m_stSrcAdd.Format("%.2X-%.2X-%.2X-%.2X-%.2X-%.2X", //Ư�� �������� Mac�� ���
	m_SrcMacAddress[0], m_SrcMacAddress[1], m_SrcMacAddress[2],
	m_SrcMacAddress[3], m_SrcMacAddress[4], m_SrcMacAddress[5]);

	UpdateData(FALSE);
}
//�ð� �ʰ��Ͽ��� ���
void CIPCAppDlg::OnTimer(UINT nIDEvent)
{
    if(nIDEvent == 1)    // ID 1�� Ÿ�̸�
    {
		m_ARP->FailAddress();	//�޾ƿ��� ���� ������, ���� ó��
		KillTimer(1);
    }
}
//ARP������ ���� ���
BOOL CIPCAppDlg::Receive(unsigned char* ppayload1, unsigned char* ppayload2)
{
	//Cache table�� ������ ������
	LPCACHE_LIST cache_list = m_ARP->GetCacheList()->next;
	m_list_arp.ResetContent();//â ����
	int count = 0;

	CString list;

	KillTimer(1);
	if(check==1){//ARP�޽����� ���� ���°� ������
		while(cache_list != NULL) //��ũ�� ����Ʈ�� ������ �˻�
		{
			if (cache_list->CACHE_List.cache_type == 0x00)
			{
				// ARP ��û�� ������ ���� ���
				list.Format("%3d.%3d.%3d.%3d    \?\?:\?\?:\?\?:\?\?:\?\?:\?\? incomplete %4d", 
				cache_list->CACHE_List.cache_pAddress[0], cache_list->CACHE_List.cache_pAddress[1], 
				cache_list->CACHE_List.cache_pAddress[2], cache_list->CACHE_List.cache_pAddress[3], cache_list->CACHE_List.cache_time);
				
			}
			else
			{
				// ARP ��û�� ������ ���� ���
				list.Format("%3d.%3d.%3d.%3d    %02x:%02x:%02x:%02x:%02x:%02x  complete %4d", 
					cache_list->CACHE_List.cache_pAddress[0], cache_list->CACHE_List.cache_pAddress[1], 
					cache_list->CACHE_List.cache_pAddress[2], cache_list->CACHE_List.cache_pAddress[3], 
					cache_list->CACHE_List.cache_enetaddr[0], cache_list->CACHE_List.cache_enetaddr[1], 
					cache_list->CACHE_List.cache_enetaddr[2], cache_list->CACHE_List.cache_enetaddr[3], 
					cache_list->CACHE_List.cache_enetaddr[4], cache_list->CACHE_List.cache_enetaddr[5], cache_list->CACHE_List.cache_time);
			}
			m_list_arp.AddString(list);//���
			cache_list = cache_list->next;//���� ����Ʈ
		}
	}
	
	check=0;
	return 0;
}
//Cache Table���� ���� ��ư�� ���� ���
void CIPCAppDlg::OnBnClickeddelete()
{
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
	CString temp_IPAddr;
	unsigned char IPAddr[4];
	//Ŀ���� ��ġ�� ���ڿ��� ������
	m_list_arp.GetText(m_list_arp.GetCurSel(), temp_IPAddr);
	unsigned char char_temp[4];
	int offset = temp_IPAddr.Find("    ");//����������
	temp_IPAddr = temp_IPAddr.Left(offset);//��������ġ ���� �ڸ�
	//.�� �������� IP�ּҸ� 4����Ѵ�.
	for(int i = 0; i < 4; i++)
	{
		CString temp;
		AfxExtractSubString(temp, temp_IPAddr, i, '.');
		sscanf(temp,"%d", &char_temp);
		IPAddr[i] = char_temp[0];
	}
	//�ش��ϴ� IP�� Cache Table���� ã�� ����
	m_ARP->DeleteCache( IPAddr );
	//���̺� ���
	PrintCacheTable();
}
//Cache Table�� ����ϴ� �Լ�
void CIPCAppDlg::PrintCacheTable()
{
	//����ϱ� ���� Cache Table�� ����Ʈ�� ������
	LPCACHE_LIST cache_list = m_ARP->GetCacheList()->next;
	m_list_arp.ResetContent();//â ����

	CString list;

	while(cache_list != NULL) 
	{
		if (cache_list->CACHE_List.cache_type == 0x00)//������ ���� ����Ʈ
		{
			list.Format("%3d.%3d.%3d.%3d    \?\?:\?\?:\?\?:\?\?:\?\?:\?\? incomplete %4d",
			cache_list->CACHE_List.cache_pAddress[0], cache_list->CACHE_List.cache_pAddress[1], 
			cache_list->CACHE_List.cache_pAddress[2], cache_list->CACHE_List.cache_pAddress[3], cache_list->CACHE_List.cache_time);
		}
		else//���� ���� ����Ʈ
		{
			list.Format("%3d.%3d.%3d.%3d    %02x:%02x:%02x:%02x:%02x:%02x  complete %4d",
			cache_list->CACHE_List.cache_pAddress[0], cache_list->CACHE_List.cache_pAddress[1], 
			cache_list->CACHE_List.cache_pAddress[2], cache_list->CACHE_List.cache_pAddress[3], 
			cache_list->CACHE_List.cache_enetaddr[0], cache_list->CACHE_List.cache_enetaddr[1], 
			cache_list->CACHE_List.cache_enetaddr[2], cache_list->CACHE_List.cache_enetaddr[3], 
			cache_list->CACHE_List.cache_enetaddr[4], cache_list->CACHE_List.cache_enetaddr[5], cache_list->CACHE_List.cache_time);
		}
		m_list_arp.AddString(list);//ȭ�鿡 ���
		cache_list = cache_list->next;//���� ����Ʈ�� ����Ŵ
	}
}
//Proxy Table�� ����ϴ� �Լ�
void CIPCAppDlg::PrintProxyTable()
{
	//Proxy Table�� ����Ʈ�� ������
	LPPROXY_LIST cache_list = m_ARP->GetProxyList()->next;
	m_list_proxy.ResetContent();//â ����

	CString list;

	while(cache_list != NULL) 
	{
		list.Format("%3d.%3d.%3d.%3d    %02x:%02x:%02x:%02x:%02x:%02x", 
		cache_list->CACHE_List.cache_pAddress[0], cache_list->CACHE_List.cache_pAddress[1], 
		cache_list->CACHE_List.cache_pAddress[2], cache_list->CACHE_List.cache_pAddress[3], 
		cache_list->CACHE_List.cache_enetaddr[0], cache_list->CACHE_List.cache_enetaddr[1], 
		cache_list->CACHE_List.cache_enetaddr[2], cache_list->CACHE_List.cache_enetaddr[3], 
		cache_list->CACHE_List.cache_enetaddr[4], cache_list->CACHE_List.cache_enetaddr[5]);
		m_list_proxy.AddString(list);//���
		cache_list = cache_list->next;//������ ����Ŵ
	}
}
//Cache Table���� ���� ������ ������ ȣ��Ǵ� �Լ�
void CIPCAppDlg::OnBnClickedalldelete()
{
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
	m_ARP->AllDeleteCache();	// ��� element ����
	PrintCacheTable();
}
//�����ư�� �������
void CIPCAppDlg::OnBnClickedOk()
{
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
	//�Ҵ�� ���̾ �����ϰ� ����
	EndofProcess();
}
//Gratuitous SendŬ���� ���
void CIPCAppDlg::OnBnClickedGratuitousArp()
{
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
	UpdateData( TRUE ) ;
	CString m_EtherAddr;
	if(Gratuitous_ARP.GetLength()!=12)
	{
		GratitousAlert g_alert;
		g_alert.DoModal();
		return;//mac�� �� �Է� ������ ��
	}
	else {
		Gratuitous_ARP.MakeLower();	// �ҹ��ڷ� ��ȯ
		ConvertHex(Gratuitous_ARP, m_GraMacAddress);//16������ ��ȯ
		unsigned char* temp_ip;
		temp_ip=m_IP->GetSourceAddress();//Source IP�� ������
		m_IP->SetDestinAddress(temp_ip);//IP���̾ Source ip ����
		m_ARP->SetSourceAddress(m_GraMacAddress);//ARP,Ethernet ���̾ Source Mac ����
		m_Ether->SetSourceAddress(m_GraMacAddress);
		m_stSrcAdd.Format("%.2X-%.2X-%.2X-%.2X-%.2X-%.2X",
			m_GraMacAddress[0], m_GraMacAddress[1], m_GraMacAddress[2],
			m_GraMacAddress[3], m_GraMacAddress[4], m_GraMacAddress[5]);
		m_IP->Send(1);//ARP���·� Send
	}
	UpdateData( FALSE ) ;
}
//Proxy add�� ���� ���
void CIPCAppDlg::OnBnClickedProxyAdd()
{
	// TODO: ���⿡ ��Ʈ�� �˸� ó�����ڵ带 �߰��մϴ�.
	ProxyTable dlg= new ProxyTable();//�� ���̾� ��ü ����

	if(dlg.DoModal()==IDOK)//Ȯ�� ��ư�� �������
	{
		//���Ͻ� â���� �Է��� MAC�� IP�� ������ ����
		memcpy(m_ProxyMacAddress,dlg.m_DstMacAddress,6);
		memcpy(m_ProxyAddress,dlg.temp,4);
		//ARP���̾ �ּ� ����
		m_ARP->SetProxEtherAddress(m_ProxyMacAddress);
		m_ARP->SetProxyIPAddress(m_ProxyAddress);
		m_ARP->proxyadd();//Proxy Table�� �ֱ� ���Ͽ� ����Ʈ�� �߰�
		PrintProxyTable();//Proxy Table���
	}
	free(dlg);//����
}
//Proxy ����
void CIPCAppDlg::OnBnClickedProxyDelete()
{
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
	CString temp_IPAddr;
	unsigned char IPAddr[4];
	//Ŀ���� ��ġ�� IP�� ������
	m_list_proxy.GetText(m_list_proxy.GetCurSel(), temp_IPAddr);
	unsigned char char_temp[4];
	int offset = temp_IPAddr.Find("    ");//�ش� �������� ã��
	temp_IPAddr = temp_IPAddr.Left(offset);//������ �ձ��� �ڸ�
	//.�� �������� IP�� ������ ����
	for(int i = 0; i < 4; i++)
	{
		CString temp;
		AfxExtractSubString(temp, temp_IPAddr, i, '.');
		sscanf(temp,"%d", &char_temp);
		IPAddr[i] = char_temp[0];
	}
	m_ARP->DeleteProxy( IPAddr );//����Ʈ���� ����
	PrintProxyTable();//Proxy Table�ٽ� ���
}