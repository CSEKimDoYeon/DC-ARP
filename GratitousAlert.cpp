// GratitousAlert.cpp : 구현 파일입니다.

#include "stdafx.h"
#include "ipc.h"
#include "GratitousAlert.h"
#include "afxdialogex.h"


// GratitousAlert 대화 상자입니다.

IMPLEMENT_DYNAMIC(GratitousAlert, CDialogEx)

GratitousAlert::GratitousAlert(CWnd* pParent /*=NULL*/)
	: CDialogEx(GratitousAlert::IDD, pParent)
{

}

GratitousAlert::~GratitousAlert()
{
}

void GratitousAlert::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(GratitousAlert, CDialogEx)
END_MESSAGE_MAP()


// GratitousAlert 메시지 처리기입니다.
