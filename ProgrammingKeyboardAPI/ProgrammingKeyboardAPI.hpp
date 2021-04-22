#pragma once
#include <stdint.h>
#include <vector>
#include <string>
//#include <mutex>
#include "ImportExport.hpp"
#include "..\SerialLib\Serial.h"

namespace TPKAPI
{
	std::vector<std::wstring> PCAPI_EXPORTIMPORT GetKeyboardList();
	bool PCAPI_EXPORTIMPORT IsFree(const wchar_t* name);//функция не используется

    struct Data
    {
        enum Mode : uint8_t
        {
            Ignore =     0b0000, //0x0
            Standart =   0b0001, //0x1
            Command =    0b0010, //0x2
            Send =       0b0100, //0x4
            SwitchMode = 0b1000, //0x8 // Switch between standard and programmable mode.
        };
        struct CommandData
        {
            enum Action : uint8_t
            {
                Break,
                KeyboardPress,
                KeyboardRelease,
                ConsumearPress,
                ConsumearRelease,
                MousePress,
                MouseRelease
            };

            Action action;
            uint8_t key_code;
        };

        Mode mode[0xFF];
        uint16_t press_map[0xFF];
        uint16_t release_map[0xFF];
        CommandData commands[4096];
    };

	class PCAPI_EXPORTIMPORT ProgrammingKeyboard
	{
	public:
		ProgrammingKeyboard();
		~ProgrammingKeyboard();

		bool open(const wchar_t* com_name);
		void close();
		bool isOpen();
		const std::wstring& getName() const;

        void setKeyMode(uint8_t key, Data::Mode mode);
        Data::Mode getKeyMode(uint8_t key) const;

        void setPressMap(uint8_t key, uint16_t pos);
        void setReleaseMap(uint8_t key, uint16_t pos);

        void setCommands(Data::CommandData* cd, size_t size);

	private:
        std::string readNoDebugInfo() const;

		mutable CSerial serial;
        //std::mutex* mutex;
	};
}