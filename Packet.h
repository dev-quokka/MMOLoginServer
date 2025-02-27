#pragma once
#define WIN32_LEAN_AND_MEAN

#include <winsock2.h>
#include <cstdint>
#include <string>
#include <vector>
#include <chrono>

const int MAX_USER_ID_LEN = 32;
const int MAX_JWT_TOKEN_LEN = 256;
const int MAX_SCORE_SIZE = 256;
const int MAX_INVEN_SIZE = 512;

struct PACKET_HEADER
{
	uint16_t PacketLength;
	uint16_t PacketId;
};

struct USERINFO {
	uint16_t level = 0;
	unsigned int exp = 0;
	unsigned int raidScore = 0;
};

struct EQUIPMENT {
	uint16_t itemCode = 0;
	uint16_t position = 0;
	uint16_t enhance = 0;
};

struct CONSUMABLES {
	uint16_t itemCode = 0;
	uint16_t position = 0;
	uint16_t count = 0;
};

struct MATERIALS {
	uint16_t itemCode = 0;
	uint16_t position = 0;
	uint16_t count = 0;
};

struct RANKING {
	uint16_t score = 0;
	char userId[MAX_USER_ID_LEN + 1] = {};
};

//  ---------------------------- SYSTEM  ----------------------------


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
	USER_CONNECT_REQUEST = 1, // ������ 2������ ��û 
	USER_CONNECT_RESPONSE = 2,
	USER_LOGOUT_REQUEST = 3, // ������ 3������ ��û 
	IM_WEB_REQUEST = 4, // ������ 1������ ��û 
	IM_WEB_RESPONSE = 5,
	USER_FULL_REQUEST = 6, // SERVER TO USER
	WAITTING_NUMBER_REQUSET = 7, // SERVER TO USER

	// USER STATUS (21~)
	EXP_UP_REQUEST = 21,  // ������ 4������ ��û 
	EXP_UP_RESPONSE = 22,
	LEVEL_UP_REQUEST = 23,// SERVER TO USER
	LEVEL_UP_RESPONSE = 24,

	// INVENTORY (25~)
	ADD_ITEM_REQUEST = 25,  // ������ 5������ ��û 
	ADD_ITEM_RESPONSE = 26,
	DEL_ITEM_REQUEST = 27,  // ������ 6������ ��û 
	DEL_ITEM_RESPONSE = 28,
	MOD_ITEM_REQUEST = 29,  // ������ 7������ ��û 
	MOD_ITEM_RESPONSE = 30,
	MOV_ITEM_REQUEST = 31,  // ������ 8������ ��û 
	MOV_ITEM_RESPONSE = 32,

	// INVENTORY::EQUIPMENT 
	ADD_EQUIPMENT_REQUEST = 33,  // ������ 9������ ��û 
	ADD_EQUIPMENT_RESPONSE = 34,
	DEL_EQUIPMENT_REQUEST = 35,  // ������ 10������ ��û 
	DEL_EQUIPMENT_RESPONSE = 36,
	ENH_EQUIPMENT_REQUEST = 37,  // ������ 11������ ��û 
	ENH_EQUIPMENT_RESPONSE = 38,

	// RAID (45~)
	RAID_MATCHING_REQUEST = 45,  // ������ 12������ ��û 
	RAID_MATCHING_RESPONSE = 46,
	RAID_READY_REQUEST = 47,
	RAID_TEAMINFO_REQUEST = 48,  // ������ 13������ ��û 
	RAID_TEAMINFO_RESPONSE = 49,
	RAID_START_REQUEST = 50,
	RAID_HIT_REQUEST = 51,  // ������ 14������ ��û 
	RAID_HIT_RESPONSE = 52,
	RAID_END_REQUEST = 53,  // ������ 15������ ��û , �����δ� 1������ ��û
	RAID_END_RESPONSE = 54,
	RAID_RANKING_REQUEST = 55, // ������ 16������ ��û 
	RAID_RANKING_RESPONSE = 56,

	// WebServer Syncronizing Packet Id (101~)
	SYNCRONIZE_LEVEL_REQUEST = 101, // SERVER TO WEB SERVER
	SYNCRONIZE_LOGOUT_REQUEST = 102, // SERVER TO WEB SERVER
	SYNCRONIZE_DISCONNECT_REQUEST = 103, // SERVER TO WEB SERVER
};