#pragma once
#include <vector>
#include <condition_variable>
#include <iostream>
#include <thread>
#include <mutex>
#include <windows.h>
#include <winsock.h>

using namespace std;
#define MAXBUFLEN 1*1024*1024
using namespace std;
typedef bool(*DecodaMsgFunc)(char* buff, int size, int &len);
typedef void(*RecvMsgFunc)(int channelID, std::vector<char> && data);

class semaphore
{
public:
	semaphore(int value = 1) :count(value) {}
	void wait();
	void signal();
 
private:
	int count;
	mutex mtk;
	condition_variable cv;
};

class IOChannel
{
public:
	IOChannel();
	~IOChannel();
	bool Init(SOCKET socket, DecodaMsgFunc, RecvMsgFunc);
	bool Send(char* buff, int len);
	int ChannelID;
private:
	DecodaMsgFunc mDecodaFunc;
	RecvMsgFunc mRecvFunc;

	int mPort;
	bool mRunning;
	WSADATA Data;
	SOCKADDR_IN  mSockAddr;
	SOCKET mSocket;
	int mStatus;
	mutex mSendLock;
	thread* mRecvThread;
	thread* mSendThread;
	char* mRecvBuff;
	char* mSendBuff;

	int mSendBuffLen;
	void RecvThreadFunc();
	void SendThreadFunc();

	
	semaphore* mSendSemaphore;
	semaphore* mRecvSemaphore;
};

