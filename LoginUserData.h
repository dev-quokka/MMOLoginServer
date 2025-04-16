#pragma once
#include <cstdint>

const uint16_t MAX_INVEN_SIZE = 512;
const uint16_t INVENTORY_SIZE = 11; // 10���� +1�ؼ� 11���� �صα�

struct USERINFO {
	unsigned int raidScore = 0;
	unsigned int exp = 0;
	uint16_t level = 0;
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