#pragma once
#include "ClientSocket.h"
#define WM_ACCEPT_SOCKET WM_USER+1
// CServerSocket command target

class CServerSocket : public CSocket
{
public:
	CServerSocket();
	virtual ~CServerSocket();
	void SetWnd(HWND hWnd);
	HWND m_hWnd;
	virtual void OnAccept(int nErrorCode);
	int clientCount;
};


