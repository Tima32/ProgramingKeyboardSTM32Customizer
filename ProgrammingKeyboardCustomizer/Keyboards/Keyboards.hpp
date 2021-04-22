#pragma once
#include <vector>
#include <mutex>
#include "..\..\ProgrammingKeyboardAPI\ProgrammingKeyboardAPI.hpp"

namespace Keyboards
{
	extern std::vector<TPKAPI::ProgrammingKeyboard*> keyboards;
	extern std::vector<std::mutex*> keyboards_m;

	extern uint8_t key_mode[0xFF];
	extern std::vector<TPKAPI::Data::CommandData> key_press_com[0xFF];
	extern std::vector<TPKAPI::Data::CommandData> key_release_com[0xFF];
}