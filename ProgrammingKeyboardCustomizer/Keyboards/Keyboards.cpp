#include "Keyboards.hpp"

namespace Keyboards
{
	std::vector<TPKAPI::ProgrammingKeyboard*> keyboards;
	std::vector<std::mutex*> keyboards_m;

	uint8_t key_mode[0xFF];
	std::vector<TPKAPI::Data::CommandData> key_press_com[0xFF];
	std::vector<TPKAPI::Data::CommandData> key_release_com[0xFF];
}