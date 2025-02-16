#pragma once

#include <iostream>

#include <sw/redis++/redis++.h>

#include "UserProcessor.h"
#include "ServerProcessor.h"

constexpr uint16_t threadCnt = 1;

int main() {
	std::shared_ptr<sw::redis::RedisCluster> redis;
	sw::redis::ConnectionOptions connection_options;

    try {
        connection_options.host = "127.0.0.1";  // Redis Cluster IP
        connection_options.port = 7001;  // Redis Cluster Master Node Port
        connection_options.socket_timeout = std::chrono::seconds(10);
        connection_options.keep_alive = true;

        // Redis Ŭ������ ����
        redis = std::make_shared<sw::redis::RedisCluster>(connection_options);
        std::cout << "Redis Ŭ������ ���� ����!" << std::endl;

    }
    catch (const  sw::redis::Error& err) {
        std::cout << "Redis ���� �߻�: " << err.what() << std::endl;
    }

    UserProcessor userProcessor;
    ServerProcessor serverProcessor;

	userProcessor.init(threadCnt, redis);


}