#pragma once
#pragma comment(lib, "ws2_32.lib") // ���־󿡼� �������α׷��� �ϱ� ���� ��
#pragma comment(lib,"mswsock.lib") //AcceptEx�� ����ϱ� ���� ��

#define SERVER_IP "127.0.0.1"
#define TO_USER_PORT 9091
#define PACKET_SIZE 1024
#define SEND_QUEUE_CNT 5
#define MAX_OVERLAP_CNT 10

#include <jwt-cpp/jwt.h>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <mswsock.h>
#include <string>
#include <thread>
#include <atomic>
#include <sw/redis++/redis++.h>
//#include <boost/lockfree/queue.hpp>

#include "Packet.h"
#include "Define.h"
#include "User.h"
#include "MySQLManager.h"

class UserProcessor {
public:
    ~UserProcessor() {
        if (userProcThread.joinable()) {
            userProcThread.join();
        }
        CloseHandle(u_IOCPHandle);
        closesocket(userIOSkt);
        WSACleanup();
        delete user;

        std::cout << "userProcThread End" << std::endl;
    }

	bool init(uint16_t threadCnt_, std::shared_ptr<sw::redis::RedisCluster> redis_, MySQLManager* mysqlManager_) {
        WSADATA wsadata;
        int check = 0;
        threadCnt = threadCnt_;

        check = WSAStartup(MAKEWORD(2, 2), &wsadata);
        if (check) {
            std::cout << "WSAStartup Fail" << std::endl;
            return false;
        }

        userIOSkt = WSASocket(AF_INET, SOCK_STREAM, IPPROTO_TCP, NULL, NULL, WSA_FLAG_OVERLAPPED);
        if (userIOSkt == INVALID_SOCKET) {
            std::cout << "Make Server Socket Fail" << std::endl;
            return false;
        }

        SOCKADDR_IN addr;
        addr.sin_port = htons(TO_USER_PORT);
        addr.sin_family = AF_INET;
        addr.sin_addr.s_addr = htonl(INADDR_ANY);

        check = bind(userIOSkt, (SOCKADDR*)&addr, sizeof(addr));
        if (check) {
            std::cout << "bind fail" << std::endl;
            return false;
        }

        check = listen(userIOSkt, SOMAXCONN);
        if (check) {
            std::cout << "listen fail" << std::endl;
            return false;
        }

        u_IOCPHandle = CreateIoCompletionPort(INVALID_HANDLE_VALUE, NULL, NULL, threadCnt_);
        if (u_IOCPHandle == NULL) {
            std::cout << "Make Iocp Hande Fail" << std::endl;
            return false;
        }

        auto bIOCPHandle = CreateIoCompletionPort((HANDLE)userIOSkt, u_IOCPHandle, (ULONG_PTR)0, 0);
        if (bIOCPHandle == nullptr) {
            std::cout << "Bind Iocp Hande Fail" << std::endl;
            return false;
        }

        user = new User();
        user->init(u_IOCPHandle);

        //for (int i = 0; i < MAX_OVERLAP_CNT; i++) {
        //    OverlappedTCP* temp = new OverlappedTCP;
        //    overLapPool.push(temp);
        //}

        redis = redis_;
        mysqlManager = mysqlManager_;

        CreateUserProcThread();
        
        user->PostAccept(userIOSkt, u_IOCPHandle);

        return true;
	}

    void CreateUserProcThread() {
        userProcRun = true;
        userProcThread = std::thread([this]() {WorkRun(); });
    }

    void WorkRun() {
        LPOVERLAPPED lpOverlapped = NULL;
        DWORD dwIoSize = 0;
        User* tempUser = nullptr;
        bool gqSucces = TRUE;

        while (userProcRun) {
            gqSucces = GetQueuedCompletionStatus(
                u_IOCPHandle,
                &dwIoSize,
                (PULONG_PTR)&tempUser,
                &lpOverlapped,
                INFINITE
            );

            auto overlappedTCP = (OverlappedTCP*)lpOverlapped;

            tempUser = overlappedTCP->user;
            int a = overlappedTCP->a;


            if (!gqSucces || (dwIoSize == 0 && overlappedTCP->taskType != TaskType::ACCEPT)) { // User Disconnect
                std::cout << "������ ���� ����. ���� �ʱ�ȭ" << std::endl;
                tempUser->Reset(u_IOCPHandle);
                tempUser->PostAccept(userIOSkt, u_IOCPHandle);
                continue;
            }

            if (a==0) {
                tempUser->UserRecv();
            }
            else if (a==1) {
                auto k = reinterpret_cast<PACKET_HEADER*>(overlappedTCP->wsaBuf.buf);
                
                if (k->PacketId == (uint16_t)WEBPACKET_ID::USER_GAMESTART_REQUEST) {
                    auto ugReq = reinterpret_cast<USER_GAMESTART_REQUEST*>(overlappedTCP->wsaBuf.buf);
                    GameStart(tempUser, ugReq);
                    std::cout << ugReq->userId << " Game Start Request" << std::endl;
                    delete[] overlappedTCP->wsaBuf.buf;
                    delete overlappedTCP;
                }

                //ZeroMemory(overlappedTCP,sizeof(OverlappedTCP)); // Push After Init
                //overLapPool.push(overlappedTCP); // Push OverLappedTcp
                //sendQueueSize.fetch_add(1);
            }
            else if (a==2) {
                delete[] overlappedTCP->wsaBuf.buf;
                delete overlappedTCP;

                user->Reset(u_IOCPHandle);
                user->PostAccept(userIOSkt, u_IOCPHandle);

                //ZeroMemory(overlappedTCP, sizeof(OverlappedTCP)); // Push After Init
                //overLapPool.push(overlappedTCP); // Push OverLappedTcp
                //sendQueueSize.fetch_add(1);
            }

        }
    }

    void GameStart(User* tempUser, USER_GAMESTART_REQUEST* ugReq) {
        // ���� Ŭ�����Ϳ� �ڿ� {}�� ID�� �ϸ� ID�� �ѹ��� ������ �ٲٴϱ� ������ �ʴ� PK�� ����.
        uint16_t pk = mysqlManager->GetPkById(ugReq->userId);
        if (pk == 0) {
            std::cout << "GetPkById() Fail" << std::endl;
            tempUser->Reset(u_IOCPHandle);
            tempUser->PostAccept(userIOSkt, u_IOCPHandle);
            return;
        }

        std::string pk_s = std::to_string(pk);

        if (!mysqlManager->GetUserEquipByPk(pk_s)) {
            std::cout << "GetUserEquipByPk() Get Fail" << std::endl;
            tempUser->Reset(u_IOCPHandle);
            tempUser->PostAccept(userIOSkt, u_IOCPHandle);
            return;
         }

        if (!mysqlManager->GetUserInvenByPk(pk_s)) {
            std::cout << "GetUserInvenByPk() Get Fail" << std::endl;
            tempUser->Reset(u_IOCPHandle);
            tempUser->PostAccept(userIOSkt, u_IOCPHandle);
            return;
        }

        std::string token = jwt::create()
            .set_issuer("web_server")
            .set_subject("Login_check")
            .set_expires_at(std::chrono::system_clock::now() + std::chrono::seconds{ 1800 }) // ��ʺ����� ����
            .sign(jwt::algorithm::hs256{ JWT_SECRET });

        std::string tag = "{" + std::string(ugReq->userId) + "}";
        std::string key = "jwtcheck:" + tag;

        auto pipe = redis->pipeline(tag);

        pipe.hset(key, token, std::to_string(pk))
            .expire(key, 1800); // set ttl 1 hour

        pipe.exec();
		USER_GAMESTART_RESPONSE ugRes; 
		ugRes.PacketId = (UINT16)WEBPACKET_ID::USER_GAMESTART_RESPONSE;
		ugRes.PacketLength = sizeof(USER_GAMESTART_RESPONSE);
		strncpy_s(ugRes.webToken, token.c_str(), 256);
        tempUser->UserSend(ugRes);
    }

private:
    bool userProcRun = false;
    //std::atomic<uint16_t> sendQueueSize{ 0 };

    uint16_t threadCnt = 0;

	SOCKET userIOSkt;
    SOCKET userSkt;
    HANDLE u_IOCPHandle;

    std::thread userProcThread;

    std::shared_ptr<sw::redis::RedisCluster> redis;

    std::string userToken;

    MySQLManager* mysqlManager;

    User* user;

    //boost::lockfree::queue<OverlappedTCP*> SendQueue{ SEND_QUEUE_CNT };
    //boost::lockfree::queue<OverlappedTCP*> overLapPool{ MAX_OVERLAP_CNT };
};