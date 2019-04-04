#pragma once
#include "IOChannel.h"
#include "_Win32Socket.h"
#include <vector>
#include <iostream>
#include <thread>
#include <mutex>

class Server
{
public:
	void Start(int port, DecodaMsgFunc, RecvMsgFunc);
	void SendToAll(char* buff, int len);
	Server();
	~Server();
private:
	DecodaMsgFunc mDecodaFunc;
	RecvMsgFunc mRecvFunc;
	bool mRunning;
	WSADATA Data;
	SOCKADDR_IN  mSockAddr;
	void AcceptThreadFunc();
	SOCKET mSocket;
	int mStatus;
	thread* mAcceptThread;
	std::vector<IOChannel* >* mIOlist;
	mutex mLock;

};

