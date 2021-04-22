#pragma once
#include <stdint.h>

namespace API
{
	enum class ReguestType : uint16_t
	{
		Error,            //произошла ошибка

		GetList,          //список клавиатур
		IsFree,           //свободность клавиатуры

		Connect,          //подключиться
		Disconnect,       //отключиться

		KeyPressNumber,   //нажать клавишу
		KeyReleaseNumber, //нажать клавишу
		KeyClickNumber,   //кликнуть клавишой
	};


	//у всех структур на первом месте находится тип запроса
	struct ReguestBaseRA
	{
		uint64_t size;
		ReguestType reguest;
	};


	//GetList
	struct GetListR
	{
		uint64_t size = sizeof(GetListR);
		ReguestType reguest = ReguestType::GetList;
	};
	struct GetListA
	{
		uint64_t size;
		ReguestType reguest;
		wchar_t keyboard_list[1]; //COM1\0COM2\0\0
		//...
	};

	//IsFree
	struct IsFreeR
	{
		uint64_t size;
		ReguestType reguest = ReguestType::IsFree;
		wchar_t name[1];
	};
	struct IsFreeA
	{
		uint64_t size = sizeof(IsFreeA);
		ReguestType reguest;
		bool is_free;
	};
}