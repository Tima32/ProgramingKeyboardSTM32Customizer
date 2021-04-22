#pragma once
#ifdef PCAPI_EXPORT
#define PCAPI_EXPORTIMPORT __declspec(dllexport)
#else
#define PCAPI_EXPORTIMPORT __declspec(dllimport)
#pragma comment(lib,"ProgrammingKeyboardAPI.lib")
#endif

// For Visual C++ compilers, we also need to turn off this annoying C4251 warning
#ifdef _MSC_VER
#pragma warning(disable: 4251)
#endif