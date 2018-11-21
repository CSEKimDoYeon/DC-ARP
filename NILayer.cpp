// NILayer.cpp: implementation of the CNILayer class.
//
//////////////////////////////////////////////////////////////////////

#include "StdAfx.h"
#include "ipc.h"
#include "NILayer.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

/////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CNILayer::CNILayer(char* pName, pcap_t* pAdapterObject,
		int iNumAdapter): CBaseLayer( pName )
{
	m_pAdapterObjects = pAdapterObject;
	m_iNumAdapter = iNumAdapter;
	SetNICList();
}

CNILayer::~CNILayer()
{

}
//NIC 목록을 가져옴
void CNILayer::SetNICList()	
{
	pcap_if_t *d;
	char errbuf[256];

	if(pcap_findalldevs(&m_allDevs, errbuf) == -1){
		fprintf(stderr, "Error in pcap_findalldevs: %s\n", errbuf);
		exit(1);
	}
	for(d = m_allDevs; d; d=d->next)
		m_iNumAdapter++;
}

void CNILayer::setAdapterNum(int index)
{
	char errbuf[256];

	getAdapterName(index);
	
	if ((m_pAdapterObjects = pcap_open_live(m_adapterName,	// name of the device
							 65536,			// portion of the packet to capture. 
											// 65536 grants that the whole packet will be captured on all the MACs.
							 1,				// promiscuous mode (nonzero means promiscuous)
							 1000,			// read timeout
							 errbuf			// error buffer
							 )) == NULL)
    {
        fprintf(stderr,"\nUnable to open the adapter. %s is not supported by WinPcap\n");
		exit(1);        // Free the device list
    }
	// save adapter's number
	m_selectedNum = index;
}
int CNILayer::getAdapterNum()//get adapter number
{
	return m_iNumAdapter;
}

char* CNILayer::getAdapterName(int index)//get adapter name
{
	pcap_if_t *d = m_allDevs;

	for(int i=0; i<index; i++)
		d = d -> next;

	strcpy(m_adapterName, d->name);

	return m_adapterName;
}

pcap_if_t* CNILayer::getAdapter(int index) //get adapter
{
	pcap_if_t *d = m_allDevs;

	for(int i=0; i<index; i++)
		d = d -> next;
	
	return d;
}


//해당되는 NIC의 MAC주소 설정
void CNILayer::setMacAddress()
{
	LPADAPTER lpAdapter = 0;
	DWORD dwErrorCode;
	PPACKET_OID_DATA OidData;
	BOOL Status;

	getAdapterName(m_selectedNum);//get adapter name
	lpAdapter = PacketOpenAdapter(m_adapterName);//open adapter

	if (!lpAdapter || (lpAdapter->hFile == INVALID_HANDLE_VALUE)){	
		dwErrorCode=GetLastError();	// if fail to open
	}

	OidData = (PPACKET_OID_DATA) malloc(6 + sizeof(PACKET_OID_DATA));// Allocate a buffer to get the MAC adress
	if (OidData == NULL) {
		PacketCloseAdapter(lpAdapter);// fail to malloc
	}

	OidData->Oid = OID_802_3_CURRENT_ADDRESS;// Retrieve the adapter MAC querying the NIC driver
	OidData->Length = 6;
	ZeroMemory(OidData->Data, 6);
	
	Status = PacketRequest(lpAdapter, FALSE, OidData);

	if(Status){//retrun value copy OidData to m_MacAddress
		memcpy(m_MacAddress, (OidData->Data), 6);
	}
	else{
		printf("error retrieving the MAC address of the adapter!\n");
	}

	free(OidData);//free OidData
	PacketCloseAdapter(lpAdapter);//close adapter
}
unsigned char* CNILayer::getMacAddress()
{
	return m_MacAddress;
}
BOOL CNILayer::Send(unsigned char* ppayload, int nlength)
{
	OID_GEN_MEDIA_CONNECT_STATUS;//requests the miniport to return the connection status of the NIC on the network as one of the following system-defined values:
	//패킷 전송
	if(pcap_sendpacket(m_pAdapterObjects, ppayload, nlength) != 0)
	{
		fprintf(stderr, "\nError sending the packet: \n", pcap_geterr(m_pAdapterObjects));
		return FALSE;
	}
	return TRUE;
}
//패킷 받음
unsigned char* CNILayer::Receive()
{
	int result;
	struct pcap_pkthdr *header;
	unsigned char data[MAX_NI_SIZE];
	const unsigned char* pkt_data;
	
	memset(data, '\0', MAX_NI_SIZE);
	
	while((result = pcap_next_ex( m_pAdapterObjects, &header, &pkt_data)) >= 0) {
		if(result == 0)
			continue;
		
		memcpy(data, pkt_data, MAX_NI_SIZE);
		return data;
	}

	if(result = -1) {
		printf("Error reading the packets : %s\n", pcap_geterr(m_pAdapterObjects));
		return 0;
	}

	return 0;
}

