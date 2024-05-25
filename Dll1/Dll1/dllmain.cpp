// dllmain.cpp : 定义 DLL 应用程序的入口点。
#include "pch.h"

#include <Windows.h>

extern "C" __declspec(dllexport) void ShowMessageBox(const wchar_t* msg)
{
    MessageBox(NULL, msg, L"Injected DLL", MB_OK);
}


BOOL APIENTRY DllMain( HMODULE hModule,
                       DWORD  ul_reason_for_call,
                       LPVOID lpReserved
                     )
{
    switch (ul_reason_for_call)
    {
    case DLL_PROCESS_ATTACH:
        ShowMessageBox(L"Attch Dll  Process");
        break;
    case DLL_THREAD_ATTACH:
        break;
    case DLL_THREAD_DETACH:
        break;
    case DLL_PROCESS_DETACH:
        ShowMessageBox(L"Exit Dll  Process");
        break;
    }
    return TRUE;
}

