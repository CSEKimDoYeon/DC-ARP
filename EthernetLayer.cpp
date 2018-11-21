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
// 헤더 초기화
void CEthernetLayer::ResetHeader()
{
	memset(m_sHeader.enet_dstaddr, 0, 6);
	memset(m_sHeader.enet_srcaddr, 0, 6);
	memset(m_sHeader.enet_data, 0, ETHER_MAX_DATA_SIZE);
	m_sHeader.enet_type = 0;
}
//Source Mac 반환
unsigned char* CEthernetLayer::GetSourceAddress()
{
	return m_sHeader.enet_srcaddr;
}
//Destination Mac 반환
unsigned char* CEthernetLayer::GetDestinAddress()
{
	return enet_temp_dstaddr;
}
//Source Mac 세팅
void CEthernetLayer::SetSourceAddress(unsigned char *pAddress)
{
	memcpy(m_sHeader.enet_srcaddr, pAddress, 6);
}
//Destination Mac 세팅
void CEthernetLayer::SetDestinAddress(unsigned char *pAddress)
{
	memcpy(m_sHeader.enet_dstaddr, pAddress, 6);
}
/* 송신 */
BOOL CEthernetLayer::Send(unsigned char *ppayload, int nlength,int port)
{
	memset(m_sHeader.enet_data, 0, ETHER_MAX_DATA_SIZE);
	memcpy(m_sHeader.enet_data, ppayload, nlength) ;
	BOOL bSuccess = FALSE ;
	
	if(port == 1)// 브로드캐스트
		memset(m_sHeader.enet_dstaddr,255,6);
	else if(port == 2)//특정 주소에 전송
		memcpy(m_sHeader.enet_dstaddr,enet_temp_dstaddr,6);
	
	m_sHeader.enet_type = htons(ARP_TYPE);// host byte order --> network byte order
	bSuccess = mp_UnderLayer->Send( (unsigned char*)&m_sHeader, nlength + ETHER_HEADER_SIZE);

	return bSuccess;
}
/* 수신 */
unsigned char* CEthernetLayer::Receive()
{
	//NILayer로부터 패킷을 받아옴
	unsigned char* ppayload = mp_UnderLayer->Receive();

	//주소를 저장할 변수
	unsigned char	dst[6];
	unsigned char	src[6];
	memset(dst, 0, 6);
	memset(src, 0, 6);
	//받아온 패킷이 있는 경우
	if (ppayload != NULL){
		PETHERNET_HEADER pFrame = (PETHERNET_HEADER) ppayload;
		if(ntohs(pFrame->enet_type)==ARP_TYPE){//ARP_TYPE인지 확인 network byte order --> host byte order
			unsigned char F = 255;
			//송신자,수신자의 MAC 저장
			memcpy(dst, pFrame->enet_dstaddr, 6);
			memcpy(src, pFrame->enet_srcaddr, 6);
			
			int i, check = 0;
			//브로드캐스트인지 확인
			for (i = 0; i < 6; i++) 
				if ((dst[i] != F)){
					check = 1;
					break;
			}
			if (!check){//브로드캐스트인 경우
					memcpy(enet_temp_dstaddr,src,6); //응답하여 반환할 destination MAC을 저장
					return pFrame->enet_data;
			}
			check = 0;
			// 목적지가 자신인지 확인
			for (i = 0; i < 6; i++) 
				if ((dst[i] != m_sHeader.enet_srcaddr[i])){
					check = 1;
					break;
				}
			if (!check){
					memcpy(enet_temp_dstaddr,src,6); // reply 해 줄 destination MAC을 저장
					return pFrame->enet_data;
			}
		}
	}
	return 0;
}

