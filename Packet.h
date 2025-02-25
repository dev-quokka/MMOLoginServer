#pragma once
#define WIN32_LEAN_AND_MEAN

#include <winsock2.h>
#include <cstdint>
#include <string>
#include <vector>
#include <chrono>

const uint16_t PACKET_ID_SIZE = 57; // Last Packet_ID Num + 1

struct PACKET_HEADER
{
	uint16_t PacketLength;
	uint16_t PacketId;
};

struct USERINFO {
	uint16_t level;
	unsigned int exp;
	USERINFO() : level(0), exp(0) {}
};

struct EQUIPMENT {
	uint16_t itemCode;
	uint16_t position;
	uint16_t enhance;
	EQUIPMENT() : itemCode(0), position(0), enhance(0) {}
};

struct CONSUMABLES {
	uint16_t itemCode;
	uint16_t position;
	uint16_t count;
	CONSUMABLES() : itemCode(0), position(0), count(0) {}
};

struct MATERIALS {
	uint16_t itemCode;
	uint16_t position;
	uint16_t count;
	MATERIALS() : itemCode(0), position(0), count(0) {}
};

//  ---------------------------- SYSTEM  ----------------------------

const int MAX_JWT_TOKEN_LEN = 256;

struct IM_WEB_REQUEST : PACKET_HEADER {
	char webToken[MAX_JWT_TOKEN_LEN + 1];
};

struct IM_WEB_RESPONSE : PACKET_HEADER {
	bool isSuccess;
};

struct SYNCRONIZE_LEVEL_REQUEST : PACKET_HEADER {
	uint16_t level;
	uint16_t userPk;
	unsigned int currentExp;
};

struct SYNCRONIZE_LOGOUT_REQUEST : PACKET_HEADER {
	uint16_t userPk;
};


//  ---------------------------- WEB  ----------------------------

const int MAX_USER_ID_LEN = 32;
const int MAX_INVEN_SIZE = 512;

struct USER_GAMESTART_REQUEST : PACKET_HEADER {
	char userId[MAX_USER_ID_LEN + 1];
};

struct USER_GAMESTART_RESPONSE : PACKET_HEADER {
	char webToken[MAX_JWT_TOKEN_LEN + 1];
};

struct USERINFO_REQUEST : PACKET_HEADER {
	char userId[MAX_USER_ID_LEN + 1];
};

struct USERINFO_RESPONSE : PACKET_HEADER {
	USERINFO UserInfo;
};

struct EQUIPMENT_REQUEST : PACKET_HEADER {

};

struct EQUIPMENT_RESPONSE : PACKET_HEADER {
	uint16_t eqCount;
	char Equipments[MAX_INVEN_SIZE+1];
};

struct CONSUMABLES_REQUEST : PACKET_HEADER {

};

struct CONSUMABLES_RESPONSE : PACKET_HEADER {
	uint16_t csCount;
	char Consumables[MAX_INVEN_SIZE+1];
};

struct MATERIALS_REQUEST : PACKET_HEADER {

};

struct MATERIALS_RESPONSE : PACKET_HEADER {
	uint16_t mtCount;
	char Materials[MAX_INVEN_SIZE+1];
};

//  ---------------------------- RAID  ----------------------------

struct RAID_HIT_REQUEST : PACKET_HEADER {
	uint16_t roomNum;
	uint16_t myNum;
	unsigned int damage;
};

struct RAID_HIT_RESPONSE : PACKET_HEADER {
	unsigned int yourScore;
	unsigned int currentMobHp;
};

struct RAID_END_REQUEST : PACKET_HEADER { // Server to USER
	unsigned int userScore;
	unsigned int teamScore;
};

struct RAID_END_RESPONSE : PACKET_HEADER { // User to Server (If Server Get This Packet, Return Room Number)

};

struct RAID_RANKING_REQUEST : PACKET_HEADER {
	unsigned int startRank;
};

struct RAID_RANKING_RESPONSE : PACKET_HEADER {
	std::vector<std::pair<std::string, unsigned int>> reqScore;
};

enum class WEBPACKET_ID : uint16_t {
	USER_LOGIN_REQUEST = 1,
	USER_LOGIN_RESPONSE = 2,
	USER_GAMESTART_REQUEST = 3,
	USER_GAMESTART_RESPONSE = 4,
	USERINFO_REQUEST = 5,
	USERINFO_RESPONSE = 6,
	EQUIPMENT_REQUEST = 7,
	EQUIPMENT_RESPONSE = 8,
	CONSUMABLES_REQUEST = 9,
	CONSUMABLES_RESPONSE = 10,
	MATERIALS_REQUEST = 11,
	MATERIALS_RESPONSE = 12,
};

enum class PACKET_ID : uint16_t {
	//SYSTEM
	USER_CONNECT_REQUEST = 1, // 유저는 2번으로 요청 
	USER_CONNECT_RESPONSE = 2,
	USER_LOGOUT_REQUEST = 3, // 유저는 3번으로 요청 
	IM_WEB_REQUEST = 4, // 유저는 1번으로 요청 
	IM_WEB_RESPONSE = 5,
	USER_FULL_REQUEST = 6, // SERVER TO USER
	WAITTING_NUMBER_REQUSET = 7, // SERVER TO USER

	// USER STATUS (21~)
	EXP_UP_REQUEST = 21,  // 유저는 4번으로 요청 
	EXP_UP_RESPONSE = 22,
	LEVEL_UP_REQUEST = 23,// SERVER TO USER
	LEVEL_UP_RESPONSE = 24,

	// INVENTORY (25~)
	ADD_ITEM_REQUEST = 25,  // 유저는 5번으로 요청 
	ADD_ITEM_RESPONSE = 26,
	DEL_ITEM_REQUEST = 27,  // 유저는 6번으로 요청 
	DEL_ITEM_RESPONSE = 28,
	MOD_ITEM_REQUEST = 29,  // 유저는 7번으로 요청 
	MOD_ITEM_RESPONSE = 30,
	MOV_ITEM_REQUEST = 31,  // 유저는 8번으로 요청 
	MOV_ITEM_RESPONSE = 32,

	// INVENTORY::EQUIPMENT 
	ADD_EQUIPMENT_REQUEST = 33,  // 유저는 9번으로 요청 
	ADD_EQUIPMENT_RESPONSE = 34,
	DEL_EQUIPMENT_REQUEST = 35,  // 유저는 10번으로 요청 
	DEL_EQUIPMENT_RESPONSE = 36,
	ENH_EQUIPMENT_REQUEST = 37,  // 유저는 11번으로 요청 
	ENH_EQUIPMENT_RESPONSE = 38,

	// RAID (45~)
	RAID_MATCHING_REQUEST = 45,  // 유저는 12번으로 요청 
	RAID_MATCHING_RESPONSE = 46,
	RAID_READY_REQUEST = 47,
	RAID_TEAMINFO_REQUEST = 48,  // 유저는 13번으로 요청 
	RAID_TEAMINFO_RESPONSE = 49,
	RAID_START_REQUEST = 50,
	RAID_HIT_REQUEST = 51,  // 유저는 14번으로 요청 
	RAID_HIT_RESPONSE = 52,
	RAID_END_REQUEST = 53,  // 유저는 15번으로 요청 , 서버로는 1번으로 요청
	RAID_END_RESPONSE = 54,
	RAID_RANKING_REQUEST = 55, // 유저는 16번으로 요청 
	RAID_RANKING_RESPONSE = 56,

	// WebServer Syncronizing Packet Id (101~)
	SYNCRONIZE_LEVEL_REQUEST = 101, // SERVER TO WEB SERVER
	SYNCRONIZE_LOGOUT_REQUEST = 102, // SERVER TO WEB SERVER
	SYNCRONIZE_DISCONNECT_REQUEST = 103, // SERVER TO WEB SERVER
};