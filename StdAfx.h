// stdafx.h : include file for standard system include files,
//  or project specific include files that are used frequently, but
//      are changed infrequently


#if !defined(AFX_STDAFX_H__119ECB1B_6E70_4662_A2A9_A20B5201CA81__INCLUDED_)
#define AFX_STDAFX_H__119ECB1B_6E70_4662_A2A9_A20B5201CA81__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#define VC_EXTRALEAN		// Exclude rarely-used stuff from Windows headers

#include <afxwin.h>         // MFC core and standard components
#include <afxext.h>         // MFC extensions
#include <afxdisp.h>        // MFC Automation classes
#include <afxdtctl.h>		// MFC support for Internet Explorer 4 Common Controls
#ifndef _AFX_NO_AFXCMN_SUPPORT
#include <afxcmn.h>			// MFC support for Windows Common Controls
#endif // _AFX_NO_AFXCMN_SUPPORT


//{{AFX_INSERT_LOCATION}}

#define MAX_LAYER_NUMBER		0xff
#define MAX_NI_SIZE				42
#define ETHER_MAX_SIZE			1514											//1514
#define ETHER_HEADER_SIZE		14												//14
#define ETHER_MAX_DATA_SIZE		( ETHER_MAX_SIZE - ETHER_HEADER_SIZE )			//1500

#define IP_HEADER_SIZE			20												//10
#define IP_DATA_SIZE			1480
//#define IP_HEADER_SIZE			10												//10
//#define IP_DATA_SIZE			1490											//1490

#define TCP_HEADER_SIZE			6												//6
#define TCP_DATA_SIZE			1484											//1484

#define CHAT_REC				0x01//used APP_DATA_MSG (REC)
#define CHAT_NOTREC				0x02//used APP_DATA_MSG (NOTREC)

#define	CHAT_TYPE				0x2080			
#define	FILE_TYPE				0x2090
#define ARP_TYPE				0x0806

#define ARP_REQ					0x0001
#define ARP_REP					0x0002


//1+2+1=4
#define APP_HEADER_SIZE			( sizeof(unsigned char)  +		    		\
								  sizeof(unsigned short) +					\
								  sizeof(unsigned char)	)
//1500-(4+6+10)=1480
#define APP_DATA_SIZE			( ETHER_MAX_DATA_SIZE - ( APP_HEADER_SIZE +		\
												          ETHER_HEADER_SIZE +		\
												          0 ) )
#define ARP_SIZE				28
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_STDAFX_H__119ECB1B_6E70_4662_A2A9_A20B5201CA81__INCLUDED_)
#include <afxcontrolbars.h>
