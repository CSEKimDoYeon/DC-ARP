// GratitousAlert.cpp : ���� �����Դϴ�.

#include "stdafx.h"
#include "ipc.h"
#include "GratitousAlert.h"
#include "afxdialogex.h"


// GratitousAlert ��ȭ �����Դϴ�.

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


// GratitousAlert �޽��� ó�����Դϴ�.
