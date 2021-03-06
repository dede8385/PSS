// DlgForbidenIP.cpp : implementation file
//

#include "stdafx.h"
#include "PSS_ClientManager.h"
#include "DlgForbidenIP.h"


// CDlgForbidenIP dialog

IMPLEMENT_DYNAMIC(CDlgForbidenIP, CDialog)

CDlgForbidenIP::CDlgForbidenIP(CWnd* pParent /*=NULL*/)
: CDialog(CDlgForbidenIP::IDD, pParent)
{
	m_pTcpClientConnect = NULL;
}

CDlgForbidenIP::~CDlgForbidenIP()
{
}

void CDlgForbidenIP::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_EDIT1, m_txtForbidenIP);
	DDX_Control(pDX, IDC_EDIT2, m_txtForbidenSeconds);
	DDX_Control(pDX, IDC_LIST1, m_lcForbidenList);
	DDX_Control(pDX, IDC_RADIO1, m_btnTimeForbiden);
}


BEGIN_MESSAGE_MAP(CDlgForbidenIP, CDialog)
	ON_BN_CLICKED(IDC_BUTTON1, &CDlgForbidenIP::OnBnClickedButton1)
	ON_BN_CLICKED(IDC_BUTTON2, &CDlgForbidenIP::OnBnClickedButton2)
	ON_BN_CLICKED(IDC_BUTTON3, &CDlgForbidenIP::OnBnClickedButton3)
END_MESSAGE_MAP()

CString CDlgForbidenIP::GetPageTitle()
{
	return _T("IP封禁管理");
}


// CDlgForbidenIP message handlers

void CDlgForbidenIP::OnBnClickedButton1()
{
	//添加封禁
	char szIP[20]      = {'\0'};
	char szSeconds[20] = {'\0'};
	CString strIP;
	CString strSeconds;
	int     nType = 0;

	m_txtForbidenIP.GetWindowText(strIP);

	int nSrcLen = WideCharToMultiByte(CP_ACP, 0, strIP, strIP.GetLength(), NULL, 0, NULL, NULL);
	int nDecLen = WideCharToMultiByte(CP_ACP, 0, strIP, nSrcLen, szIP, 20, NULL,NULL);
	szIP[nDecLen] = '\0';

	nSrcLen = WideCharToMultiByte(CP_ACP, 0, strSeconds, strSeconds.GetLength(), NULL, 0, NULL, NULL);
	nDecLen = WideCharToMultiByte(CP_ACP, 0, strSeconds, nSrcLen, szSeconds, 20, NULL,NULL);
	szSeconds[nDecLen] = '\0';

	if(strlen(szIP) == 0)
	{
		MessageBox(_T(MESSAGE_INSERT_NULL) , _T(MESSAGE_TITLE_ERROR), MB_OK);
		return;
	}

	switch(GetCheckedRadioButton(IDC_RADIO1, IDC_RADIO2))
	{
	case IDC_RADIO1:
		nType = 0;
		break;
	case IDC_RADIO2:
		nType = 1;
		break;
	}

	char szSendMessage[200] = {'\0'};
	char szCommand[100]     = {'\0'};
	sprintf_s(szCommand, 100, "%s ForbiddenIP -c %s -t %d -s %s ", m_pTcpClientConnect->GetKey(), szIP, nType, szSeconds);
	int nSendLen = (int)strlen(szCommand); 

	memcpy_s(szSendMessage, 200, &nSendLen, sizeof(int));
	memcpy_s(&szSendMessage[4], 200, &szCommand, nSendLen);

	char szRecvBuff[10 * 1024] = {'\0'};
	int nRecvLen = 10 * 1024;
	bool blState = m_pTcpClientConnect->SendConsoleMessage(szSendMessage, nSendLen + sizeof(int), (char*)szRecvBuff, nRecvLen);
	if(blState == false)
	{
		MessageBox(_T(MESSAGE_SENDERROR) , _T(MESSAGE_TITLE_ERROR), MB_OK);
	}
	else
	{
		int nStrLen      = 0;
		int nPos         = 4;
		int nResult      = 0;
		memcpy_s(&nResult, sizeof(int), &szRecvBuff[nPos], sizeof(int));
		nPos += sizeof(int);

		if(nResult == 0)
		{
			MessageBox(_T(MESSAGE_RESULT_SUCCESS) , _T(MESSAGE_TITLE_ERROR), MB_OK);
		}
		else
		{
			MessageBox(_T(MESSAGE_RESULT_FAIL) , _T(MESSAGE_TITLE_SUCCESS), MB_OK);
			OnBnClickedButton3();
		}
	}
}

void CDlgForbidenIP::OnBnClickedButton2()
{
	//解除封禁
	char szIP[20]    = {'\0'};
	CString strIP;

	m_txtForbidenIP.GetWindowText(strIP);

	int nSrcLen = WideCharToMultiByte(CP_ACP, 0, strIP, strIP.GetLength(), NULL, 0, NULL, NULL);
	int nDecLen = WideCharToMultiByte(CP_ACP, 0, strIP, nSrcLen, szIP, 20, NULL,NULL);
	szIP[nDecLen] = '\0';

	if(strlen(szIP) == 0)
	{
		MessageBox(_T(MESSAGE_INSERT_NULL) , _T(MESSAGE_TITLE_ERROR), MB_OK);
		return;
	}

	char szSendMessage[200] = {'\0'};
	char szCommand[100]     = {'\0'};
	sprintf_s(szCommand, 100, "%s LiftedIP %s", m_pTcpClientConnect->GetKey(), szIP);
	int nSendLen = (int)strlen(szCommand); 

	memcpy_s(szSendMessage, 200, &nSendLen, sizeof(int));
	memcpy_s(&szSendMessage[4], 200, &szCommand, nSendLen);

	char szRecvBuff[10 * 1024] = {'\0'};
	int nRecvLen = 10 * 1024;
	bool blState = m_pTcpClientConnect->SendConsoleMessage(szSendMessage, nSendLen + sizeof(int), (char*)szRecvBuff, nRecvLen);
	if(blState == false)
	{
		MessageBox(_T(MESSAGE_SENDERROR) , _T(MESSAGE_TITLE_ERROR), MB_OK);
	}
	else
	{
		int nStrLen      = 0;
		int nPos         = 4;
		int nResult      = 0;
		memcpy_s(&nResult, sizeof(char), &szRecvBuff[nPos], sizeof(char));
		nPos += sizeof(char);

		if(nResult == 0)
		{
			MessageBox(_T(MESSAGE_RESULT_SUCCESS) , _T(MESSAGE_TITLE_ERROR), MB_OK);
		}
		else
		{
			MessageBox(_T(MESSAGE_RESULT_FAIL) , _T(MESSAGE_TITLE_SUCCESS), MB_OK);
			OnBnClickedButton3();
		}
	}
}

void CDlgForbidenIP::SetTcpClientConnect( CTcpClientConnect* pTcpClientConnect )
{
	m_pTcpClientConnect = pTcpClientConnect;
}

BOOL CDlgForbidenIP::OnInitDialog()
{
	CDialog::OnInitDialog();

	m_lcForbidenList.InsertColumn(0, _T("IP地址"), LVCFMT_CENTER, 150);
	m_lcForbidenList.InsertColumn(1, _T("封禁类型"), LVCFMT_CENTER, 100);
	m_lcForbidenList.InsertColumn(2, _T("封禁开始时间"), LVCFMT_CENTER, 200);
	m_lcForbidenList.InsertColumn(3, _T("封禁持续秒数"), LVCFMT_CENTER, 100);

	return TRUE;
}

void CDlgForbidenIP::OnBnClickedButton3()
{
	//查看列表
	m_lcForbidenList.DeleteAllItems();

	vecForbiddenIP objvecForbiddenIP;

	char szSendMessage[200] = {'\0'};
	char szCommand[100]     = {'\0'};
	sprintf_s(szCommand, 100, "%s ShowForbiddenIP -a", m_pTcpClientConnect->GetKey());
	int nSendLen = (int)strlen(szCommand); 

	memcpy_s(szSendMessage, 200, &nSendLen, sizeof(int));
	memcpy_s(&szSendMessage[4], 200, &szCommand, nSendLen);

	char szRecvBuff[10 * 1024] = {'\0'};
	int nRecvLen = 10 * 1024;
	bool blState = m_pTcpClientConnect->SendConsoleMessage(szSendMessage, nSendLen + sizeof(int), (char*)szRecvBuff, nRecvLen);
	if(blState == false)
	{
		MessageBox(_T(MESSAGE_SENDERROR) , _T(MESSAGE_TITLE_ERROR), MB_OK);
		return;
	}
	else
	{
		int nStrLen        = 0;
		int nPos           = 4;
		int nForbidenCount = 0;
		memcpy_s(&nForbidenCount, sizeof(int), &szRecvBuff[nPos], sizeof(int));
		nPos += sizeof(int);

		for(int i = 0; i < nForbidenCount; i++)
		{
			//开始还原数据结构
			_ForbiddenIP ForbiddenIP;

			memcpy_s(&nStrLen, sizeof(char), &szRecvBuff[nPos], sizeof(char));
			nPos += sizeof(char);

			memcpy_s(ForbiddenIP.m_szIP, nStrLen, &szRecvBuff[nPos], nStrLen);
			nPos += nStrLen;
			ForbiddenIP.m_szIP[nStrLen] = '\0';

			memcpy_s(&ForbiddenIP.m_nType, sizeof(char), &szRecvBuff[nPos], sizeof(char));
			nPos += sizeof(char);

			char szUpdateTime[30] = {'\0'};
			memcpy_s(&ForbiddenIP.m_nBeginTime, sizeof(int), &szRecvBuff[nPos], sizeof(int));
			nPos += sizeof(int);
			struct tm tmDate;
			time_t newRawTime = ForbiddenIP.m_nBeginTime;
			localtime_s(&tmDate, &newRawTime);
			sprintf_s(szUpdateTime, 30, "%04d-%02d-%02d %02d:%02d:%02d", tmDate.tm_year + 1900, 
				tmDate.tm_mon + 1,
				tmDate.tm_mday,
				tmDate.tm_hour,
				tmDate.tm_min,
				tmDate.tm_sec);

			memcpy_s(&ForbiddenIP.m_nSecond, sizeof(int), &szRecvBuff[nPos], sizeof(int));
			nPos += sizeof(int);

			//显示在桌面上
			wchar_t szForbidenIP[200]    = {'\0'};
			wchar_t szzUpdateTime[30] = {'\0'};

			int nSrcLen = MultiByteToWideChar(CP_ACP, 0, ForbiddenIP.m_szIP, -1, NULL, 0);
			int nDecLen = MultiByteToWideChar(CP_ACP, 0, ForbiddenIP.m_szIP, -1, szForbidenIP, 200);

			nSrcLen = MultiByteToWideChar(CP_ACP, 0, szUpdateTime, -1, NULL, 0);
			nDecLen = MultiByteToWideChar(CP_ACP, 0, szUpdateTime, -1, szzUpdateTime, 200);

			CString strSeconds;
			CString strType;

			if(ForbiddenIP.m_nType == 0)
			{
				strType = _T("永久封禁");
			}
			else
			{
				strType = _T("时段封禁");
			}

			strSeconds.Format(_T("%d"), ForbiddenIP.m_nSecond);

			m_lcForbidenList.InsertItem(i, szForbidenIP);
			m_lcForbidenList.SetItemText(i, 1, strType);
			m_lcForbidenList.SetItemText(i, 2, szzUpdateTime);
			m_lcForbidenList.SetItemText(i, 3, strSeconds);

			objvecForbiddenIP.push_back(ForbiddenIP);
		}
	}
}
