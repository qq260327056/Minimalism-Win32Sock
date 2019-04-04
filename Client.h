#pragma once
#include <string>
#include "Server.h"
#include <condition_variable>
#include <iostream>
#include <thread>
#include <mutex>

using namespace std;


#include <iostream>
#include <thread>
#include <mutex>
 

using namespace std;



class Client
{
public:
	Client();
	~Client();
public:
	void Connect(std::string ip,int port, DecodaMsgFunc, RecvMsgFunc);
	bool Send(char*buff, int len);
private:

	int mPort;
	WSADATA Data;
	SOCKADDR_IN  mSockAddr;
	SOCKET mSocket;
	int mStatus;
	IOChannel* mIOChannel;

};

