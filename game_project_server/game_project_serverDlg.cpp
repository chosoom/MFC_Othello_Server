
// game_project_serverDlg.cpp : implementation file
//

#include "stdafx.h"
#include "game_project_server.h"
#include "game_project_serverDlg.h"
#include "afxdialogex.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// Cgame_project_serverDlg dialog
#define PORT 9999


Cgame_project_serverDlg::Cgame_project_serverDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(IDD_GAME_PROJECT_SERVER_DIALOG, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void Cgame_project_serverDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST_CLIENT, m_list_client);
	DDX_Control(pDX, IDC_LIST_MSG, m_list_msg);
	DDX_Control(pDX, IDC_LIST4, m_room_client_list);
}

BEGIN_MESSAGE_MAP(Cgame_project_serverDlg, CDialogEx)
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_MESSAGE(WM_ACCEPT_SOCKET, &Cgame_project_serverDlg::OnAcceptSocket)
	ON_MESSAGE(WM_CLIENT_MSG_RECV, &Cgame_project_serverDlg::OnClientMsgRecv)
	ON_MESSAGE(WM_CLIENT_CLOSE, &Cgame_project_serverDlg::OnClientClose)
	ON_MESSAGE(WM_CLIENT_MSG_SEND, &Cgame_project_serverDlg::OnClientMsgSend)
	ON_MESSAGE(WM_CLIENT_READY, &Cgame_project_serverDlg::OnClientReady)
	ON_MESSAGE(WM_CLIENT_GAME_RECV, &Cgame_project_serverDlg::OnClientGameRecv)
	ON_MESSAGE(WM_CLIENT_TURN_RECV, &Cgame_project_serverDlg::OnClientTurnRecv)
	ON_MESSAGE(WM_RSP_CHOICE_RECV, &Cgame_project_serverDlg::OnRspChoiceRecv)
END_MESSAGE_MAP()


// Cgame_project_serverDlg message handlers

BOOL Cgame_project_serverDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dialog
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon

	// TODO: Add extra initialization here
	m_serverSocket = new CServerSocket;
	m_serverSocket->SetWnd(this->m_hWnd);

	m_serverSocket->Create(PORT);
	m_serverSocket->Listen();

	return TRUE;  // return TRUE  unless you set the focus to a control
}

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void Cgame_project_serverDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // device context for painting

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// Center icon in client rectangle
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// Draw the icon
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}

// The system calls this function to obtain the cursor to display while the user drags
//  the minimized window.
HCURSOR Cgame_project_serverDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}



afx_msg LRESULT Cgame_project_serverDlg::OnAcceptSocket(WPARAM wParam, LPARAM lParam)
{
	CString str;
	m_clientSocket = (CClientSocket*)lParam;
	m_clientSocket->m_ready = false;
	m_ptrClientSocketList.AddTail(m_clientSocket);

	str.Format(_T("Client (%d)"), int(m_clientSocket));
	m_list_client.InsertString(-1, str);

	m_clientSocket = NULL;
	delete m_clientSocket;
	return 0;
}


afx_msg LRESULT Cgame_project_serverDlg::OnClientMsgRecv(WPARAM wParam, LPARAM lParam)
{
	LPCTSTR lpData = (LPCTSTR)lParam;

	m_list_msg.InsertString(-1, lpData);
	m_list_msg.SetCurSel(m_list_msg.GetCount() - 1);
	return 0;
}


afx_msg LRESULT Cgame_project_serverDlg::OnClientClose(WPARAM wParam, LPARAM lParam)
{
	CClientSocket *pClient = (CClientSocket*)lParam;
	CString str,strTmp;
	UINT idx = 0;
	POSITION pos = m_ptrClientSocketList.Find(pClient);

	if (pos != NULL) {
		str.Format(_T("Client (%d)"), (int)pClient);
		idx = m_list_client.SelectString(-1, (LPCTSTR)str);
		m_list_client.DeleteString(idx);

		m_ptrClientSocketList.RemoveAt(pos);
	}

	pos = m_room.clientList.Find(pClient);
	if (pos != NULL){

		idx = m_room_client_list.SelectString(-1, (LPCTSTR)str);
		m_room_client_list.DeleteString(idx);

		m_room.clientList.RemoveAt(pos);
	}

	delete pClient;
	return 0;
}


afx_msg LRESULT Cgame_project_serverDlg::OnClientMsgSend(WPARAM wParam, LPARAM lParam)
{
	LPCTSTR lpData = (LPCTSTR)lParam;
	POSITION pos = m_ptrClientSocketList.GetHeadPosition();

	while (pos != NULL) {
		CClientSocket *pClient = (CClientSocket*)m_ptrClientSocketList.GetNext(pos);
		if (pClient != NULL) {
			msgMessage *msg = new msgMessage;
			msg->id = 2;
			msg->size = sizeof(msgRecvMessage);
			_tcscpy_s(msg->data.str,lpData);
			_tcscpy_s(msg->data.name, _T(""));
			pClient->Send((char*)msg, sizeof(msgMessage)*2);
		}
	}

	return 0;
}


afx_msg LRESULT Cgame_project_serverDlg::OnClientReady(WPARAM wParam, LPARAM lParam)
{
	CClientSocket *pClient = (CClientSocket*)lParam;
	
	POSITION pos;
	if (m_room.clientList.GetCount() < 2 ) {
		pClient->m_ready = true;
		m_room.clientList.AddTail(pClient);
		CString str;
		str.Format(_T("Client (%d)"), int(pClient));
		m_room_client_list.InsertString(-1, str);
		if (m_room.clientList.GetCount() == 2) {
			pos = m_room.clientList.GetHeadPosition();
			int count = 0;
			if (pos != NULL) {
				CClientSocket *ptmp1 = (CClientSocket*)m_room.clientList.GetNext(pos);
				readyMessage *msg1 = new readyMessage;
				readyMessage *msg2 = new readyMessage;
				CClientSocket *ptmp2 = (CClientSocket*)m_room.clientList.GetNext(pos);
				
				msg1->id = 4;
				msg1->size = sizeof(readyRecvMessage);
				msg1->data.ready = 1;
				_tcscpy_s(msg1->data.name, ptmp2->m_name);
				ptmp1->Send((char*)msg1, sizeof(readyMessage) * 2);

				msg2->id = 4;
				msg2->size = sizeof(readyRecvMessage);
				msg2->data.ready = 0;
				_tcscpy_s(msg2->data.name, ptmp1->m_name);

				ptmp2->Send((char*)msg2, sizeof(readyMessage) * 2);
			}
		}
	}
	else{
		readyMessage *msg = new readyMessage;
		msg->id = 5;
		msg->size = sizeof(readyRecvMessage);
		msg->data.ready = -1;
		pClient->Send((char *)msg, sizeof(readyMessage) * 2);
	}

	
	return 0;
}


afx_msg LRESULT Cgame_project_serverDlg::OnClientGameRecv(WPARAM wParam, LPARAM lParam)
{
	gameMessage *pmsg = (gameMessage*)lParam;

	/*POSITION pos = m_room.clientList.Find(msg->m_clientSocket);

	if (pos != NULL) {
		pos = m_room.clientList.GetHeadPosition();
		
	}*/
	POSITION pos = m_room.clientList.GetHeadPosition();
	if (pos != NULL) {
		CClientSocket *pClient1 = (CClientSocket*)m_room.clientList.GetNext(pos);
		CClientSocket *pClient2 = (CClientSocket*)m_room.clientList.GetNext(pos);
		gameRecvMessage *msg = new gameRecvMessage;
		msg->id = 6;
		msg->size = sizeof(gameMessage);
		for (int i = 0; i < 8; i++) {
			for (int j = 0; j < 8; j++) {
				msg->data.m_map[i][j] = pmsg->m_map[i][j];
			}
		}
		pClient1->Send((char*)msg, sizeof(gameRecvMessage) * 2);

		pClient2->Send((char*)msg, sizeof(gameRecvMessage) * 2);
	}

	delete pmsg;
	return 0;
}


afx_msg LRESULT Cgame_project_serverDlg::OnClientTurnRecv(WPARAM wParam, LPARAM lParam)
{
	CClientSocket *pClient = (CClientSocket*)lParam;

	POSITION pos = m_room.clientList.Find(pClient);
	if (pos != NULL) {
		pos = m_room.clientList.GetHeadPosition();
		while (pos != NULL) {
			CClientSocket *tmp = (CClientSocket*)m_room.clientList.GetNext(pos);
			turnRecvMessage *msg = new turnRecvMessage;
			msg->id = 7;
			msg->size = sizeof(turnMessage);
			msg->data.turn = true;
			tmp->Send((char*)msg, sizeof(turnRecvMessage) * 2);
		}
	}

	return 0;
}



afx_msg LRESULT Cgame_project_serverDlg::OnRspChoiceRecv(WPARAM wParam, LPARAM lParam)
{
	CClientSocket *pClient = (CClientSocket*)lParam;

	POSITION pos = m_room.clientList.Find(pClient);

	int count = 0;
	if (pos != NULL) {
		CClientSocket *tmp = (CClientSocket*)m_room.clientList.GetAt(pos);
		tmp->m_rsp_choice = pClient->m_rsp_choice;
		pos = m_room.clientList.GetHeadPosition();
		while (pos != NULL) {
			tmp = (CClientSocket*)m_room.clientList.GetNext(pos);
			if (tmp->m_rsp_choice == -1) break;
			else count++;
		}
		if (count == 2) {
			int* result = new int[2];
			pos = m_room.clientList.GetHeadPosition();
			CClientSocket *tmp1 = (CClientSocket*)m_room.clientList.GetNext(pos);
			CClientSocket *tmp2 = (CClientSocket*)m_room.clientList.GetNext(pos);

			choiceRecvMessage *msg1 = new choiceRecvMessage;
			choiceRecvMessage *msg2 = new choiceRecvMessage;

			result=CompareResult(tmp1->m_rsp_choice, tmp2->m_rsp_choice);
			msg1->id = 402;
			msg1->size = sizeof(choiceMessage);
			msg1->data.choice = tmp2->m_rsp_choice;
			msg1->data.result = result[0];
			if (result[0] == 0) tmp2->m_rsp_choice = -1;
			tmp1->Send((char*)msg1, sizeof(choiceRecvMessage) * 2);

			msg2->id = 402;
			msg2->size = sizeof(choiceMessage);
			msg2->data.choice = tmp1->m_rsp_choice;
			msg2->data.result = result[1];
			if (result[1] == 0) tmp1->m_rsp_choice = -1;
			tmp2->Send((char*)msg2, sizeof(choiceRecvMessage) * 2);

		}
	}

	return 0;
}

int* Cgame_project_serverDlg::CompareResult(int player1,int player2) {

	int* result = new int[2];
	CString str;

	if (player1 == player2) {
		str = "비겼습니다.";
		//Client로 갈 최종결과값
		result[0] = 0;
		result[1] = 0;
	}
	if ((player1 == 1 && player2 == 2) || (player1 == 2 && player2 == 3) || (player1 == 3 && player2 == 1)) {
		str = "player1이 이겼음 player2가 졌음";
		//Client로 갈 최종결과값
		result[0] = 1;
		result[1] = -1;
	}
	else if ((player1 == 1 && player2 == 3) || (player1 == 2 && player2 == 1) || (player1 == 3 && player2 == 2)) {
		str = "player1이 졌음 player2가 이겼음";
		//Client로 갈 최종결과값
		result[0] = -1;
		result[1] = 1;
	}

	m_list_msg.InsertString(-1, str);


	return result;

	
}