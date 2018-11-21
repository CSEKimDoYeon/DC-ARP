// EthernetLayer.h: interface for the CEthernetLayer class.
//
/////////////////////////////////////////////////////////////////////

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "BaseLayer.h"


class CEthernetLayer 
: public CBaseLayer  
{
private:
	inline void		ResetHeader( );

public:
	unsigned char*	Receive();
	BOOL			Send( unsigned char* ppayload, int nlength,int port);
	void			SetDestinAddress( unsigned char* pAddress );
	void			SetSourceAddress( unsigned char* pAddress );
	unsigned char*	GetDestinAddress( );
	unsigned char*	GetSourceAddress( ) ;

	unsigned char		enet_temp_dstaddr[6];

	CEthernetLayer( char* pName );
	virtual ~CEthernetLayer();

	typedef struct _ETHERNET_HEADER {

		unsigned char	enet_dstaddr[6]	;// destination address
		unsigned char	enet_srcaddr[6]	;// source address
		unsigned short	enet_type;		// type
		unsigned char	enet_data[ ETHER_MAX_DATA_SIZE ] ;

	} ETHERNET_HEADER, *PETHERNET_HEADER ;

protected:
	ETHERNET_HEADER	m_sHeader ;
};