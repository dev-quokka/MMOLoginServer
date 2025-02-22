#pragma once



#include <winsock2.h>
#include <ws2tcpip.h>
#include <mswsock.h>
#include <string>
#include <thread>
#include <atomic>
#include <iostream>

#include "Define.h"

class User {
public:
    ~User() {
        closesocket(userSkt);
    }

    bool init (HANDLE u_IOCPHandle) {

        userSkt = WSASocket(AF_INET, SOCK_STREAM, IPPROTO_IP, NULL, 0, WSA_FLAG_OVERLAPPED);

        if (userSkt == INVALID_SOCKET) {
            std::cout << "Client socket Error : " << GetLastError() << std::endl;
        }

        auto tIOCPHandle = CreateIoCompletionPort((HANDLE)userSkt, u_IOCPHandle, (ULONG_PTR)this, 0);

        if (tIOCPHandle == INVALID_HANDLE_VALUE)
        {
            std::cout << "reateIoCompletionPort()�Լ� ���� :" << GetLastError() << std::endl;
        }
        return true;
    }

    void Reset(HANDLE u_IOCPHandle) {
        shutdown(userSkt, SD_BOTH);
        closesocket(userSkt);
        userSkt = WSASocket(AF_INET, SOCK_STREAM, IPPROTO_IP, NULL, 0, WSA_FLAG_OVERLAPPED);

        if (userSkt == INVALID_SOCKET) {
            std::cout << "Client socket Error : " << GetLastError() << std::endl;
        }

        auto tIOCPHandle = CreateIoCompletionPort((HANDLE)userSkt, u_IOCPHandle, (ULONG_PTR)this, 0);

        if (tIOCPHandle == INVALID_HANDLE_VALUE)
        {
            std::cout << "reateIoCompletionPort()�Լ� ���� :" << GetLastError() << std::endl;
        }
    }

    bool UserRecv() {
        //OverlappedTCP* tempOvLap;

        //if (overLapPool.pop(tempOvLap)) {
        //    sendQueueSize.fetch_sub(1);

        DWORD dwFlag = 0;
        DWORD dwRecvBytes = 0;

        recvOvLap = new OverlappedTCP;
        ZeroMemory(recvOvLap, sizeof(OverlappedTCP));
        recvOvLap->wsaBuf.len = MAX_SOCK;
        recvOvLap->wsaBuf.buf = new char[MAX_SOCK];
        recvOvLap->user = this;
        recvOvLap->a = 1;
        recvOvLap->taskType = TaskType::RECV;

        int tempR = WSARecv(userSkt, &(recvOvLap->wsaBuf), 1, &dwRecvBytes, &dwFlag, (LPWSAOVERLAPPED)recvOvLap, NULL);

        if (tempR == SOCKET_ERROR && (WSAGetLastError() != ERROR_IO_PENDING))
        {
            std::cout << userSkt << " WSARecv() Fail : " << WSAGetLastError() << std::endl;
            return false;
        }

        return true;

        //}

        //else {
        //    std::cout << "Recv Fail, OverLapped Pool Full" << std::endl;
        //    return false;
        //}
    }

    void UserSend(USER_GAMESTART_RESPONSE ugRes) {
        //OverlappedTCP* overlappedTCP;

        //if (SendQueue.pop(overlappedTCP)) {
            //sendQueueSize.fetch_sub(1);

        DWORD dwSendBytes = 0;

        sendOvLap = new OverlappedTCP;
        ZeroMemory(sendOvLap, sizeof(OverlappedTCP));
        sendOvLap->wsaBuf.len = MAX_SOCK;
        sendOvLap->wsaBuf.buf = new char[MAX_SOCK];
        CopyMemory(sendOvLap->wsaBuf.buf, &ugRes, sizeof(USER_GAMESTART_RESPONSE));
        sendOvLap->user = this;
        sendOvLap->a = 2;
        sendOvLap->taskType = TaskType::SEND;

        int sCheck = WSASend(userSkt,
            &(sendOvLap->wsaBuf),
            1,
            &dwSendBytes,
            0,
            (LPWSAOVERLAPPED)sendOvLap,
            NULL);
        //}

        //else std::cout << "Send Fail, OverLappend Pool Full" << std::endl;
    }


    bool PostAccept(SOCKET userIOSkt, HANDLE u_IOCPHandle) {
        DWORD bytes = 0;
        DWORD flags = 0;

        aceeptOvLap = {};

        aceeptOvLap.taskType = TaskType::ACCEPT;
        aceeptOvLap.user = this;
        aceeptOvLap.wsaBuf.len = 0;
        aceeptOvLap.wsaBuf.buf = nullptr;
        aceeptOvLap.a = 0;

        if (AcceptEx(userIOSkt, userSkt, acceptBuf, 0, sizeof(SOCKADDR_IN) + 16, sizeof(SOCKADDR_IN) + 16, &bytes, (LPWSAOVERLAPPED)&aceeptOvLap) == 0) {
            if (WSAGetLastError() != WSA_IO_PENDING) {
                std::cout << "AcceptEx Error : " << GetLastError() << std::endl;
                return false;
            }
        }

        std::cout << "AcceptEx �Լ� ����" << std::endl;

        return true;
    }
private:
	SOCKET userSkt;

    OverlappedTCP aceeptOvLap;
    OverlappedTCP* sendOvLap;
    OverlappedTCP* recvOvLap;

    char acceptBuf[64];
};