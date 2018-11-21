// EthernetLayer.cpp: implementation of the CEthernetLayer class.
//
//////////////////////////////////////////////////////////////////////

#include "StdAfx.h"
#include "ipc.h"
#include "IPLayer.h"
#include "NILayer.h"
#include "IPCAppDlg.h"


#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#define new DEBUG_NEW
#endif

/////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CIPLayer::CIPLayer(char* pName)
: CBaseLayer(pName)
{
	ResetHeader();
}

CIPLayer::~CIPLayer()
{
}

void CIPLayer::ResetHeader()
{
	m_sHeader.ip_verlen=0;
	m_sHeader.ip_tos=0;
	m_sHeader.ip_len=0;
	m_sHeader.ip_id=0;
	m_sHeader.ip_fragoff=0;
	m_sHeader.ip_ttl=0;
	m_sHeader.ip_proto=0;
	m_sHeader.ip_cksum=0;

	memset(m_sHeader.ip_data, 0, APP_DATA_SIZE);
}
//return src
unsigned char* CIPLayer::GetSourceAddress()
{
	return m_sHeader.ip_src.S_un.S_ip_addr;
}
//return des 
unsigned char* CIPLayer::GetDestinAddress()
{
	return m_sHeader.ip_dst.S_un.S_ip_addr;
}
//set src
void CIPLayer::SetSourceAddress(unsigned char *pAddress)
{
	memcpy(m_sHeader.ip_src.S_un.S_ip_addr, pAddress, 4);
}
//set des
void CIPLayer::SetDestinAddress(unsigned char *pAddress)
{
	memcpy(m_sHeader.ip_dst.S_un.S_ip_addr, pAddress, 4);
}
//전송
BOOL CIPLayer::Send(int port)
{
	BOOL bSuccess = FALSE;

	bSuccess = mp_UnderLayer->Send((unsigned char*) &m_sHeader, IP_HEADER_SIZE, port);//하위 레이어에 전달

	return bSuccess;
}
//받음
unsigned char* CIPLayer::Receive()
{
	unsigned char* ppayload = mp_UnderLayer->Receive();
	BOOL bSuccess = FALSE;
	if(ppayload!=NULL)
	{
		if(ppayload[0]==1 && ppayload[1]==0 && ppayload[2]==0 && ppayload[3]==0)
		{
			this->mp_aUpperLayer[0]->Receive(m_sHeader.ip_dst.S_un.S_ip_addr,this->mp_UnderLayer->mp_UnderLayer->GetDestinAddress());
		}
		else
		{
			memcpy(m_sHeader.ip_dst.S_un.S_ip_addr, ppayload, 4);
			Send(2);
		}
	}
	
	return 0;
}
