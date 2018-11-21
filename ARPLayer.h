// ArpLayer.h: interface for the CArpLayer class.
//
/////////////////////////////////////////////////////////////////////

#if !defined(AFX_ARPLAYER_H__CEFEEDC0_FA5B_4029_8E6A_3DAEAB9DBEE1__INCLUDED_)
#define AFX_ARPLAYER_H__CEFEEDC0_FA5B_4029_8E6A_3DAEAB9DBEE1__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "BaseLayer.h"

typedef struct Cache_List {
	unsigned char	cache_pAddress[4];
	unsigned char	cache_enetaddr[6];
	unsigned char	cache_type;
	unsigned short	cache_time;
} CACHE_List, *LPCACHE_List;

typedef struct CACHE_LIST *LPCACHE_LIST;
struct CACHE_LIST {
	CACHE_List		CACHE_List;
	LPCACHE_LIST	next;
};		// Cache list �ڷ���

typedef struct Proxy_List {
	unsigned char	cache_pAddress[4];
	unsigned char	cache_enetaddr[6];
} PROXY_List, *LPPROXY_List;

typedef struct PROXY_LIST *LPPROXY_LIST;
struct PROXY_LIST {
	PROXY_List	CACHE_List;
	LPPROXY_LIST	next;
};		// Proxy list �ڷ���

class CARPLayer 
: public CBaseLayer  
{

public:
	CARPLayer( char* pName );
	virtual ~CARPLayer();

	unsigned char swap_ip[4];
	unsigned char pi[4];
	unsigned char pe[4];

	BOOL			Send( unsigned char* ppayload, int nlength,int op);
	unsigned char*	Receive();

	void			SetSourceIPAddress(unsigned char *pAddress);
	void			SetDesIPAddress(unsigned char *pAddress);
	void			SetDestinAddress( unsigned char* pAddress );
	void			SetSourceAddress( unsigned char* pAddress );
	unsigned char*	GetDesIPAddress( );
	unsigned char*	GetSourceIPAddress( );
	unsigned char*	GetDestinAddress( );
	unsigned char*	GetSourceAddress( ) ;
	void SetProxyIPAddress(unsigned char *pAddress);
	void SetProxEtherAddress(unsigned char *pAddress);

		
	LPCACHE_LIST GetCacheList();//CacheList�� ��ȯ
	LPPROXY_LIST GetProxyList();//ProxyList�� ��ȯ
	BOOL DeleteCache(unsigned char* pAddress);//CacheList���� item����
	BOOL Listcheck(unsigned char* pAddress);//List�� �ִ��� Ȯ��
	BOOL Listchange(unsigned char* pAddress);//Mac�ּҸ� ����
	BOOL FailAddress();//������ ���°��
	BOOL proxyadd();//Proxy�߰�
	void AllDeleteCache(); //CacheList���� ����
	BOOL DeleteProxy(unsigned char* pAddress);//Proxy����
	BOOL proxycheck(unsigned char* pAddress);//Proxy�� �ִ��� �˻�
	void timeminus();//�ð� �ٿ��ִ� �Լ�

	typedef struct _IP_ADDR {
		
		union {
			struct { unsigned char s_b1, s_b2, s_b3, s_b4 ; } S_un_b;
			struct { unsigned short s_w1, s_w2 ; } S_un_w;
			unsigned long S_addr;
			unsigned char S_ip_addr[4];
		} S_un ;
		
#define s_addr		S_un.S_addr			// can be used for most tcp & ip code
#define s_host		S_un.S_un_b.s_b2	// ȣ��Ʈ on imp
#define s_net		S_un.S_un_b.s_b1	// network
#define s_imp		S_un.S_un_w.s_w2	// imp
#define s_impno		S_un.S_un_b.s_b4	// imp #
#define s_lh		S_un.S_un_b.s_b3	// logical ȣ��Ʈ
		
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

	
	
	
	typedef struct _ARP {
		unsigned short	arp_hdtype;
		unsigned short	arp_prototype;
		unsigned char	arp_hdlength;
		unsigned char	arp_protolength;
		unsigned short	arp_op;
		
		unsigned char	arp_srchdaddr[6];
		unsigned char	arp_srcprotoaddr[4];
		unsigned char	arp_dsthdaddr[6];
		unsigned char	arp_dstprotoaddr[4];
		
	} ARP, *LPARP;
	
	ARP	m_arp;
	LPCACHE_LIST cache_header;
	LPPROXY_LIST proxy_header;
};


#endif // !defined(AFX_ARPLAYER_H__CEFEEDC0_FA5B_4029_8E6A_3DAEAB9DBEE1__INCLUDED_)
