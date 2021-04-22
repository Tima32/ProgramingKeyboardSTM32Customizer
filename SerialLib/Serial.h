#include <Windows.h>
#include <string>

class CSerial
{

public:
	CSerial();
	CSerial(const CSerial&) = delete;
	CSerial(CSerial&& r) noexcept;
	~CSerial();

	bool open(uint8_t port, uint32_t band = 115200);
	bool open(const wchar_t* port, uint32_t band = 115200);
	bool close();
	bool isOpened();
	const std::wstring& getName() const;

	bool readString(std::string& str, size_t max_length = -1);
	uint32_t readData(void* data, uint32_t size);
	uint32_t writeData(const void* data, uint32_t size);

	bool setReadTimeout(uint32_t timeout);
	bool setWriteTimeout(uint32_t timeout);
	void purgeRead();

	CSerial& operator=(const CSerial&) = delete;
	CSerial& operator=(CSerial&& r) noexcept;

private:

	HANDLE hSerial;
	std::wstring portName;
	COMMTIMEOUTS comm_timeouts;
};