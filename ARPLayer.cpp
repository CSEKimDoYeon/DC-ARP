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
{		//��ũ�� ����Ʈ ��� �Ҵ�, �ʱ�ȭ�� ���ش�.

	m_arp.arp_hdtype=0x00;							//hdtype �ʱ�ȭ 
	m_arp.arp_prototype=0x00;						//prototype �ʱ�ȭ 
	m_arp.arp_hdlength=0;							//hdlength �ʱ�ȭ 
	m_arp.arp_protolength=0;						//protolength �ʱ�ȭ 
	m_arp.arp_op=0;									//op  �ʱ�ȭ 

	
	cache_header = ( LPCACHE_LIST )malloc( sizeof( CACHE_LIST ) );		//CACHE_LIST �����ŭ ũ�⸦ �Ҵ����ش�.
	cache_header->next = NULL;

	proxy_header = ( LPPROXY_LIST )malloc( sizeof( PROXY_LIST ) );		//PROXY_LIST �����ŭ ũ�⸦ �Ҵ����ش�.
	proxy_header->next = NULL;
}

CARPLayer::~CARPLayer()
{
}

//MAC�ּҸ� return���ش�. 
unsigned char* CARPLayer::GetSourceAddress()
{
	return m_arp.arp_srchdaddr;
}

//������ MAC�ּҸ� return���ش�.
unsigned char* CARPLayer::GetDestinAddress()
{
	return m_arp.arp_dsthdaddr;
}

//IP�ּҸ� return���ش�.
unsigned char* CARPLayer::GetSourceIPAddress()
{
	return m_arp.arp_srcprotoaddr;
}

//������ IP Address�� return���ش�
unsigned char* CARPLayer::GetDesIPAddress()
{
	return m_arp.arp_dstprotoaddr;
}

//Proxy IP setting �۾� 
void CARPLayer::SetProxyIPAddress(unsigned char *pAddress)
{
	memcpy(pi, pAddress, 4);
}

//Proxy Mac(�̴���) setting �۾�
void CARPLayer::SetProxEtherAddress(unsigned char *pAddress)
{
	memcpy(pe, pAddress, 6);
}
//Source Mac setting �۾�
void CARPLayer::SetSourceAddress(unsigned char *pAddress)
{
	memcpy(m_arp.arp_srchdaddr, pAddress, 6);
}
//������ Mac setting �۾� 
void CARPLayer::SetDestinAddress(unsigned char *pAddress)
{
	memcpy(m_arp.arp_dsthdaddr, pAddress, 6);
}
//source IP setting �۾�
void CARPLayer::SetSourceIPAddress(unsigned char *pAddress)
{
	memcpy(m_arp.arp_srcprotoaddr, pAddress, 4);
}
//������ IP set
void CARPLayer::SetDesIPAddress(unsigned char *pAddress)
{
	memcpy(m_arp.arp_dstprotoaddr, pAddress, 4);
}

//Cache List ��ȯ
LPCACHE_LIST CARPLayer::GetCacheList() 
{
	timeminus();  //ȣ��� ������ �ð��� �������ش�.
	return cache_header;
}

//Proxy List��ȯ
LPPROXY_LIST CARPLayer::GetProxyList() 
{
	
	return proxy_header; //���Ͻ� ����κ��� ��ȯ���ش�. 
}

//ȣ�� �� ������ �ð��� 1�ʾ� ���ҽ����ش�.
void CARPLayer::timeminus()
{
	LPCACHE_LIST prev, current;

	prev = cache_header;
	current = prev->next;	  
	

	while(current != NULL)   //current�� NULL ���� �ƴ϶��  
	{														
		current->CACHE_List.cache_time = current->CACHE_List.cache_time-1;	// �ش� ���(ARP Cache)�� �ð��� 1�� ���ҽ����ش�.
		
		if(current->CACHE_List.cache_time <= 0)				//�ð��� 0���� ���ų� ������ time out�̹Ƿ� ����Ʈ���� ���������ش�.
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

// �۽�
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


// ����
unsigned char* CARPLayer::Receive()
{
	unsigned char* ppayload = mp_UnderLayer->Receive();	//Ethernet���κ��� ��Ŷ receive

	if (ppayload != NULL){
		LPARP pFrame = (LPARP) ppayload;	//���� ���̾ ����
		
		unsigned char	id[4];			//������ ip
		unsigned char	is[4];			//���� �� ip
		unsigned char	ss[4];			//�۽��� ip
		memset(id, 0, 4);
		memset(is, 0, 4);
		memset(ss, 0, 4);

		unsigned char temp[4];
		memset(temp, 0, 4);

		unsigned char F = 255;
		memcpy(id, pFrame->arp_dstprotoaddr, 4);
		memcpy(is, m_arp.arp_srcprotoaddr, 4);
		memcpy(ss, pFrame->arp_srcprotoaddr, 4);
		int i, check = 0;				// i �� check ������ �ʱ�ȭ���ְ� for���� ���� �˻�����

		
		for (i = 0; i < 4; i++)			//���� ��Ŷ�� ������ �´� ��쿡��
		{
			if ((id[i] != is[i])){
				check = 1;
				break;
			}
		}
		if (!check){
			check=0;
			for (i = 0; i < 4; i++)			 //���� ���� �� �ڽ��� ���� ��쿡�� �ѱ��.
			{
				 
				if ((is[i] != ss[i])){		//���� ������ ���� ����
					check=1;
					break;
				}
			}


			//ip�� ���ؼ� �ٸ� ���� �ϳ��� ������ ������ �������Ѵ�.

			if(check==1){
			
				if(ntohs(pFrame->arp_op) == ARP_REQ)
				{
					memcpy(swap_ip, pFrame->arp_dstprotoaddr ,4);
					memcpy(m_arp.arp_dsthdaddr, pFrame->arp_srchdaddr, 6);
					memcpy(m_arp.arp_dstprotoaddr, pFrame->arp_srcprotoaddr, 4);

					//cache_element ������ CACHE_LIST������ ũ�⸸ŭ �Ҵ����ش�.
					LPCACHE_LIST cache_element = (LPCACHE_LIST)malloc(sizeof(CACHE_LIST));
					LPCACHE_LIST pre = cache_header;

					while (pre != NULL)		//pre�� NULL�� �ƴ϶�� 
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
					cache_element->CACHE_List.cache_time = 1200;	//20��
					cache_element->CACHE_List.cache_type = 1;		//������� ����Ʈ�ΰ��


					//������ ����Ʈ�� �ڿ� �߰�
					pre = cache_header;

					while (pre->next != NULL)
					{
						pre = pre->next;
					}
					pre->next = cache_element;
					cache_element->next = NULL;

					return m_arp.arp_dstprotoaddr;
				}
				//������ ���� ���̹Ƿ� ���̺� �߰��������.

				else if(ntohs(pFrame->arp_op) == ARP_REP)   //ARP���� 0x0002
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
					cache_element->CACHE_List.cache_time = 1200;//20��
					cache_element->CACHE_List.cache_type = 1;//������� ����Ʈ�ΰ��

					//������ ����Ʈ�� �ڿ� �߰�
					while(pre->next!=NULL)
					{
						pre=pre->next;
					}
					pre->next=cache_element;//���̺� �ڿ� �߰������ִ� �κ�
					cache_element->next=NULL;
					return temp;
				}
			}
		}
			check = 0;


			//���� ���� ���� �ƴѵ� ������ �Դٿ�
			//gratitousȮ�� for��

			for (i = 0; i < 4; i++) 
			{
				if ((id[i] != ss[i])){
					check = 1;
					break;
				}
			}

			if(check==0)		//gratitous ARP �� ��
			{
				
				BOOL check;
				check=Listcheck(ss);	//����Ʈ�� �����ϴ��� �˻�

				if(check==FALSE)		//check�� false��� ���̺� ���� ����
				{
					return 0;			//�׳� �������ִ� ��	
				}
				else{					//check true...		���̺� �ִ� ����
					SetDestinAddress(pFrame->arp_srchdaddr);
					Listchange(ss);		//���ּ� ����
					return 0;
				}
			}

			//target�� �ڽ��� �ƴѰ��, proxy table�� �ִ��� �˻����ִ� if��

			if(proxycheck(pFrame->arp_dstprotoaddr))	//Proxy table�� �����ϸ�
			{
				memcpy(m_arp.arp_dsthdaddr, pFrame->arp_srchdaddr, 6);
				memcpy(m_arp.arp_dstprotoaddr, pFrame->arp_srcprotoaddr, 4);
				memcpy(swap_ip, pFrame->arp_dstprotoaddr, 4);
				return pFrame->arp_srcprotoaddr;
			}
	}
	return 0;
}


//proxy table�� �ִ��� �˻� proxycheck  
BOOL CARPLayer::proxycheck(unsigned char* pAddress)
{
	LPPROXY_LIST seekList = proxy_header->next;  

	while(seekList!=NULL) //seekList�� NULL�� ������ while��..
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

//proxy table�� �߰�

BOOL CARPLayer::proxyadd()
{
	//PROXY_LIST�� �����ŭ ũ�⸦ �Ҵ����ش�.

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


//gratuitous�ΰ�� mac�� �ٲ��ش�

BOOL CARPLayer::Listchange(unsigned char* pAddress)		//mac�� �ٲ��ִ� Listchange
{
	LPCACHE_LIST seekList = cache_header->next;

	while(seekList!=NULL)
	{
		int check=1;

		//ip�� �������� �˻����ݴϴ�.
		for(int i=0;i<4;i++)
		{
			if(seekList->CACHE_List.cache_pAddress[i]!=pAddress[i])
			{
				check=0;
				break;
			}
		}
		if(check==1) //check Ÿ���� ���� ���� ���� �ִٸ� mac�� �ٲ��ְ� Ÿ���� �ٲ��ش�.
		{
			memcpy(seekList->CACHE_List.cache_enetaddr,m_arp.arp_dsthdaddr,6);
			seekList->CACHE_List.cache_type=1;
			return TRUE;
		}
		seekList=seekList->next;
	}

	return FALSE;
}

//proxy ����
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
		if(check==1)	//proxy Table�� �˻��ؼ� ����� ���� ���� �ִٸ� check�� 1�� �����̹Ƿ� �� �ش� �κ��� �����ش�.
		{
			pre->next = current->next;
			free(current);
			break;
		}
		pre = current;
		current = current->next;
	}
	if(current == NULL)			//current�� NULL�̶�� FALSE�� �������ְ�
		return FALSE;

	return TRUE;				//NULL�ƴϸ� TRUE�� �������ش�.  
}

//ĳ�� ���̺� ����
BOOL CARPLayer::DeleteCache(unsigned char* pAddress)
{
	LPCACHE_LIST pre, current;

	pre = cache_header;
	current = pre->next;
	
	while(current != NULL)
	{
		int check=1;

		//������ ��ġ�� IP�� �˻��� �Ѵ�.
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

//���� ���� �ش� �Լ�

void CARPLayer::AllDeleteCache() 
{
	LPCACHE_LIST temp_list, temp_element;
	temp_list = cache_header->next;
	cache_header->next = NULL;

	while(temp_list != NULL) //list�� NULL�� �ƴѰ�쿡�� ����Ʈ�� ���鼭 ��� entry�� �������ش�.
	{
		temp_element = temp_list;
		temp_list = temp_list->next;
		free(temp_element);
	}	
}


//���̺� �ִ��� �˻�

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
		//���̺� �ְ� MAC�� �ִ� ��� ���� true
		if(check==1&&seekList->CACHE_List.cache_type==1)
			return TRUE;

		//���̺��� ������ MAC�� �𸣴� ���, ���̺��� �����ϰ� ARP����
		else if(check==1&&seekList->CACHE_List.cache_type==0)
		{
			DeleteCache(seekList->CACHE_List.cache_pAddress);
			return FALSE;
		}
		seekList=seekList->next;
	}

	return FALSE;
}


//������ ���� ��쿡�� �߰��� ���ش�.

BOOL CARPLayer::FailAddress()
{
	LPCACHE_LIST cache_element = (LPCACHE_LIST)malloc(sizeof(CACHE_LIST));
	memcpy(cache_element->CACHE_List.cache_pAddress, m_arp.arp_dstprotoaddr, 4);		
	cache_element->CACHE_List.cache_time = 180;			//������ ���� ��� (incomplete..) 180�ʷ� �������ش�.
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