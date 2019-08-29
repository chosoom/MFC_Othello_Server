// ServerSocket.cpp : implementation file
//

#include "stdafx.h"
#include "game_project_server.h"
#include "ServerSocket.h"


// CServerSocket

CServerSocket::CServerSocket()
	: clientCount(0)
{
}

CServerSocket::~CServerSocket()
{
}


// CServerSocket member functions
void CServerSocket::SetWnd(HWND hWnd) {
	m_hWnd = hWnd;
}

void CServerSocket::OnAccept(int nErrorCode)
{
	// TODO: Add your specialized code here and/or call the base class
	CClientSocket* pClient = new CClientSocket;
	if (Accept(*pClient)) {
		pClient->SetWnd(m_hWnd);
	}
	else {
		delete pClient;
		AfxMessageBox(_T("ERROR: Failed can't accept net Client"));
	}
	SendMessage(m_hWnd, WM_ACCEPT_SOCKET, 0, (LPARAM)pClient);
	CSocket::OnAccept(nErrorCode);
}
