// TCPLayer.h: interface for the TCPLayer class.
//
/////////////////////////////////////////////////////////////////////

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "BaseLayer.h"


class CIPLayer
	: public CBaseLayer
{
private:
	inline void		ResetHeader();

public:
	unsigned char*	Receive();
	int				Send(int port);
	void			SetDestinAddress(unsigned char* pAddress);
	void			SetSourceAddress(unsigned char* pAddress);
	unsigned char*	GetDestinAddress();
	unsigned char*	GetSourceAddress();
	CIPLayer(char* pName);
	virtual ~CIPLayer();

		typedef struct _IP_ADDR {
		
		union {
			struct { unsigned char s_b1, s_b2, s_b3, s_b4 ; } S_un_b;
			struct { unsigned short s_w1, s_w2 ; } S_un_w;
			unsigned long S_addr;
			unsigned char S_ip_addr[4];
		} S_un ;
		
#define s_addr		S_un.S_addr			// can be used for most tcp & ip code
#define s_host		S_un.S_un_b.s_b2	// 호스트 on imp
#define s_net		S_un.S_un_b.s_b1	// network
#define s_imp		S_un.S_un_w.s_w2	// imp
#define s_impno		S_un.S_un_b.s_b4	// imp #
#define s_lh		S_un.S_un_b.s_b3	// logical 호스트
		
#define iaddr		S_un.S_ip_addr		// array
#define iaddr0		S_un.S_un_b.s_b1	// byte 0
#define iaddr1		S_un.S_un_b.s_b2	// byte 1
#define iaddr2		S_un.S_un_b.s_b3	// byte 2
#define iaddr3		S_un.S_un_b.s_b4	// byte 3
		
	} IP_ADDR, *LPIP_ADDR ;
	
	
	typedef struct _IP_HEADER {
		
		unsigned char	ip_verlen;		// ip version
		unsigned char	ip_tos;			// type of service
		unsigned short	ip_len;			// total packet length
		unsigned short	ip_id;			// datadram id
		unsigned short	ip_fragoff;		// fragment offset
		unsigned char	ip_ttl;			// time to live, in gateway hops
		unsigned char	ip_proto;		// IP protocol
		unsigned short	ip_cksum;		// header checksum
		IP_ADDR			ip_src;			// IP address of source
		IP_ADDR			ip_dst;			// IP address of destination
		unsigned char	ip_data[APP_DATA_SIZE];		// variable length data
		
	} IP_HEADER, *PIP_HEADER ;
protected:
	IP_HEADER	m_sHeader;
};