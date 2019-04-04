#include "stdafx.h"
#include "Server.h"

#pragma comment(lib,"ws2_32.lib")
Server::Server()
{
	mDecodaFunc = nullptr;
	mLock.lock();
	mAcceptThread = new thread([&]() {this->AcceptThreadFunc(); });
	mAcceptThread->detach();
	mIOlist = new std::vector<IOChannel*> ;
	mRunning = false;
	mStatus = WSAStartup(MAKEWORD(1, 1), &Data);
}

void Server::AcceptThreadFunc()
{
	mLock.lock();
	mLock.unlock();
	int len=0;
	SOCKET clientSocket;
	while (this->mRunning)
	{
		len = sizeof(SOCKADDR_IN);
		SOCKADDR_IN addr;
		SOCKET clientSocket = accept(mSocket, (LPSOCKADDR)&addr, &len);
		printf_s("****************** accept one");
		if (clientSocket == INVALID_SOCKET)
		{
			break;
		}
		else
		{
			IOChannel* oneIOChannel = new IOChannel;
			oneIOChannel->Init(clientSocket,this->mDecodaFunc,this->mRecvFunc);
			mIOlist->push_back(oneIOChannel);
		}
	}
	
}
 
void Server::SendToAll(char* buff,int len)
{
	for (auto oneIOChannel : *this->mIOlist)
	{
		oneIOChannel->Send(buff, len);
	}
}
void Server::Start(int port, DecodaMsgFunc decFunc, RecvMsgFunc recFunc)
{
	mRunning = true;
	mDecodaFunc = decFunc;
	mRecvFunc = recFunc;
	
	memset(&mSockAddr, 0, sizeof(mSockAddr));
	mSockAddr.sin_port = htons(port);
	mSockAddr.sin_family = AF_INET;
	mSockAddr.sin_addr.s_addr = htonl(INADDR_ANY);
	mSocket = socket(AF_INET, SOCK_STREAM, 0);
    mStatus = ::bind(mSocket, (LPSOCKADDR)&mSockAddr, sizeof(mSockAddr));

	if (mStatus == SOCKET_ERROR)
	{

	}

	mStatus = listen(mSocket, 1);
	mLock.unlock();
}

Server::~Server()
{
	mRunning = false;
	::closesocket(mSocket);
	
	::delete mAcceptThread;
 
}
