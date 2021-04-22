#include "Serial.h"
#include <string>
#include <iostream>

using namespace std;

CSerial::CSerial():
	portName(), comm_timeouts()
{
	hSerial = nullptr;
}
CSerial::CSerial(CSerial&& r) noexcept
{
	hSerial = r.hSerial;
	portName = std::move(r.portName);
	comm_timeouts = r.comm_timeouts;
	r.hSerial = nullptr;
}
CSerial::~CSerial()
{
	close();
}

bool CSerial::open(uint8_t port, uint32_t band)
{
	if (isOpened())
		close();
	//portName = "COM";       //диапазон 0-9
	portName = L"\\\\.\\COM";  //диапазон 0-255-?
	portName += std::to_wstring(port);

	hSerial = CreateFileW(portName.c_str(), GENERIC_READ | GENERIC_WRITE, 0, 0, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);
	if (!hSerial)
		return false;

	//Connection setting up
	DCB dcbSerialParams = { 0 };
	dcbSerialParams.DCBlength = sizeof(dcbSerialParams);
	if (!GetCommState(hSerial, &dcbSerialParams))
	{
		std::cout << "getting state error\n";
		return false;
	}
	dcbSerialParams.BaudRate = band;
	dcbSerialParams.ByteSize = 8;
	dcbSerialParams.StopBits = ONESTOPBIT;
	dcbSerialParams.Parity = NOPARITY;
	dcbSerialParams.fDtrControl = DTR_CONTROL_ENABLE;

	if (!SetCommState(hSerial, &dcbSerialParams))
	{
		std::cout << "error setting serial port state\n";
		DeleteFileW(portName.c_str());
		return false;
	}
	GetCommTimeouts(hSerial, &comm_timeouts);
	return true;
}
bool CSerial::open(const wchar_t* port, uint32_t band)
{
	if (isOpened())
		close();
	portName = port;

	hSerial = CreateFileW(portName.c_str(), GENERIC_READ | GENERIC_WRITE, 0, 0, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);
	if (!hSerial)
		return false;

	//Connection setting up
	DCB dcbSerialParams = { 0 };
	dcbSerialParams.DCBlength = sizeof(dcbSerialParams);
	if (!GetCommState(hSerial, &dcbSerialParams))
	{
		std::cout << "getting state error\n";
		return false;
	}
	dcbSerialParams.BaudRate = band;
	dcbSerialParams.ByteSize = 8;
	dcbSerialParams.StopBits = ONESTOPBIT;
	dcbSerialParams.Parity = NOPARITY;
	dcbSerialParams.fDtrControl = DTR_CONTROL_ENABLE;

	if (!SetCommState(hSerial, &dcbSerialParams))
	{
		std::cout << "error setting serial port state\n";
		DeleteFileW(portName.c_str());
		return false;
	}
	GetCommTimeouts(hSerial, &comm_timeouts);
	return true;
}
bool CSerial::close()
{
	if (hSerial == nullptr)
		return false;

	//DeleteFileW(portName.c_str());
	CloseHandle(hSerial);
	hSerial = nullptr;
	return true;
}
bool CSerial::isOpened()
{
	return hSerial != 0;
}
const std::wstring& CSerial::getName() const
{
	return portName;
}

bool CSerial::readString(std::string& str, size_t max_length)
{
	str = "";

	DWORD iSize;
	char sReceivedChar = 0;
	while (true)
	{
		bool success = ReadFile(hSerial, &sReceivedChar, 1, &iSize, 0);  // получаем 1 байт
		if (success == false || iSize == 0)
			return false;
		if (iSize > 0)
		{
			if (sReceivedChar == '\0' || sReceivedChar == '\n'/* || sReceivedChar == 13*/)
				return true;
			else if (sReceivedChar != 13)
				str = str + sReceivedChar;
		}
		if (str.size() == max_length)
			return false;
	}
}
uint32_t CSerial::readData(void *data , uint32_t size)
{
	DWORD iSize;
	if (ReadFile(hSerial, data, size, &iSize, 0))
		return iSize;
	return 0;

	/*DWORD bytesRead{};
	DWORD errors = 0;
	COMSTAT status = {0};
	unsigned int toRead = 0;

	ClearCommError(hSerial, &errors, &status);

	if (status.cbInQue > 0)
	{
		if (status.cbInQue > size)
		{
			toRead = size;
		}
		else
		{
			toRead = status.cbInQue;
		}
	}

	if (ReadFile(hSerial, data, toRead, &bytesRead, NULL))
	{
		return bytesRead;
	}

	return 0;*/
}
uint32_t CSerial::writeData(const void* data, uint32_t size)
{
	DWORD dwBytesWritten;    // transmitted bytes amount
	if (WriteFile(hSerial, data, size, &dwBytesWritten, NULL))
		return dwBytesWritten;
	return 0;
}

bool CSerial::setReadTimeout(uint32_t timeout)
{
	comm_timeouts.ReadTotalTimeoutConstant = timeout;
	return SetCommTimeouts(hSerial, &comm_timeouts);
}
bool CSerial::setWriteTimeout(uint32_t timeout)
{
	comm_timeouts.WriteTotalTimeoutConstant = timeout;
	return SetCommTimeouts(hSerial, &comm_timeouts);
}
void CSerial::purgeRead()
{
	PurgeComm(hSerial, PURGE_RXCLEAR);
}

CSerial& CSerial::operator=(CSerial&& r) noexcept
{
	close();
	hSerial = r.hSerial;
	portName = std::move(r.portName);
	comm_timeouts = r.comm_timeouts;
	r.hSerial = nullptr;
	return *this;
}