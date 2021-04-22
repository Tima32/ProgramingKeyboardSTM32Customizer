#include <iostream>
#include "APIFunctions.h"
#include "..\main.h"

using std::cout;
using std::wcout;
using std::endl;

namespace API
{
	ReguestType GetType(const void* reguest)
	{
		const ReguestBaseRA* rtra = reinterpret_cast<const ReguestBaseRA*>(reguest);
		return rtra->reguest;
	};

	bool GetListF(void* buff, size_t buff_size, unsigned long* reply_size)
	{
		GetListA* gla = reinterpret_cast<GetListA*>(buff);
		gla->reguest = ReguestType::GetList;

		program::keyboards_m.lock();
		//расчет размера пакета
		*reply_size = sizeof(GetListA);
		for (size_t i = 0; i < program::keyboards.size(); i++)
		{
			*reply_size += (program::keyboards[i].name.size()+1)*2;
		}
		if (*reply_size > buff_size)
		{
			cout << "<API:GetListF>:Error: The buffer is " << buff_size << " bytes in size, and " << *reply_size << " bytes are required." << endl;
			program::keyboards_m.unlock();
			return false;
		}

		//копирование в буффер
		size_t num = 0;
		for (size_t i = 0; i < program::keyboards.size(); i++)
		{
			wcout << L"<GetListF>:Info: " << program::keyboards[i].name << endl;
			for (size_t i2 = 0; i2 < program::keyboards[i].name[i2] != L'\0'; i2++, num++)
				gla->keyboard_list[num] = program::keyboards[i].name[i2];
			gla->keyboard_list[num] = L'\0';
			num++;
		}
		gla->keyboard_list[num] = L'\0';

		program::keyboards_m.unlock();
		return true;
	}
	bool IsFreeF(void* buff, size_t buff_size, unsigned long* reply_size, const IsFreeR* reguerst)
	{
		IsFreeA* answer = reinterpret_cast<IsFreeA*>(buff);
		answer->reguest = ReguestType::IsFree;
		answer->is_free = true;
		*reply_size = sizeof(IsFreeA);

		//поиск
		program::keyboards_m.lock();
		for (auto& i : program::keyboards)
		{
			if (i.name == reguerst->name)
			{
				answer->is_free = !i.pipe;
				break;
			}
		}
		program::keyboards_m.unlock();
		return true;
	}
}