#pragma once
#define NOMINMAX

#include <jwt-cpp/jwt.h>
#include <winsock2.h>
#include <windef.h>
#include <cstdint>
#include <iostream>
#include <random>
#include <unordered_map>
#include <sw/redis++/redis++.h>

#include "Packet.h"
#include "UserSyncData.h"
#include "ConnUsersManager.h"
#include "MySQLManager.h"

const std::string JWT_SECRET = "quokka_lover";
constexpr int MAX_LOGIN_PACKET_SIZE = 128;

class PacketManager {
public:
    ~PacketManager() {
        packetRun = false;

        for (int i = 0; i < packetThreads.size(); i++) { // Shutdown Redis Threads
            if (packetThreads[i].joinable()) {
                packetThreads[i].join();
            }
        }
    }

    void init(const uint16_t packetThreadCnt_);
    void SetManager(ConnUsersManager* connUsersManager_);
    void PushPacket(const uint16_t connObjNum_, const uint32_t size_, char* recvData_);

private:
    bool CreatePacketThread(const uint16_t packetThreadCnt_);
    void PacketRun(const uint16_t packetThreadCnt_);
    void PacketThread();

    //SYSTEM
    void ImLoginResponse(uint16_t connObjNum_, uint16_t packetSize_, char* pPacket_);

    void GetUserInfo(uint16_t connObjNum_, uint16_t packetSize_, char* pPacket_);
    void GetEquipment(uint16_t connObjNum_, uint16_t packetSize_, char* pPacket_);
    void GetConsumables(uint16_t connObjNum_, uint16_t packetSize_, char* pPacket_);
    void GetMaterials(uint16_t connObjNum_, uint16_t packetSize_, char* pPacket_);

    void GameStart(uint16_t connObjNum_, uint16_t packetSize_, char* pPacket_);

    typedef void(PacketManager::* RECV_PACKET_FUNCTION)(uint16_t, uint16_t, char*);

    // 5000 bytes
    thread_local static std::mt19937 gen;

    // 242 bytes
    sw::redis::ConnectionOptions connection_options;

    // 136 bytes
    boost::lockfree::queue<DataPacket> procSktQueue{ MAX_LOGIN_PACKET_SIZE };

    // 80 bytes
    std::unordered_map<uint16_t, RECV_PACKET_FUNCTION> packetIDTable;

    // 32 bytes
    std::vector<std::thread> packetThreads;
    std::vector<int> mapProbabilities = { 30, 30, 40 }; // Map probabilities on room creation

    // 16 bytes
    std::shared_ptr<sw::redis::RedisCluster> redis;
    std::thread packetThread;

    // 8 bytes
    std::uniform_int_distribution<int> dist;

    ConnUsersManager* connUsersManager;
    MySQLManager* mySQLManager;

    // 2 bytes
    uint16_t centerServerObjNum = 0;

    // 1 bytes
    bool packetRun = false;
};

