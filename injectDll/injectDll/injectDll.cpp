// Injector.cpp
#include <Windows.h>
#include <iostream>

BOOL InjectDLL(DWORD dwProcessId, const char* dllPath)
{
    HANDLE hProcess = OpenProcess(PROCESS_ALL_ACCESS, FALSE, dwProcessId);
    if (!hProcess)
    {
        std::cerr << "Failed to open target process." << std::endl;
        return FALSE;
    }

    LPVOID pDllPath = VirtualAllocEx(hProcess, NULL, strlen(dllPath) + 1, MEM_COMMIT, PAGE_READWRITE);
    if (!pDllPath)
    {
        std::cerr << "Failed to allocate memory in target process." << std::endl;
        CloseHandle(hProcess);
        return FALSE;
    }

    if (!WriteProcessMemory(hProcess, pDllPath, (LPVOID)dllPath, strlen(dllPath) + 1, NULL))
    {
        std::cerr << "Failed to write DLL path to target process memory." << std::endl;
        VirtualFreeEx(hProcess, pDllPath, 0, MEM_RELEASE);
        CloseHandle(hProcess);
        return FALSE;
    }

    HMODULE hKernel32 = GetModuleHandle(L"Kernel32");
    FARPROC pLoadLibrary = GetProcAddress(hKernel32, "LoadLibraryA");

    HANDLE hThread = CreateRemoteThread(hProcess, NULL, 0, (LPTHREAD_START_ROUTINE)pLoadLibrary, pDllPath, 0, NULL);
    if (!hThread)
    {
        std::cerr << "Failed to create remote thread in target process." << std::endl;
        VirtualFreeEx(hProcess, pDllPath, 0, MEM_RELEASE);
        CloseHandle(hProcess);
        return FALSE;
    }

    WaitForSingleObject(hThread, INFINITE);

    VirtualFreeEx(hProcess, pDllPath, 0, MEM_RELEASE);
    CloseHandle(hThread);
    CloseHandle(hProcess);

    return TRUE;
}

int main()
{
    DWORD processId = 41524;
    const char* dllPath = "D:\\woker\\repository\\other\\windows\\inject-dll\\Dll1\\x64\\Debug\\Dll1.dll";

    if (InjectDLL(processId, dllPath))
    {
        std::cout << "DLL injected successfully." << std::endl;
    }
    else
    {
        std::cerr << "DLL injection failed." << std::endl;
    }

    return 0;
}
