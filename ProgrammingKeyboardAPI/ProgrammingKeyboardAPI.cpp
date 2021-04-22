#define _CRT_SECURE_NO_WARNINGS

#include "ProgrammingKeyboardAPI.hpp"
#include "..\ProgrammingKeyboardHost\API\APIStruct.hpp"
#include <sstream>

//#include <windows.h>
//#include <stdio.h>
#include <iostream>

using namespace std;

namespace TPKAPI
{
	std::vector<std::wstring> GetKeyboardList()
	{
        HANDLE hPipe;
        const size_t buff_size = 4096;
        uint8_t  buff[4096];
        BOOL   fSuccess = FALSE;
        DWORD  cbRead, cbToWrite, cbWritten, dwMode;
        const wchar_t* lpszPipename = L"\\\\.\\pipe\\TimProgrammingKeyboardAPI";
        vector<wstring> keyboard_list;

        // Try to open a named pipe; wait for it, if necessary.
        while (1)
        {
            hPipe = CreateFile(
                lpszPipename,   // pipe name 
                GENERIC_READ |  // read and write access 
                GENERIC_WRITE,
                0,              // no sharing 
                NULL,           // default security attributes
                OPEN_EXISTING,  // opens existing pipe 
                0,              // default attributes 
                NULL);          // no template file 


            if (hPipe != INVALID_HANDLE_VALUE)
                break;

            // Exit if an error other than ERROR_PIPE_BUSY occurs. 
            if (GetLastError() != ERROR_PIPE_BUSY)
            {
                wprintf(L"<TPKAPI:GetKeyboardList>:Error: Could not open pipe. GLE=%d\n", GetLastError());
                return keyboard_list;
            }

            // All pipe instances are busy, so wait for 20 seconds. 
            if (!WaitNamedPipe(lpszPipename, 20000))
            {
                printf("<TPKAPI:GetKeyboardList>:Error: Could not open pipe: 20 second wait timed out.");
                return keyboard_list;
            }
        }

        // The pipe connected; change to message-read mode.  
        dwMode = PIPE_READMODE_MESSAGE;
        fSuccess = SetNamedPipeHandleState(
            hPipe,    // pipe handle 
            &dwMode,  // new pipe mode 
            NULL,     // don't set maximum bytes 
            NULL);    // don't set maximum time 
        if (!fSuccess)
        {
            wprintf(L"<TPKAPI:GetKeyboardList>:Error: SetNamedPipeHandleState failed. GLE=%d\n", GetLastError());
            return keyboard_list;
        }

        API::GetListR glr;

        fSuccess = WriteFile(
            hPipe,                  // pipe handle 
            &glr,                   // message 
            sizeof(glr),            // message length 
            &cbWritten,             // bytes written 
            NULL);                  // not overlapped 

        if (!fSuccess)
        {
            wprintf(L"<TPKAPI:GetKeyboardList>:Error: WriteFile to pipe failed. GLE=%d\n", GetLastError());
            return keyboard_list;
        }

        // Read from the pipe.
        fSuccess = ReadFile(
            hPipe,    // pipe handle 
            buff,     // buffer to receive reply 
            buff_size,// size of buffer 
            &cbRead,  // number of bytes read 
            NULL);    // not overlapped 

        API::GetListA* gla = (API::GetListA*)buff;
        fSuccess = fSuccess && gla->reguest == API::ReguestType::GetList;
        CloseHandle(hPipe);

        if (!fSuccess)
        {
            wprintf(L"<TPKAPI:GetKeyboardList>:Error: ReadFile from pipe failed. GLE=%d\n", GetLastError());
            return keyboard_list;
        }

        //извлечение строк
        wstring str;
        for (size_t i = 0; gla->keyboard_list[i] != L'\0'; i++)
        {
            str = L"";
            for (; gla->keyboard_list[i] != L'\0'; i++)
            {
                str += gla->keyboard_list[i];
            }
            keyboard_list.push_back(str);
        }

        return keyboard_list;
	}
    bool IsFree(const wchar_t* name)
    {
        HANDLE hPipe;
        const size_t buff_size = 4096;
        uint8_t  buff[4096];
        BOOL   fSuccess = FALSE;
        DWORD  cbRead, cbToWrite, cbWritten, dwMode;
        const wchar_t* lpszPipename = L"\\\\.\\pipe\\TimProgrammingKeyboardAPI";
        bool ret = false;

        // Try to open a named pipe; wait for it, if necessary.
        while (1)
        {
            hPipe = CreateFile(
                lpszPipename,   // pipe name 
                GENERIC_READ |  // read and write access 
                GENERIC_WRITE,
                0,              // no sharing 
                NULL,           // default security attributes
                OPEN_EXISTING,  // opens existing pipe 
                0,              // default attributes 
                NULL);          // no template file 


            if (hPipe != INVALID_HANDLE_VALUE)
                break;

            // Exit if an error other than ERROR_PIPE_BUSY occurs. 
            if (GetLastError() != ERROR_PIPE_BUSY)
            {
                wprintf(L"<TPKAPI:GetKeyboardList>:Error: Could not open pipe. GLE=%d\n", GetLastError());
                return ret;
            }

            // All pipe instances are busy, so wait for 20 seconds. 
            if (!WaitNamedPipe(lpszPipename, 20000))
            {
                printf("<TPKAPI:GetKeyboardList>:Error: Could not open pipe: 20 second wait timed out.");
                return ret;
            }
        }

        // The pipe connected; change to message-read mode.  
        dwMode = PIPE_READMODE_MESSAGE;
        fSuccess = SetNamedPipeHandleState(
            hPipe,    // pipe handle 
            &dwMode,  // new pipe mode 
            NULL,     // don't set maximum bytes 
            NULL);    // don't set maximum time 
        if (!fSuccess)
        {
            wprintf(L"<TPKAPI:GetKeyboardList>:Error: SetNamedPipeHandleState failed. GLE=%d\n", GetLastError());
            return ret;
        }

        API::IsFreeR* reguest = reinterpret_cast<API::IsFreeR*>(buff);
        reguest->reguest = API::ReguestType::IsFree;
        wcscpy_s(reguest->name, 6, name);

        fSuccess = WriteFile(
            hPipe,                  // pipe handle 
            buff,                   // message 
            256,                    // message length 
            &cbWritten,             // bytes written 
            NULL);                  // not overlapped 

        if (!fSuccess)
        {
            wprintf(L"<TPKAPI:GetKeyboardList>:Error: WriteFile to pipe failed. GLE=%d\n", GetLastError());
            return ret;
        }

        // Read from the pipe.
        fSuccess = ReadFile(
            hPipe,    // pipe handle 
            buff,     // buffer to receive reply 
            buff_size,// size of buffer 
            &cbRead,  // number of bytes read 
            NULL);    // not overlapped 

        API::IsFreeA* answer = (API::IsFreeA*)buff;
        fSuccess = fSuccess && answer->reguest == API::ReguestType::IsFree;
        CloseHandle(hPipe);

        if (!fSuccess)
        {
            wprintf(L"<TPKAPI:GetKeyboardList>:Error: ReadFile from pipe failed. GLE=%d\n", GetLastError());
            return ret;
        }

        //извлечение результата
        ret = answer->is_free;

        return ret;
    }

    TPKAPI::ProgrammingKeyboard::ProgrammingKeyboard()
    {}
    TPKAPI::ProgrammingKeyboard::~ProgrammingKeyboard()
    {}

    bool TPKAPI::ProgrammingKeyboard::open(const wchar_t* com_name)
    {
        if (!serial.open(com_name, 2000000))
        {
            wcout << L"<TPKAPI:ProgrammingKeyboard:open>:Error: Well managed to connect to " << com_name << endl;
            return false;
        }

        serial.setReadTimeout(1000);
        serial.writeData("gdn", 4);
        string read_string;
        if (!serial.readString(read_string, 256) && read_string != "TimProgrammingKeyboard")
        {
            wcout << L"<TPKAPI:ProgrammingKeyboard:open>:Error: " << com_name << L" non-programmable keyboard." << endl;
            serial.close();
            return false;
        }
        
        return true;
    }
    void TPKAPI::ProgrammingKeyboard::close()
    {
        serial.close();
    }

    bool TPKAPI::ProgrammingKeyboard::isOpen()
    {
        return serial.isOpened();
    }
    const std::wstring& ProgrammingKeyboard::getName() const
    {
        return serial.getName();
    }

    void ProgrammingKeyboard::setKeyMode(uint8_t key, Data::Mode mode)
    {
        stringstream s;
        s << "ksm " << std::hex << (uint16_t)key << " " << (uint16_t)mode;
        string str = s.str().c_str();

        serial.purgeRead();
        serial.writeData(str.c_str(), str.size()+1);
    }
    Data::Mode ProgrammingKeyboard::getKeyMode(uint8_t key) const
    {
        stringstream s;
        s << "kgm " << std::hex << (int)key;
        string str = s.str().c_str();

        serial.purgeRead();
        serial.writeData(str.c_str(), str.size() + 1);

        str = readNoDebugInfo();
        if (str.find("kgm") != 0 || str.size() < 5)
        {
            cout << "<TPKAPI:ProgrammingKeyboard:getKeyMode>:Error: Bad returned data" << endl;
        }
        str = str.substr(4);
        return Data::Mode(stoul(str, nullptr, 16));
    }

    void ProgrammingKeyboard::setPressMap(uint8_t key, uint16_t pos)
    {
        stringstream s;
        s << "kspm " << std::hex << (uint16_t)key << " " << (uint16_t)pos;
        string str = s.str().c_str();

        serial.purgeRead();
        serial.writeData(str.c_str(), str.size() + 1);
    }
    void ProgrammingKeyboard::setReleaseMap(uint8_t key, uint16_t pos)
    {
        stringstream s;
        s << "ksrm " << std::hex << (uint16_t)key << " " << (uint16_t)pos;
        string str = s.str().c_str();

        serial.purgeRead();
        serial.writeData(str.c_str(), str.size() + 1);
    }

    void ProgrammingKeyboard::setCommands(Data::CommandData* cd, size_t size)
    {
        serial.purgeRead();
        serial.writeData("ksc", 4);

        const uint16_t* arr = reinterpret_cast<const uint16_t*>(cd);
        stringstream s;
        s << std::hex;
        string str;
        for (size_t i = 0; i < size; i++)
        {
            s.str("");
            s << arr[i];
            str = s.str();
            cout << str << " ";
            serial.writeData(str.c_str(), str.size() + 1);
        }
        serial.writeData("ksce", 5);
    }

    //private
    std::string ProgrammingKeyboard::readNoDebugInfo() const
    {
        string s;
        do
        {
            if (!serial.readString(s, 256))
            {
                cout << "<TPKAPI:ProgrammingKeyboard:readNoDebugInfo>:Error: Error read" << endl;
                return s;
            }
        } while (s[0] == '<');
        return s;
    }
}