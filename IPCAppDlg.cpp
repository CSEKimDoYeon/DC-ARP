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
	//초기화
	m_stSrcAdd = _T("");
	check=0;
	//}}AFX_DATA_INIT
	// Note that LoadIcon does not require a subsequent DestroyIcon in Win32
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);

	//레이어 생성
	m_LayerMgr.AddLayer(new CEthernetLayer("Ethernet"));
	m_LayerMgr.AddLayer(new CIPLayer("IP"));
	m_LayerMgr.AddLayer(new CARPLayer("ARP"));
	m_LayerMgr.AddLayer(new CNILayer("NI"));	
	m_LayerMgr.AddLayer(this);

	//레이어들간 연결
	m_LayerMgr.ConnectLayers("NI ( *Ethernet ( *ARP ) )");
	m_LayerMgr.GetLayer( "IP" )->SetUnderLayer(m_LayerMgr.GetLayer( "ARP" ) );
	m_LayerMgr.GetLayer( "Ethernet" )->SetUpperLayer(m_LayerMgr.GetLayer( "IP" ) );
	m_LayerMgr.GetLayer( "IP" )->SetUpperLayer(m_LayerMgr.GetLayer( "ARPDlg" ) );
	m_LayerMgr.GetLayer( "ARPDlg" )->SetUnderLayer(m_LayerMgr.GetLayer( "IP" ) );

	//레이어를 가리키는 변수 지정
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
		if (MessageBox("종료하시겠습니까?",
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

//주소 세팅
void CIPCAppDlg::OnButtonAddrSet()
{
	UpdateData(TRUE);

	if (!m_bSendReady){
		//설정을 누른경우
		m_bSendReady = TRUE;
		SetDlgState(IPC_ADDR_SET);
		//선택된 NIC의 MAC주소를 NI레이어에서 가져옴
		m_NI->setAdapterNum(m_NICSet.GetCurSel());
		m_NI->setMacAddress();//Mac주소 설정
		m_SrcMacAddress = m_NI->getMacAddress();//맥주소를 가져와서 SourceMac지정
		//특정 형식으로 Mac을 출력
		m_stSrcAdd.Format("%.2X-%.2X-%.2X-%.2X-%.2X-%.2X",
			m_SrcMacAddress[0], m_SrcMacAddress[1], m_SrcMacAddress[2],
			m_SrcMacAddress[3], m_SrcMacAddress[4], m_SrcMacAddress[5]);
		
		//입력받은 IP주소를 가져와 temp에 저장
		unsigned char temp[4];
		m_src.GetAddress(temp[0],temp[1],temp[2],temp[3]);
		
		//각 레이어의 주소 설정부분에 Mac, IP를 설정
		m_Ether->SetSourceAddress(m_SrcMacAddress);
		m_ARP->SetSourceAddress(m_SrcMacAddress);
		m_ARP->SetSourceIPAddress(temp);
		m_IP->SetSourceAddress(temp);

		//쓰레드 시작
		m_ReceiveThread = ::AfxBeginThread(CIPCAppDlg::ReceiveThread, this); //ARP받아오는 쓰레드
		m_printThread = ::AfxBeginThread(CIPCAppDlg::PrintThread, this);	//출력 쓰레드
	}
	else {
		// 재설정
		m_bSendReady = FALSE;
		SetDlgState(IPC_ADDR_RESET);
	}
	UpdateData(FALSE);

}
//Cache Table을 갱신하는 쓰레드
UINT CIPCAppDlg::PrintThread(LPVOID pParam)
{
	CIPCAppDlg* obj = (CIPCAppDlg*) pParam;
	//주소 설정을 한 경우 동작하며, 재설정을 누르면 종료
	while (obj->m_bSendReady == TRUE)
	{
		//2초에 한번씩 테이블을 갱신한다.
		obj->PrintCacheTable();
		Sleep(1000);
	}
	return 1;
}
//ARP 메시지를 받아오는 쓰레드
UINT CIPCAppDlg::ReceiveThread(LPVOID pParam)
{
	CIPCAppDlg* obj = (CIPCAppDlg*) pParam;
	while (obj->m_bSendReady == TRUE)
	{
		//IP레이어의 Receive를 호출하여 패킷을 가져온다.
		CBaseLayer* bLayer;
		bLayer = obj->m_LayerMgr.GetLayer( "IP" );
		bLayer->Receive();

	}
	return 1;
}
//Cstring형을 16진수로 바꾸어주는 함수
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
//다이얼 상태를 변화
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
		pSetAddrButton->SetWindowText("재설정(&R)");
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
		pSetAddrButton->SetWindowText("설정(&O)");
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
//종료시 할당 해제
void CIPCAppDlg::EndofProcess()
{
	m_LayerMgr.DeAllocLayer();
}
//ARP전송을 위해 IP를 입력하고 send를 누른 경우
void CIPCAppDlg::OnBnClickedSend()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	BOOL checking=FALSE;
	int i;
	UpdateData( TRUE ) ;
	//입력된 IP를 받아서 temp에 저장
	unsigned char temp[4];
	m_dst.GetAddress(temp[0],temp[1],temp[2],temp[3]);
	//현재의 source IP를 가져와 temp2에 저장
	unsigned char* temp2;
	temp2=m_IP->GetSourceAddress();
	//내가 나한테 ARP를 보낸 경우 i==4가 되고 아무 행동 하지 않고 return
	for(i=0;i<4;i++){
		if(temp[i]!=temp2[i])
			break;
	}
	if(i==4)
		return;

	checking = m_ARP->Listcheck(temp);//ARP레이어의 체크 함수를 호출하여 Cache Table에 이미 등록되어 있는지 확인
	if(checking == FALSE){//등록되지 않은 경우 send
		//IP레이어와 ARP레이어에 목적지의 IP주소 세팅
		m_IP->SetDestinAddress(temp);
		m_ARP->SetDesIPAddress(temp);
		check = 1;
		this->mp_UnderLayer->Send(1);//send 호출
		SetTimer( 1, 4000, NULL ) ;//타입 체크 최대 4초 기다림
	}
	UpdateData( FALSE ) ;
	
}
void CIPCAppDlg::OnComboNIC() 
{
	UpdateData(TRUE);

	m_NI->setAdapterNum(m_NICSet.GetCurSel());
	m_NI->setMacAddress();//Mac주소 설정
	m_SrcMacAddress = m_NI->getMacAddress();//맥주소를 가져와서 SourceMac지정								
	m_stSrcAdd.Format("%.2X-%.2X-%.2X-%.2X-%.2X-%.2X", //특정 형식으로 Mac을 출력
	m_SrcMacAddress[0], m_SrcMacAddress[1], m_SrcMacAddress[2],
	m_SrcMacAddress[3], m_SrcMacAddress[4], m_SrcMacAddress[5]);

	UpdateData(FALSE);
}
//시간 초과하였을 경우
void CIPCAppDlg::OnTimer(UINT nIDEvent)
{
    if(nIDEvent == 1)    // ID 1번 타이머
    {
		m_ARP->FailAddress();	//받아오는 것을 실패함, 실패 처리
		KillTimer(1);
    }
}
//ARP응답을 받은 경우
BOOL CIPCAppDlg::Receive(unsigned char* ppayload1, unsigned char* ppayload2)
{
	//Cache table의 값들을 가져옴
	LPCACHE_LIST cache_list = m_ARP->GetCacheList()->next;
	m_list_arp.ResetContent();//창 리셋
	int count = 0;

	CString list;

	KillTimer(1);
	if(check==1){//ARP메시지를 보낸 상태가 맞으면
		while(cache_list != NULL) //링크드 리스트의 끝까지 검색
		{
			if (cache_list->CACHE_List.cache_type == 0x00)
			{
				// ARP 요청시 응답이 없을 경우
				list.Format("%3d.%3d.%3d.%3d    \?\?:\?\?:\?\?:\?\?:\?\?:\?\? incomplete %4d", 
				cache_list->CACHE_List.cache_pAddress[0], cache_list->CACHE_List.cache_pAddress[1], 
				cache_list->CACHE_List.cache_pAddress[2], cache_list->CACHE_List.cache_pAddress[3], cache_list->CACHE_List.cache_time);
				
			}
			else
			{
				// ARP 요청시 응답이 왔을 경우
				list.Format("%3d.%3d.%3d.%3d    %02x:%02x:%02x:%02x:%02x:%02x  complete %4d", 
					cache_list->CACHE_List.cache_pAddress[0], cache_list->CACHE_List.cache_pAddress[1], 
					cache_list->CACHE_List.cache_pAddress[2], cache_list->CACHE_List.cache_pAddress[3], 
					cache_list->CACHE_List.cache_enetaddr[0], cache_list->CACHE_List.cache_enetaddr[1], 
					cache_list->CACHE_List.cache_enetaddr[2], cache_list->CACHE_List.cache_enetaddr[3], 
					cache_list->CACHE_List.cache_enetaddr[4], cache_list->CACHE_List.cache_enetaddr[5], cache_list->CACHE_List.cache_time);
			}
			m_list_arp.AddString(list);//출력
			cache_list = cache_list->next;//다음 리스트
		}
	}
	
	check=0;
	return 0;
}
//Cache Table에서 삭제 버튼을 누른 경우
void CIPCAppDlg::OnBnClickeddelete()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	CString temp_IPAddr;
	unsigned char IPAddr[4];
	//커서가 위치한 문자열을 가져옴
	m_list_arp.GetText(m_list_arp.GetCurSel(), temp_IPAddr);
	unsigned char char_temp[4];
	int offset = temp_IPAddr.Find("    ");//오프셋지정
	temp_IPAddr = temp_IPAddr.Left(offset);//오프셋위치 까지 자름
	//.을 기준으로 IP주소를 4등분한다.
	for(int i = 0; i < 4; i++)
	{
		CString temp;
		AfxExtractSubString(temp, temp_IPAddr, i, '.');
		sscanf(temp,"%d", &char_temp);
		IPAddr[i] = char_temp[0];
	}
	//해당하는 IP를 Cache Table에서 찾아 삭제
	m_ARP->DeleteCache( IPAddr );
	//테이블 출력
	PrintCacheTable();
}
//Cache Table을 출력하는 함수
void CIPCAppDlg::PrintCacheTable()
{
	//출력하기 위해 Cache Table의 리스트를 가져옴
	LPCACHE_LIST cache_list = m_ARP->GetCacheList()->next;
	m_list_arp.ResetContent();//창 리셋

	CString list;

	while(cache_list != NULL) 
	{
		if (cache_list->CACHE_List.cache_type == 0x00)//응답이 없는 리스트
		{
			list.Format("%3d.%3d.%3d.%3d    \?\?:\?\?:\?\?:\?\?:\?\?:\?\? incomplete %4d",
			cache_list->CACHE_List.cache_pAddress[0], cache_list->CACHE_List.cache_pAddress[1], 
			cache_list->CACHE_List.cache_pAddress[2], cache_list->CACHE_List.cache_pAddress[3], cache_list->CACHE_List.cache_time);
		}
		else//응답 받은 리스트
		{
			list.Format("%3d.%3d.%3d.%3d    %02x:%02x:%02x:%02x:%02x:%02x  complete %4d",
			cache_list->CACHE_List.cache_pAddress[0], cache_list->CACHE_List.cache_pAddress[1], 
			cache_list->CACHE_List.cache_pAddress[2], cache_list->CACHE_List.cache_pAddress[3], 
			cache_list->CACHE_List.cache_enetaddr[0], cache_list->CACHE_List.cache_enetaddr[1], 
			cache_list->CACHE_List.cache_enetaddr[2], cache_list->CACHE_List.cache_enetaddr[3], 
			cache_list->CACHE_List.cache_enetaddr[4], cache_list->CACHE_List.cache_enetaddr[5], cache_list->CACHE_List.cache_time);
		}
		m_list_arp.AddString(list);//화면에 출력
		cache_list = cache_list->next;//다음 리스트를 가르킴
	}
}
//Proxy Table을 출력하는 함수
void CIPCAppDlg::PrintProxyTable()
{
	//Proxy Table의 리스트를 가져옴
	LPPROXY_LIST cache_list = m_ARP->GetProxyList()->next;
	m_list_proxy.ResetContent();//창 리셋

	CString list;

	while(cache_list != NULL) 
	{
		list.Format("%3d.%3d.%3d.%3d    %02x:%02x:%02x:%02x:%02x:%02x", 
		cache_list->CACHE_List.cache_pAddress[0], cache_list->CACHE_List.cache_pAddress[1], 
		cache_list->CACHE_List.cache_pAddress[2], cache_list->CACHE_List.cache_pAddress[3], 
		cache_list->CACHE_List.cache_enetaddr[0], cache_list->CACHE_List.cache_enetaddr[1], 
		cache_list->CACHE_List.cache_enetaddr[2], cache_list->CACHE_List.cache_enetaddr[3], 
		cache_list->CACHE_List.cache_enetaddr[4], cache_list->CACHE_List.cache_enetaddr[5]);
		m_list_proxy.AddString(list);//출력
		cache_list = cache_list->next;//다음을 가리킴
	}
}
//Cache Table에서 전부 삭제를 누르면 호출되는 함수
void CIPCAppDlg::OnBnClickedalldelete()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	m_ARP->AllDeleteCache();	// 모든 element 제거
	PrintCacheTable();
}
//종료버튼을 누른경우
void CIPCAppDlg::OnBnClickedOk()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	//할당된 레이어를 해제하고 종료
	EndofProcess();
}
//Gratuitous Send클릭한 경우
void CIPCAppDlg::OnBnClickedGratuitousArp()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	UpdateData( TRUE ) ;
	CString m_EtherAddr;
	if(Gratuitous_ARP.GetLength()!=12)
	{
		GratitousAlert g_alert;
		g_alert.DoModal();
		return;//mac을 다 입력 안했을 때
	}
	else {
		Gratuitous_ARP.MakeLower();	// 소문자로 변환
		ConvertHex(Gratuitous_ARP, m_GraMacAddress);//16진수로 변환
		unsigned char* temp_ip;
		temp_ip=m_IP->GetSourceAddress();//Source IP를 가져옴
		m_IP->SetDestinAddress(temp_ip);//IP레이어에 Source ip 세팅
		m_ARP->SetSourceAddress(m_GraMacAddress);//ARP,Ethernet 레이어에 Source Mac 세팅
		m_Ether->SetSourceAddress(m_GraMacAddress);
		m_stSrcAdd.Format("%.2X-%.2X-%.2X-%.2X-%.2X-%.2X",
			m_GraMacAddress[0], m_GraMacAddress[1], m_GraMacAddress[2],
			m_GraMacAddress[3], m_GraMacAddress[4], m_GraMacAddress[5]);
		m_IP->Send(1);//ARP형태로 Send
	}
	UpdateData( FALSE ) ;
}
//Proxy add를 누른 경우
void CIPCAppDlg::OnBnClickedProxyAdd()
{
	// TODO: 여기에 컨트롤 알림 처리기코드를 추가합니다.
	ProxyTable dlg= new ProxyTable();//새 다이얼 객체 생성

	if(dlg.DoModal()==IDOK)//확인 버튼을 누른경우
	{
		//프록시 창에서 입력한 MAC과 IP를 가져와 저장
		memcpy(m_ProxyMacAddress,dlg.m_DstMacAddress,6);
		memcpy(m_ProxyAddress,dlg.temp,4);
		//ARP레이어에 주소 세팅
		m_ARP->SetProxEtherAddress(m_ProxyMacAddress);
		m_ARP->SetProxyIPAddress(m_ProxyAddress);
		m_ARP->proxyadd();//Proxy Table에 넣기 위하여 리스트에 추가
		PrintProxyTable();//Proxy Table출력
	}
	free(dlg);//해제
}
//Proxy 삭제
void CIPCAppDlg::OnBnClickedProxyDelete()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	CString temp_IPAddr;
	unsigned char IPAddr[4];
	//커서가 위치한 IP를 가져옴
	m_list_proxy.GetText(m_list_proxy.GetCurSel(), temp_IPAddr);
	unsigned char char_temp[4];
	int offset = temp_IPAddr.Find("    ");//해당 오프셋을 찾음
	temp_IPAddr = temp_IPAddr.Left(offset);//오프셋 앞까지 자름
	//.을 기준으로 IP를 나눠서 저장
	for(int i = 0; i < 4; i++)
	{
		CString temp;
		AfxExtractSubString(temp, temp_IPAddr, i, '.');
		sscanf(temp,"%d", &char_temp);
		IPAddr[i] = char_temp[0];
	}
	m_ARP->DeleteProxy( IPAddr );//리스트에서 삭제
	PrintProxyTable();//Proxy Table다시 출력
}