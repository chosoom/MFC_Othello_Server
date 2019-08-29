// ClikentSocket.cpp : implementation file
//

#include "stdafx.h"
#include "game_project_server.h"
#include "ClientSocket.h"


// CClikentSocket

CClientSocket::CClientSocket()
	: m_rsp_choice(-1)
{
}

CClientSocket::~CClientSocket()
{
}


// CClikentSocket member functions

void CClientSocket::SetWnd(HWND hWnd) {
	m_hWnd = hWnd;
}


void CClientSocket::OnClose(int nErrorCode)
{
	// TODO: Add your specialized code here and/or call the base class
	SendMessage(m_hWnd, WM_CLIENT_CLOSE, 0, (LPARAM)this);
	CSocket::OnClose(nErrorCode);
}


void CClientSocket::OnReceive(int nErrorCode)
{
	// TODO: Add your specialized code here and/or call the base class
	int header[2];
	
	Receive((char*)header, sizeof(int) * 2);

	if (header[0] == 2) {
		CString strTmp = _T(""),strIPAddress=_T("");
		UINT uPortNum = 0;
		msgRecvMessage *msg = new msgRecvMessage;
		ZeroMemory(msg, sizeof(msgRecvMessage));
		Receive((char*)msg, header[1]);
		GetPeerName(strIPAddress, uPortNum);
		strTmp.Format(_T("[%s:%d:%d:%s]:%s"), strIPAddress, uPortNum,int(this), msg->name, msg->str);
		SendMessage(m_hWnd, WM_CLIENT_MSG_RECV, 0, (LPARAM)((LPCTSTR)strTmp));
		strTmp.Format(_T("[%s]:%s"), msg->name, msg->str);
		SendMessage(m_hWnd, WM_CLIENT_MSG_SEND, 0, (LPARAM)((LPCTSTR)strTmp));
	}
	if (header[0] == 3) {
		readyRecvMessage *msg = new readyRecvMessage;
		ZeroMemory(msg, sizeof(readyRecvMessage));
		Receive((char*)msg, header[1]);
		this->m_name = msg->name;
		SendMessage(m_hWnd, WM_CLIENT_READY, 0, (LPARAM)this);
	}
	if (header[0] == 6) {
		gameMessage *msg = new gameMessage;
		ZeroMemory(msg, sizeof(gameMessage));
		Receive((char*)msg, header[1]);
		SendMessage(m_hWnd, WM_CLIENT_GAME_RECV, 0, (LPARAM)msg);
	}
	if (header[0] == 7) {
		SendMessage(m_hWnd, WM_CLIENT_TURN_RECV, 0, (LPARAM)this);
	}
	/*바위 받아오기*/
	if (header[0] == 401) {
		choiceMessage* msg = new choiceMessage;
		CString str;
		ZeroMemory(msg, sizeof(choiceMessage));
		Receive((char *)msg, header[1]);
		this->m_rsp_choice = msg->choice;
		//CPP-> 메인다이얼로그로 보냄 int형으로 보낸다
		SendMessage(m_hWnd, WM_RSP_CHOICE_RECV, 0, (LPARAM)this);
	}
	CSocket::OnReceive(nErrorCode);
}
