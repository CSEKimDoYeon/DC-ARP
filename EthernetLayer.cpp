// EthernetLayer.cpp: implementation of the CEthernetLayer class.
//
/////////////////////////////////////////////////////////////////////

#include "StdAfx.h"
#include "ipc.h"
#include "EthernetLayer.h"
#include "NILayer.h"
#include "IPCAppDlg.h"


#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE [] = __FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CEthernetLayer::CEthernetLayer(char* pName)
: CBaseLayer(pName)
{
	ResetHeader();
}

CEthernetLayer::~CEthernetLayer()
{
}
// ��� �ʱ�ȭ
void CEthernetLayer::ResetHeader()
{
	memset(m_sHeader.enet_dstaddr, 0, 6);
	memset(m_sHeader.enet_srcaddr, 0, 6);
	memset(m_sHeader.enet_data, 0, ETHER_MAX_DATA_SIZE);
	m_sHeader.enet_type = 0;
}
//Source Mac ��ȯ
unsigned char* CEthernetLayer::GetSourceAddress()
{
	return m_sHeader.enet_srcaddr;
}
//Destination Mac ��ȯ
unsigned char* CEthernetLayer::GetDestinAddress()
{
	return enet_temp_dstaddr;
}
//Source Mac ����
void CEthernetLayer::SetSourceAddress(unsigned char *pAddress)
{
	memcpy(m_sHeader.enet_srcaddr, pAddress, 6);
}
//Destination Mac ����
void CEthernetLayer::SetDestinAddress(unsigned char *pAddress)
{
	memcpy(m_sHeader.enet_dstaddr, pAddress, 6);
}
/* �۽� */
BOOL CEthernetLayer::Send(unsigned char *ppayload, int nlength,int port)
{
	memset(m_sHeader.enet_data, 0, ETHER_MAX_DATA_SIZE);
	memcpy(m_sHeader.enet_data, ppayload, nlength) ;
	BOOL bSuccess = FALSE ;
	
	if(port == 1)// ��ε�ĳ��Ʈ
		memset(m_sHeader.enet_dstaddr,255,6);
	else if(port == 2)//Ư�� �ּҿ� ����
		memcpy(m_sHeader.enet_dstaddr,enet_temp_dstaddr,6);
	
	m_sHeader.enet_type = htons(ARP_TYPE);// host byte order --> network byte order
	bSuccess = mp_UnderLayer->Send( (unsigned char*)&m_sHeader, nlength + ETHER_HEADER_SIZE);

	return bSuccess;
}
/* ���� */
unsigned char* CEthernetLayer::Receive()
{
	//NILayer�κ��� ��Ŷ�� �޾ƿ�
	unsigned char* ppayload = mp_UnderLayer->Receive();

	//�ּҸ� ������ ����
	unsigned char	dst[6];
	unsigned char	src[6];
	memset(dst, 0, 6);
	memset(src, 0, 6);
	//�޾ƿ� ��Ŷ�� �ִ� ���
	if (ppayload != NULL){
		PETHERNET_HEADER pFrame = (PETHERNET_HEADER) ppayload;
		if(ntohs(pFrame->enet_type)==ARP_TYPE){//ARP_TYPE���� Ȯ�� network byte order --> host byte order
			unsigned char F = 255;
			//�۽���,�������� MAC ����
			memcpy(dst, pFrame->enet_dstaddr, 6);
			memcpy(src, pFrame->enet_srcaddr, 6);
			
			int i, check = 0;
			//��ε�ĳ��Ʈ���� Ȯ��
			for (i = 0; i < 6; i++) 
				if ((dst[i] != F)){
					check = 1;
					break;
			}
			if (!check){//��ε�ĳ��Ʈ�� ���
					memcpy(enet_temp_dstaddr,src,6); //�����Ͽ� ��ȯ�� destination MAC�� ����
					return pFrame->enet_data;
			}
			check = 0;
			// �������� �ڽ����� Ȯ��
			for (i = 0; i < 6; i++) 
				if ((dst[i] != m_sHeader.enet_srcaddr[i])){
					check = 1;
					break;
				}
			if (!check){
					memcpy(enet_temp_dstaddr,src,6); // reply �� �� destination MAC�� ����
					return pFrame->enet_data;
			}
		}
	}
	return 0;
}

