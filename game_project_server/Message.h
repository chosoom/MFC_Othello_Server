#pragma once

struct msgRecvMessage {
	TCHAR name[50];
	TCHAR str[2000];
};

struct msgMessage {
	int id;
	int size;
	msgRecvMessage data;
};

struct readyRecvMessage {
	TCHAR name[50];
	int ready;
};

struct readyMessage {
	int id;
	int size;
	readyRecvMessage data;
};

struct gameMessage {
	int m_map[8][8];
};

struct gameRecvMessage {
	int id;
	int size;
	gameMessage data;
};

struct turnMessage {
	bool turn;
};

struct turnRecvMessage
{
	int id;
	int size;
	turnMessage data;
};

struct choiceMessage {
	int choice;
	int result;
};
struct choiceRecvMessage {
	int id;
	int size;
	choiceMessage data;
};
