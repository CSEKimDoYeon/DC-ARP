// ArpLayer.cpp: implementation of the CArpLayer class.
//
/////////////////////////////////////////////////////////////////////

#include "StdAfx.h"
#include "ipc.h"
#include "ARPLayer.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif


//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CARPLayer::CARPLayer( char* pName ) : CBaseLayer( pName )
{		//링크드 리스트 헤더 할당, 초기화를 해준다.

	m_arp.arp_hdtype=0x00;							//hdtype 초기화 
	m_arp.arp_prototype=0x00;						//prototype 초기화 
	m_arp.arp_hdlength=0;							//hdlength 초기화 
	m_arp.arp_protolength=0;						//protolength 초기화 
	m_arp.arp_op=0;									//op  초기화 

	
	cache_header = ( LPCACHE_LIST )malloc( sizeof( CACHE_LIST ) );		//CACHE_LIST 사이즈만큼 크기를 할당해준다.
	cache_header->next = NULL;

	proxy_header = ( LPPROXY_LIST )malloc( sizeof( PROXY_LIST ) );		//PROXY_LIST 사이즈만큼 크기를 할당해준다.
	proxy_header->next = NULL;
}

CARPLayer::~CARPLayer()
{
}

//MAC주소를 return해준다. 
unsigned char* CARPLayer::GetSourceAddress()
{
	return m_arp.arp_srchdaddr;
}

//목적지 MAC주소를 return해준다.
unsigned char* CARPLayer::GetDestinAddress()
{
	return m_arp.arp_dsthdaddr;
}

//IP주소를 return해준다.
unsigned char* CARPLayer::GetSourceIPAddress()
{
	return m_arp.arp_srcprotoaddr;
}

//목적지 IP Address를 return해준다
unsigned char* CARPLayer::GetDesIPAddress()
{
	return m_arp.arp_dstprotoaddr;
}

//Proxy IP setting 작업 
void CARPLayer::SetProxyIPAddress(unsigned char *pAddress)
{
	memcpy(pi, pAddress, 4);
}

//Proxy Mac(이더넷) setting 작업
void CARPLayer::SetProxEtherAddress(unsigned char *pAddress)
{
	memcpy(pe, pAddress, 6);
}
//Source Mac setting 작업
void CARPLayer::SetSourceAddress(unsigned char *pAddress)
{
	memcpy(m_arp.arp_srchdaddr, pAddress, 6);
}
//목적지 Mac setting 작업 
void CARPLayer::SetDestinAddress(unsigned char *pAddress)
{
	memcpy(m_arp.arp_dsthdaddr, pAddress, 6);
}
//source IP setting 작업
void CARPLayer::SetSourceIPAddress(unsigned char *pAddress)
{
	memcpy(m_arp.arp_srcprotoaddr, pAddress, 4);
}
//목적지 IP set
void CARPLayer::SetDesIPAddress(unsigned char *pAddress)
{
	memcpy(m_arp.arp_dstprotoaddr, pAddress, 4);
}

//Cache List 반환
LPCACHE_LIST CARPLayer::GetCacheList() 
{
	timeminus();  //호출될 때마다 시간을 감소해준다.
	return cache_header;
}

//Proxy List반환
LPPROXY_LIST CARPLayer::GetProxyList() 
{
	
	return proxy_header; //프록시 헤더부분을 반환해준다. 
}

//호출 될 때마다 시간을 1초씩 감소시켜준다.
void CARPLayer::timeminus()
{
	LPCACHE_LIST prev, current;

	prev = cache_header;
	current = prev->next;	  
	

	while(current != NULL)   //current가 NULL 값이 아니라면  
	{														
		current->CACHE_List.cache_time = current->CACHE_List.cache_time-1;	// 해당 노드(ARP Cache)의 시간을 1초 감소시켜준다.
		
		if(current->CACHE_List.cache_time <= 0)				//시간이 0보다 같거나 작으면 time out이므로 리스트에서 삭제시켜준다.
		{
			prev->next = current->next;
			free(current);
			current = prev;
			continue;		
		}
		prev = current;
		current = current->next;
	}
}

// 송신
BOOL CARPLayer::Send(unsigned char *ppayload, int nlength,int op)
{
	PIP_HEADER ip_temp;

	BOOL bSuccess = FALSE;
	
	if(op == ARP_REQ){				//ARP Request
		ip_temp = (PIP_HEADER) ppayload;
		m_arp.arp_hdtype = 0x0001;
		m_arp.arp_prototype = 0x0800;
		m_arp.arp_hdlength = 6;
		m_arp.arp_protolength = 4;
		m_arp.arp_op = htons(ARP_REQ);			// op = 0x0001
		memcpy(m_arp.arp_dstprotoaddr , ip_temp->ip_dst.S_un.S_ip_addr,4);
		memcpy(m_arp.arp_srcprotoaddr, ip_temp->ip_src.S_un.S_ip_addr,4);
		bSuccess = mp_UnderLayer->Send( (unsigned char*)&m_arp, ARP_SIZE,1);
	}
	else if(op == ARP_REP)				//ARP Reply
	{
		ip_temp = (PIP_HEADER) ppayload;
		m_arp.arp_hdtype = 0x0001;
		m_arp.arp_prototype = 0x0800;
		m_arp.arp_hdlength = 6;
		m_arp.arp_protolength = 4;
		m_arp.arp_op = htons(ARP_REP);			// op = 0x0002
		memcpy(m_arp.arp_dstprotoaddr , ip_temp->ip_dst.S_un.S_ip_addr,4);
		memcpy(m_arp.arp_srcprotoaddr, swap_ip,4);
		bSuccess = mp_UnderLayer->Send( (unsigned char*)&m_arp, ARP_SIZE,2);
	}	
	return bSuccess ;
}


// 수신
unsigned char* CARPLayer::Receive()
{
	unsigned char* ppayload = mp_UnderLayer->Receive();	//Ethernet으로부터 패킷 receive

	if (ppayload != NULL){
		LPARP pFrame = (LPARP) ppayload;	//상위 레이어에 전달
		
		unsigned char	id[4];			//수신자 ip
		unsigned char	is[4];			//현재 내 ip
		unsigned char	ss[4];			//송신자 ip
		memset(id, 0, 4);
		memset(is, 0, 4);
		memset(ss, 0, 4);

		unsigned char temp[4];
		memset(temp, 0, 4);

		unsigned char F = 255;
		memcpy(id, pFrame->arp_dstprotoaddr, 4);
		memcpy(is, m_arp.arp_srcprotoaddr, 4);
		memcpy(ss, pFrame->arp_srcprotoaddr, 4);
		int i, check = 0;				// i 와 check 변수를 초기화해주고 for문을 통해 검사해줌

		
		for (i = 0; i < 4; i++)			//보낸 패킷이 나에게 맞는 경우에는
		{
			if ((id[i] != is[i])){
				check = 1;
				break;
			}
		}
		if (!check){
			check=0;
			for (i = 0; i < 4; i++)			 //보낸 것을 내 자신이 받은 경우에는 넘긴다.
			{
				 
				if ((is[i] != ss[i])){		//내가 보낸걸 내가 받음
					check=1;
					break;
				}
			}


			//ip를 비교해서 다른 것이 하나라도 있으면 응답을 보내야한다.

			if(check==1){
			
				if(ntohs(pFrame->arp_op) == ARP_REQ)
				{
					memcpy(swap_ip, pFrame->arp_dstprotoaddr ,4);
					memcpy(m_arp.arp_dsthdaddr, pFrame->arp_srchdaddr, 6);
					memcpy(m_arp.arp_dstprotoaddr, pFrame->arp_srcprotoaddr, 4);

					//cache_element 변수에 CACHE_LIST사이즈 크기만큼 할당해준다.
					LPCACHE_LIST cache_element = (LPCACHE_LIST)malloc(sizeof(CACHE_LIST));
					LPCACHE_LIST pre = cache_header;

					while (pre != NULL)		//pre가 NULL이 아니라면 
					{
						
						if (pre->CACHE_List.cache_pAddress[0] == m_arp.arp_dstprotoaddr[0] &&
							pre->CACHE_List.cache_pAddress[1] == m_arp.arp_dstprotoaddr[1] &&
							pre->CACHE_List.cache_pAddress[2] == m_arp.arp_dstprotoaddr[2] &&
							pre->CACHE_List.cache_pAddress[3] == m_arp.arp_dstprotoaddr[3])
							return m_arp.arp_dstprotoaddr;		
						pre = pre->next;
					}


					memcpy(cache_element->CACHE_List.cache_enetaddr, m_arp.arp_dsthdaddr, 6);
					memcpy(cache_element->CACHE_List.cache_pAddress, m_arp.arp_dstprotoaddr, 4);
					cache_element->CACHE_List.cache_time = 1200;	//20분
					cache_element->CACHE_List.cache_type = 1;		//응답받은 리스트인경우


					//생성한 리스트를 뒤에 추가
					pre = cache_header;

					while (pre->next != NULL)
					{
						pre = pre->next;
					}
					pre->next = cache_element;
					cache_element->next = NULL;

					return m_arp.arp_dstprotoaddr;
				}
				//응답을 받은 것이므로 테이블에 추가해줘야함.

				else if(ntohs(pFrame->arp_op) == ARP_REP)   //ARP응답 0x0002
				{
					SetDestinAddress(pFrame->arp_srchdaddr);
					SetDesIPAddress(pFrame->arp_srcprotoaddr);
					temp[0]=1;

					LPCACHE_LIST cache_element = (LPCACHE_LIST)malloc(sizeof(CACHE_LIST));
					LPCACHE_LIST pre = cache_header;

					while (pre->next != NULL)
					{
						
						if (pre->CACHE_List.cache_pAddress[0] == m_arp.arp_dstprotoaddr[0] &&
							pre->CACHE_List.cache_pAddress[1] == m_arp.arp_dstprotoaddr[1] && 
							pre->CACHE_List.cache_pAddress[2] == m_arp.arp_dstprotoaddr[2] && 
							pre->CACHE_List.cache_pAddress[3] == m_arp.arp_dstprotoaddr[3] )
							return temp;					
						pre = pre->next;
					}

					memcpy(cache_element->CACHE_List.cache_enetaddr, m_arp.arp_dsthdaddr, 6);
					memcpy(cache_element->CACHE_List.cache_pAddress, m_arp.arp_dstprotoaddr, 4);		
					cache_element->CACHE_List.cache_time = 1200;//20분
					cache_element->CACHE_List.cache_type = 1;//응답받은 리스트인경우

					//생성한 리스트를 뒤에 추가
					while(pre->next!=NULL)
					{
						pre=pre->next;
					}
					pre->next=cache_element;//테이블 뒤에 추가시켜주는 부분
					cache_element->next=NULL;
					return temp;
				}
			}
		}
			check = 0;


			//내가 받을 것은 아닌데 나한테 왔다온
			//gratitous확인 for문

			for (i = 0; i < 4; i++) 
			{
				if ((id[i] != ss[i])){
					check = 1;
					break;
				}
			}

			if(check==0)		//gratitous ARP 온 거
			{
				
				BOOL check;
				check=Listcheck(ss);	//리스트에 존재하는지 검사

				if(check==FALSE)		//check가 false라면 테이블에 없는 상태
				{
					return 0;			//그냥 리턴해주는 거	
				}
				else{					//check true...		테이블에 있는 상태
					SetDestinAddress(pFrame->arp_srchdaddr);
					Listchange(ss);		//맥주소 변경
					return 0;
				}
			}

			//target이 자신이 아닌경우, proxy table에 있는지 검사해주는 if문

			if(proxycheck(pFrame->arp_dstprotoaddr))	//Proxy table에 존재하면
			{
				memcpy(m_arp.arp_dsthdaddr, pFrame->arp_srchdaddr, 6);
				memcpy(m_arp.arp_dstprotoaddr, pFrame->arp_srcprotoaddr, 4);
				memcpy(swap_ip, pFrame->arp_dstprotoaddr, 4);
				return pFrame->arp_srcprotoaddr;
			}
	}
	return 0;
}


//proxy table에 있는지 검사 proxycheck  
BOOL CARPLayer::proxycheck(unsigned char* pAddress)
{
	LPPROXY_LIST seekList = proxy_header->next;  

	while(seekList!=NULL) //seekList가 NULL일 때까지 while문..
	{
		int check=1; 

		for(int i=0;i<4;i++)
		{
			if(seekList->CACHE_List.cache_pAddress[i]!=pAddress[i])
			{
				check=0;
				break;
			}
		}
		if(check==1)
			return TRUE;
		seekList=seekList->next;
	}

	return FALSE;
}

//proxy table에 추가

BOOL CARPLayer::proxyadd()
{
	//PROXY_LIST의 사이즈만큼 크기를 할당해준다.

	LPPROXY_LIST proxy_element = (LPPROXY_LIST)malloc(sizeof(PROXY_LIST));	

	memcpy(proxy_element->CACHE_List.cache_enetaddr, pe, 6);
	memcpy(proxy_element->CACHE_List.cache_pAddress, pi, 4);		
	proxy_element->next = proxy_header->next;
	proxy_header->next = proxy_element;	
	LPPROXY_LIST pre = proxy_header;

	while(pre->next!=NULL) 
	{
		pre=pre->next;
	}
	pre->next=proxy_element;
	proxy_element->next=NULL;

	return TRUE;
}


//gratuitous인경우 mac을 바꿔준다

BOOL CARPLayer::Listchange(unsigned char* pAddress)		//mac을 바꿔주는 Listchange
{
	LPCACHE_LIST seekList = cache_header->next;

	while(seekList!=NULL)
	{
		int check=1;

		//ip가 같은곳을 검사해줍니다.
		for(int i=0;i<4;i++)
		{
			if(seekList->CACHE_List.cache_pAddress[i]!=pAddress[i])
			{
				check=0;
				break;
			}
		}
		if(check==1) //check 타입을 봐서 같은 곳이 있다면 mac을 바꿔주고 타입을 바꿔준다.
		{
			memcpy(seekList->CACHE_List.cache_enetaddr,m_arp.arp_dsthdaddr,6);
			seekList->CACHE_List.cache_type=1;
			return TRUE;
		}
		seekList=seekList->next;
	}

	return FALSE;
}

//proxy 삭제
BOOL CARPLayer::DeleteProxy(unsigned char* pAddress)
{
	LPPROXY_LIST pre, current;

	pre = proxy_header;
	current = pre->next;
	
	while(current != NULL)
	{
		int check=1;
		for(int i=0;i<4;i++)
		{
			if(current->CACHE_List.cache_pAddress[i]!=pAddress[i])
			{
				check=0;
				break;
			}
		}
		if(check==1)	//proxy Table을 검사해서 현재와 같은 것이 있다면 check가 1인 상태이므로 그 해당 부분을 지워준다.
		{
			pre->next = current->next;
			free(current);
			break;
		}
		pre = current;
		current = current->next;
	}
	if(current == NULL)			//current가 NULL이라면 FALSE를 리턴해주고
		return FALSE;

	return TRUE;				//NULL아니면 TRUE를 리턴해준다.  
}

//캐시 테이블 삭제
BOOL CARPLayer::DeleteCache(unsigned char* pAddress)
{
	LPCACHE_LIST pre, current;

	pre = cache_header;
	current = pre->next;
	
	while(current != NULL)
	{
		int check=1;

		//삭제할 위치를 IP로 검색을 한다.
		for(int i=0;i<4;i++)
		{
			if(current->CACHE_List.cache_pAddress[i]!=pAddress[i])
			{
				check=0;
				break;
			}
		}
		if(check==1)
		{
			pre->next = current->next;
			free(current);
			break;
		}
		pre = current;
		current = current->next;
	}

	if(current == NULL)
		return FALSE;

	return TRUE;
}

//전부 삭제 해당 함수

void CARPLayer::AllDeleteCache() 
{
	LPCACHE_LIST temp_list, temp_element;
	temp_list = cache_header->next;
	cache_header->next = NULL;

	while(temp_list != NULL) //list가 NULL이 아닌경우에는 리스트를 돌면서 모든 entry를 제거해준다.
	{
		temp_element = temp_list;
		temp_list = temp_list->next;
		free(temp_element);
	}	
}


//테이블에 있는지 검사

BOOL CARPLayer::Listcheck(unsigned char* pAddress)
{
	LPCACHE_LIST seekList = cache_header->next;

	while(seekList!=NULL)
	{
		int check=1;
		for(int i=0;i<4;i++)
		{
			if(seekList->CACHE_List.cache_pAddress[i]!=pAddress[i])
			{
				check=0;
				break;
			}
		}
		//테이블에 있고 MAC도 있는 경우 리턴 true
		if(check==1&&seekList->CACHE_List.cache_type==1)
			return TRUE;

		//테이블에는 있지만 MAC을 모르는 경우, 테이블에서 삭제하고 ARP전송
		else if(check==1&&seekList->CACHE_List.cache_type==0)
		{
			DeleteCache(seekList->CACHE_List.cache_pAddress);
			return FALSE;
		}
		seekList=seekList->next;
	}

	return FALSE;
}


//응답이 업는 경우에서 추가를 해준다.

BOOL CARPLayer::FailAddress()
{
	LPCACHE_LIST cache_element = (LPCACHE_LIST)malloc(sizeof(CACHE_LIST));
	memcpy(cache_element->CACHE_List.cache_pAddress, m_arp.arp_dstprotoaddr, 4);		
	cache_element->CACHE_List.cache_time = 180;			//응답이 없는 경우 (incomplete..) 180초로 설정해준다.
	cache_element->CACHE_List.cache_type = 0;				

	LPCACHE_LIST pre = cache_header;
	while(pre->next!=NULL)
	{
		pre=pre->next;
	}
	pre->next=cache_element;
	cache_element->next=NULL;
						
	return TRUE;
}