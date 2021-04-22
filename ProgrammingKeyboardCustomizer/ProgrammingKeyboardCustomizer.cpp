#include "MainWindow/MainWindow.hpp"
#include <ProgrammingKeyboardAPI\ProgrammingKeyboardAPI.hpp>
#include <stack>

//#include <windows.h>
#include <iostream>

using namespace std;


int wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPWSTR lpCmdLine, int nCmdShow)
{
    AllocConsole();
    FILE* file_c;
    freopen_s(&file_c, "conin$", "r", stdin);
    freopen_s(&file_c, "conout$", "w", stdout);
    freopen_s(&file_c, "conout$", "w", stderr);
    std::cout.clear();
    std::wcout.clear();
    std::cin.clear();
    cout << "Alloc console" << endl;
    //MessageBoxW(0, L"Начать?", L"Вопрос", 0);

    Sleep(1000);

    wcout << L"Programming keyboards list: ";
    auto list = TPKAPI::GetKeyboardList();
    for (auto& s : list)
    {
        wcout << s << L" ";
    }
    wcout << endl;

    MainWindow mw;
    mw.autoStep();

    return 0;
}
