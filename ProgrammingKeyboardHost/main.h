#pragma once
#include <vector>
#include <string>
#include <mutex>
#include <thread>
#include "..\SerialLib\Serial.h"

namespace program
{
	struct PKeyboard
	{
		CSerial port;
		std::wstring name;
		HANDLE pipe = 0;
	};
	void UpdateListSerialPorts();
	void SerialPortChange(WPARAM wParam, LPARAM lParam);
	bool IsProgrammingKeyboard(CSerial& serial);
	extern std::vector<PKeyboard> keyboards;
	extern std::mutex keyboards_m;

	void InitializationEventWindow();
	INT_PTR CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
	extern HWND event_window_hwnd;

	void PipeLoop();
	DWORD WINAPI PipeClientThread(LPVOID lpvParam);
	void PipeFreePKeyboard(HANDLE pipe);
	constexpr size_t pipe_buff_size = 1024;
	extern std::thread* pipe_loop_t;
}