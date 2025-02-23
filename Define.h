#pragma once
#define WIN32_LEAN_AND_MEAN 

#include <winsock2.h>
#include <ws2tcpip.h>
#include <mswsock.h>
#include <cstdint>

const uint32_t MAX_SOCK = 1024; // Set Max Socket Buf
const uint32_t MAX_RECV_DATA = 8096;

std::string JWT_SECRET = "quokka_lover";

class User;
class ServerProcessor;

enum class TaskType {
	ACCEPT = 0,
	RECV = 1,
	SEND = 2
};

struct OverlappedEx {
	// 4 bytes
	TaskType taskType; // ACCPET, RECV, SEND INFO

	WSAOVERLAPPED wsaOverlapped;
};

struct OverlappedTCP : OverlappedEx {
	// 8 bytes
	User* user;
	ServerProcessor* serverProcessor;

	int a;
	// 16 bytes
	WSABUF wsaBuf; // TCP Buffer
};
