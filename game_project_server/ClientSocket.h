#pragma once
#include "Message.h"

// CClikentSocket command target
#define WM_CLIENT_MSG_RECV WM_USER+6
#define WM_CLIENT_CLOSE WM_USER+7
#define WM_CLIENT_MSG_SEND WM_USER+8
#define WM_CLIENT_READY WM_USER+9
#define WM_CLIENT_GAME_RECV WM_USER+10
#define WM_CLIENT_TURN_RECV WM_USER+11
#define WM_RSP_CHOICE_RECV WM_USER+12


class CClientSocket : public CSocket
{
public:
	CClientSocket();
	virtual ~CClientSocket();
	void SetWnd(HWND hWnd);
	HWND m_hWnd;
	virtual void OnClose(int nErrorCode);
	virtual void OnReceive(int nErrorCode);
	CString m_name;
	bool m_ready;
	int m_rsp_choice;
};

class Room {
public:
	int Num;
	CObList clientList;
	Room() {

	}
	~Room() {

	}
};