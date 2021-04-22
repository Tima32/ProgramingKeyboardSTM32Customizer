#include <Windows.h>
#include <Dbt.h>
#include <iostream>
#include <conio.h>
#include <strsafe.h>
#include "main.h"
#include "API/APIFunctions.h"

using namespace std;

namespace program
{
	void UpdateListSerialPorts()
	{
		static wchar_t szDevices[65535];
		DWORD dwChars = QueryDosDeviceW(nullptr, szDevices, 65535);
		if (dwChars)
		{
			int i = 0;
			while (true)
			{
				const TCHAR* pszCurrentDevice = &szDevices[i]; // Получаем текущее имя устройства
				size_t nLen = wcslen(pszCurrentDevice);// Если похоже на "COMX" выводим на экран

				if (nLen > 3 && wcsncmp(pszCurrentDevice, L"COM", 3) == 0)
				{
					wprintf(L"<UpdateListSerialPorts>:Info: Find port %s\n", pszCurrentDevice);

					keyboards_m.lock();
					for (auto i = keyboards.begin(); i != keyboards.end(); ++i)
					{
						if (i->name == pszCurrentDevice)
							goto EndBushToSerialPortsList;//пропускаем этап проверки и добавления в список
					}
					//проверка и добавление в список
					{
						wstring name = L"\\\\.\\";
						name += pszCurrentDevice;
						CSerial serial;
						serial.open(name.c_str());
						serial.setReadTimeout(100);
						if (!IsProgrammingKeyboard(serial))
							goto EndBushToSerialPortsList;//не добавляемы
						serial.close();
						keyboards.emplace_back();
						keyboards[keyboards.size() - 1].port = move(serial);
						keyboards[keyboards.size() - 1].name = pszCurrentDevice;
					}

				EndBushToSerialPortsList:
					keyboards_m.unlock();
				}

				// Переходим к следующему символу терминатору
				while (szDevices[i] != L'\0')
					i++;
				i++;// Перескакиваем на следующую строку

				// Список завершается двойным симмволом терминатором, так что если символ
				// NULL, мы дошли до конца
				if (szDevices[i] == L'\0')
					break;
			}
		}
	}
	void SerialPortChange(WPARAM wParam, LPARAM lParam)
	{
		//подключено новое устройство
		if (wParam == DBT_DEVICEARRIVAL)
		{
			DEV_BROADCAST_HDR* dev = (DEV_BROADCAST_HDR*)lParam;
			if (dev->dbch_devicetype == DBT_DEVTYP_PORT) //подключен serial порт
			{
				DEV_BROADCAST_PORT* serial_s = (DEV_BROADCAST_PORT*)dev;
				wcout << L"<SerialPortChange>:Info: Serialport connected: " << serial_s->dbcp_name << endl;
				keyboards_m.lock();
				wstring name = L"\\\\.\\";
				name += serial_s->dbcp_name;
				CSerial serial;
				serial.open(name.c_str());
				serial.setReadTimeout(100);
				if (!IsProgrammingKeyboard(serial))
				{
					keyboards_m.unlock();
					return;//не добавляемы
				}
				serial.close();
				keyboards.emplace_back();
				keyboards[keyboards.size() - 1].port = move(serial);
				keyboards[keyboards.size() - 1].name = serial_s->dbcp_name;
				keyboards_m.unlock();
			}
		}
		//устройствоотключено
		else if (wParam == DBT_DEVICEREMOVECOMPLETE)
		{
			DEV_BROADCAST_HDR* dev = (DEV_BROADCAST_HDR*)lParam;
			if (dev->dbch_devicetype == DBT_DEVTYP_PORT) //отключен serial порт
			{
				DEV_BROADCAST_PORT* serial_s = (DEV_BROADCAST_PORT*)dev;
				wcout << L"<SerialPortChange>:Info: Serialport disconnected: " << serial_s->dbcp_name << endl;
				keyboards_m.lock();
				for (auto i = keyboards.begin(); i != keyboards.end(); ++i)
				{
					if (i->name == serial_s->dbcp_name)
					{
						keyboards.erase(i);
						break;
					}
				}
				keyboards_m.unlock();
			}
		}
	}
	bool IsProgrammingKeyboard(CSerial& serial)
	{
		serial.setReadTimeout(100);
		serial.writeData("gdn", 4);

		string s;
		while (1)
		{
			auto bc = serial.readString(s, 100);
			if (s.size() != 0 && s[0] != '<')
			{
				if (s == "TimProgrammingKeyboard")
				{
					cout << "<IsProgrammingKeyboard>:Info: This is a programming keyboard." << endl;
					return true;
				}
				goto IsNotPK;
			}
			else if (!bc)
				goto IsNotPK;
		}
		IsNotPK:
		cout << "<IsProgrammingKeyboard>:Info: This is not a programming keyboard." << endl;
		return false;
	}
	std::vector<PKeyboard> keyboards;
	std::mutex keyboards_m;

	void InitializationEventWindow()
	{
		static const wchar_t* window_class_name = L"EventWindow";
		HMODULE module;
		if (!GetModuleHandleExW(GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS | GET_MODULE_HANDLE_EX_FLAG_UNCHANGED_REFCOUNT, (wchar_t*)InitializationEventWindow, &module))
		{
			cout << "<main>:Error: GetModuleExW error" << endl;
			exit(-1);
		}

		WNDCLASS wc = { };

		wc.lpfnWndProc = program::WindowProc;
		wc.hInstance = module;
		wc.lpszClassName = window_class_name;

		RegisterClassW(&wc);
		program::event_window_hwnd = CreateWindowExW(
			0,
			window_class_name,
			window_class_name,
			WS_DISABLED,            // Window style

			// Size and position
			CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT,

			NULL,       // Parent window    
			NULL,       // Menu
			module,     // Instance handle
			NULL        // Additional application data
		);
		if (program::event_window_hwnd == nullptr)
		{
			cout << "<InitializationEventWindow>:Error: CreateWindowExW error" << endl;
			exit(-1);
		}
		//ShowWindow(program::event_window_hwnd, SW_SHOW);
	}
	INT_PTR CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
	{
		switch (uMsg)
		{
		case (WM_DEVICECHANGE):
			SerialPortChange(wParam, lParam);
			break;
		case WM_DESTROY:
			PostQuitMessage(0);
			return 0;

		case WM_PAINT:
		{
			PAINTSTRUCT ps;
			HDC hdc = BeginPaint(hwnd, &ps);
			FillRect(hdc, &ps.rcPaint, (HBRUSH)(COLOR_WINDOW + 1));
			EndPaint(hwnd, &ps);
		}
		return 0;
		}
		return DefWindowProcW(hwnd, uMsg, wParam, lParam);
	};
	HWND event_window_hwnd;

	void PipeLoop()
	{
		const wchar_t* pipe_name = L"\\\\.\\pipe\\TimProgrammingKeyboardAPI";

		HANDLE pipe_h;
		bool pipe_connect_f;
		//constexpr size_t pipe_buff_size = 1024;
		HANDLE  thread_id = 0;

		for (;;)
		{
			wcout << L"<PipeLoop>:Info: Main thread awaiting client connection on " << pipe_name << endl;
			pipe_h = CreateNamedPipeW(
				pipe_name,                // pipe name 
				PIPE_ACCESS_DUPLEX,       // read/write access 
				PIPE_TYPE_MESSAGE |       // message type pipe 
				PIPE_READMODE_MESSAGE |   // message-read mode 
				PIPE_WAIT,                // blocking mode 
				PIPE_UNLIMITED_INSTANCES, // max. instances  
				pipe_buff_size,           // output buffer size 
				pipe_buff_size,           // input buffer size 
				0,                        // client time-out 
				NULL);                    // default security attribute 

			if (pipe_h == INVALID_HANDLE_VALUE)
			{
				cout << "<PipeLoop>:Error: CreateNamedPipe failed, GLE=" << GetLastError() << endl;
				exit(-1);
			}

			// Wait for the client to connect; if it succeeds, 
			// the function returns a nonzero value. If the function
			// returns zero, GetLastError returns ERROR_PIPE_CONNECTED. 

			pipe_connect_f = ConnectNamedPipe(pipe_h, NULL) ?
				TRUE : (GetLastError() == ERROR_PIPE_CONNECTED);

			if (pipe_connect_f)
			{
				printf("Client connected, creating a processing thread.\n");

				// Create a thread for this client. 
				thread_id = CreateThread(
					NULL,              // no security attribute 
					0,                 // default stack size 
					PipeClientThread,  // thread proc
					(LPVOID)pipe_h,    // thread parameter 
					0,                 // not suspended 
					nullptr);          // returns thread ID 

				if (thread_id == NULL)
				{
					cout << "<PipeLoop>:Error: CreateThread failed, GLE=" << GetLastError() << endl;
					exit(-1);
				}
				else CloseHandle(thread_id);
			}
			else
				// The client could not connect, so close the pipe. 
				CloseHandle(thread_id);
		}
	}
	DWORD WINAPI PipeClientThread(LPVOID lpvParam)
	{
		//HANDLE hHeap = GetProcessHeap();
		//TCHAR* pchRequest = (TCHAR*)HeapAlloc(hHeap, 0, pipe_buff_size * sizeof(TCHAR));
		//TCHAR* pchReply = (TCHAR*)HeapAlloc(hHeap, 0, pipe_buff_size * sizeof(TCHAR));
		uint8_t* pchRequest = new uint8_t[pipe_buff_size];
		uint8_t* pchReply = new uint8_t[pipe_buff_size];

		DWORD cbBytesRead = 0, cbReplyBytes = 0, cbWritten = 0;
		BOOL fSuccess = FALSE;
		HANDLE hPipe = NULL;

		// Do some extra error checking since the app will keep running even if this
		// thread fails.

		if (lpvParam == NULL)
		{
			printf("<PipeClientThread>:ERROR: Pipe Server Failure:\n");
			printf("   InstanceThread got an unexpected NULL value in lpvParam.\n");
			printf("   InstanceThread exitting.\n");
			delete[] pchReply;
			delete[] pchRequest;
			return (DWORD)-1;
		}

		if (pchRequest == NULL)
		{
			printf("<PipeClientThread>:ERROR: Pipe Server Failure:\n");
			printf("   InstanceThread got an unexpected NULL heap allocation.\n");
			printf("   InstanceThread exitting.\n");
			if (pchReply != NULL) delete[] pchReply;
			return (DWORD)-1;
		}

		if (pchReply == NULL)
		{
			printf("<PipeClientThread>:ERROR: Pipe Server Failure:\n");
			printf("   InstanceThread got an unexpected NULL heap allocation.\n");
			printf("   InstanceThread exitting.\n");
			if (pchRequest != NULL) delete[] pchRequest;
			return (DWORD)-1;
		}

		// Print verbose messages. In production code, this should be for debugging only.
		printf("<PipeClientThread>:Info: InstanceThread created, receiving and processing messages.\n");

		// The thread's parameter is a handle to a pipe object instance. 
		hPipe = (HANDLE)lpvParam;

		// Loop until done reading
		while (1)
		{
			// Read client requests from the pipe. This simplistic code only allows messages
			// up to BUFSIZE characters in length.
			fSuccess = ReadFile(
				hPipe,        // handle to pipe 
				pchRequest,    // buffer to receive data 
				pipe_buff_size, // size of buffer 
				&cbBytesRead, // number of bytes read 
				NULL);        // not overlapped I/O 

			if (!fSuccess || cbBytesRead == 0)
			{
				if (GetLastError() == ERROR_BROKEN_PIPE)
				{
					wprintf(L"<PipeClientThread>:Info: client disconnected.\n");
				}
				else
				{
					wprintf(L"<PipeClientThread>:Error: InstanceThread ReadFile failed, GLE=%d.\n", GetLastError());
				}
				break;
			}

			//Составление ответа на запрос
			auto type = API::GetType(pchRequest);
			if (type == API::ReguestType::GetList)
			{
				API::GetListF(pchReply, pipe_buff_size, &cbReplyBytes);
			}
			else if (type == API::ReguestType::IsFree)
			{
				API::IsFreeF(pchReply, pipe_buff_size, &cbReplyBytes, (API::IsFreeR*)pchRequest);
			}

			// Write the reply to the pipe. 
			fSuccess = WriteFile(
				hPipe,        // handle to pipe 
				pchReply,     // buffer to write from 
				cbReplyBytes, // number of bytes to write 
				&cbWritten,   // number of bytes written 
				NULL);        // not overlapped I/O 

			if (!fSuccess || cbReplyBytes != cbWritten)
			{
				wprintf(L"InstanceThread WriteFile failed, GLE=%d.\n", GetLastError());
				break;
			}
		}

		// Flush the pipe to allow the client to read the pipe's contents 
		// before disconnecting. Then disconnect the pipe, and close the 
		// handle to this pipe instance. 

		PipeFreePKeyboard(hPipe);//освобождение клавиатуы
		FlushFileBuffers(hPipe);
		DisconnectNamedPipe(hPipe);
		CloseHandle(hPipe);

		delete[] pchReply;
		delete[] pchRequest;
		printf("InstanceThread exiting.\n");
		return 1;
	}
	void PipeFreePKeyboard(HANDLE pipe)
	{
		keyboards_m.lock();
		for (auto& i : keyboards)
		{
			if (i.pipe == pipe)
			{
				i.pipe = 0;
				break;
			}
		}
		keyboards_m.unlock();
	}
	std::thread* pipe_loop_t;
}
void icon()
{
	HICON hicon = NULL;
	//HRESULT limr = LoadIconMetric(NULL, IDI_INFORMATION, LIM_LARGE, &hicon);

	NOTIFYICONDATAW nt = { 0 };
	nt.cbSize = sizeof(NOTIFYICONDATAW);
	nt.hWnd = program::event_window_hwnd;
	nt.uID = 0;

	nt.uFlags = NIF_MESSAGE | /*NIF_ICON |*/ NIF_TIP | NIF_INFO;
	nt.dwInfoFlags = NIIF_INFO;
	nt.uCallbackMessage = 1;
	nt.hIcon = hicon;
	nt.hBalloonIcon = hicon;


	cout << Shell_NotifyIcon(NIM_ADD, &nt) << endl;
}
int main()
{
	program::InitializationEventWindow();//инициализация окна принемающего события
	program::UpdateListSerialPorts();//Получаем все COM порты в системе

	//void program::PipeLoop();
	program::pipe_loop_t = new thread(program::PipeLoop);

	// Run the message loop.
	MSG msg = { };
	while (GetMessage(&msg, NULL, 0, 0))
	{
		TranslateMessage(&msg);
		DispatchMessageW(&msg);
	}

	return 0;
}