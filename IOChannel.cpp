#include "stdafx.h"
#include "IOChannel.h"

void semaphore::wait()
{
	unique_lock<mutex> lck(mtk);
	if (--count < 0)//��Դ��������߳�
		cv.wait(lck);
}

void semaphore::signal()
{
	unique_lock<mutex> lck(mtk);
	if (++count <= 0)//���̹߳��𣬻���һ��
		cv.notify_one();
}

 

IOChannel::IOChannel()
{
	static int gId = 1000;
	ChannelID = gId++;
	mSendBuff = 0;
	mDecodaFunc = nullptr;
	mRecvFunc = nullptr;
 
	mRecvThread = nullptr;
	mSendThread = nullptr;
	mRunning = false;
	mSendSemaphore = new semaphore(0);
	mRecvSemaphore = new semaphore(0);
	mRecvBuff = new char[MAXBUFLEN];
	mSendBuff = new char[MAXBUFLEN];
	mSendBuffLen = 0;
	mRecvThread = new thread([&]() {this->RecvThreadFunc(); });
	mRecvThread->detach();
	mSendThread = new thread([&]() {this->SendThreadFunc(); });
	mSendThread->detach();
	mSocket = INVALID_SOCKET;
}


IOChannel::~IOChannel()
{
	mRunning = false;
	::closesocket(mSocket);
}

bool IOChannel::Init(SOCKET socket, DecodaMsgFunc decodaFunc, RecvMsgFunc recvFunc)
{
	mSocket = socket;
	mDecodaFunc = decodaFunc;
	mRecvFunc   = recvFunc;
	mSendSemaphore->signal();
	mRecvSemaphore->signal();
	mRunning = true;
	return true;
}


bool IOChannel::Send(char*buff, int len)
{
	if (!this->mRunning)
		return false;
	if (len >= MAXBUFLEN)
		return false;
	if (mSendBuffLen + len >= MAXBUFLEN)
		return false;
	mSendLock.lock();
	::memcpy_s(mSendBuff + mSendBuffLen, MAXBUFLEN, buff, len);
	mSendBuffLen += len;
	mSendLock.unlock();
	mSendSemaphore->signal();
}


void IOChannel::RecvThreadFunc()
{
	mRecvSemaphore->wait();
	int buffLen = MAXBUFLEN;
	int buffBegin = 0;
	char* buffer = new char[MAXBUFLEN];
	while (this->mRunning)
	{
		/* recv(�����ӵ��׽���, �����������ݻ�������ָ��, ��������С, ������Ʒ�ʽ) */
		int numrcv = recv(mSocket, buffer + buffBegin, buffLen, 0);
		if ((numrcv == 0) || (numrcv == SOCKET_ERROR))
		{
			closesocket(mSocket);
			break;
		}
		else
		{
			buffBegin += numrcv;
			buffLen -= numrcv;
			if (buffLen <= 0) //buff���
				break;
			int msgLen = 0;
			if (this->mDecodaFunc(buffer, numrcv, msgLen))
			{
				std::vector<char>  data;
				data.insert(data.end(),buffer, buffer + msgLen);
				this->mRecvFunc(this->ChannelID,std::move(data));
				buffBegin = 0;
				buffLen = MAXBUFLEN;
			}
		}
	} /* while */
	delete buffer;
}


void IOChannel::SendThreadFunc()
{
	mSendSemaphore->wait();
	
	while (mRunning)
	{
		int hasSendLen = 0;
		mSendSemaphore->wait();
		mSendLock.lock();
		while (hasSendLen >= 0 && hasSendLen < mSendBuffLen)
		{
			int oneSendLend = send(mSocket, mSendBuff + hasSendLen, mSendBuffLen- hasSendLen, 0);
			hasSendLen += oneSendLend;

		}
		if (hasSendLen >= mSendBuffLen)
		{
			mSendBuffLen = 0;
		}
		mSendLock.unlock();
	}
	 
}
