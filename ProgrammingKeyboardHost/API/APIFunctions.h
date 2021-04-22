#pragma once
#include <stdint.h>
#include "APIStruct.hpp"

namespace API
{
	ReguestType GetType(const void* reguest);

	bool GetListF(void* buff, size_t buff_size, unsigned long* reply_size);
	bool IsFreeF(void* buff, size_t buff_size, unsigned long* reply_size, const IsFreeR* reguerst);
}