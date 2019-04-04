#include "stdafx.h"
#include "Client.h"
#include "IOChannel.h"
#pragma comment(lib,"ws2_32.lib")

Client::Client()
{
	mStatus = WSAStartup(MAKEWORD(1, 1), &Data);
	mIOChannel = new IOChannel;
}
 
bool Client::Send(char*buff,int len)
{
	return  mIOChannel->Send(buff, len);
	
}

void Client::Connect(std::string ip, int port, DecodaMsgFunc decFunc, RecvMsgFunc recFunc)
{
	mPort = port;
	WSADATA Data;
	SOCKADDR_IN destSockAddr;
	SOCKET destSocket;
	unsigned long destAddr;
 
	destAddr = inet_addr(ip.c_str());
	memcpy(&destSockAddr.sin_addr, &destAddr, sizeof(destAddr));
	destSockAddr.sin_port = htons(port);
	destSockAddr.sin_family = AF_INET;
	destSocket = socket(AF_INET, SOCK_STREAM, 0);
	if (destSocket == INVALID_SOCKET)
	{
		return;
	}
	/* connect to the server */
	mStatus = connect(destSocket, (LPSOCKADDR)&destSockAddr, sizeof(destSockAddr));
	if (mStatus == SOCKET_ERROR)
	{
		return;
	}
	else
	{
		mSocket = destSocket;
		mIOChannel->Init(mSocket, decFunc, recFunc);
	}
}

Client::~Client()
{
 
	delete mIOChannel;
	::closesocket(mSocket);
}
