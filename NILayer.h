// NILayer.h: interface for the CNILayer class.
//
/////////////////////////////////////////////////////////////////////

#if !defined(AFX_NILAYER_H__204C0D4B_2678_445B_BE49_D42C24DDC32D__INCLUDED_)
#define AFX_NILAYER_H__204C0D4B_2678_445B_BE49_D42C24DDC32D__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "BaseLayer.h"
#include "pcap.h"
#include "packet32.h"
#include "ntddndis.h"

class CNILayer : public CBaseLayer  
{
public:
	CNILayer(char* pName, pcap_t* pAdapterObject = NULL,
		int iNumAdapter = 0);
	CNILayer();
	virtual ~CNILayer();

	unsigned char*		Receive();
	BOOL		Send(unsigned char* ppayload, int nlength);
	void		SetNICList();
	void		setAdapterNum(int index);
	int			getAdapterNum();
	char*		getAdapterName(int index);
	pcap_if_t*	getAdapter(int index);
	void		setMacAddress();
	unsigned char*	getMacAddress();
	

private:
	unsigned char	m_MacAddress[6];	// Mac Adress's value
	char			m_adapterName[1024];
	int				m_selectedNum;		// adapter's index
	int				m_iNumAdapter;
	pcap_t*			m_pAdapterObjects;
	pcap_if_t*		m_allDevs;

};

#endif // !defined(AFX_NILAYER_H__204C0D4B_2678_445B_BE49_D42C24DDC32D__INCLUDED_)
