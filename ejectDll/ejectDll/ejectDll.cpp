#include <iostream>
#include <Windows.h>
#include <tchar.h>
#include <TlHelp32.h>
#include <vector>

using namespace std;


BOOL EjectDll(DWORD dwPID, LPCTSTR szDllName) {
	BOOL bMore = FALSE, bFound = FALSE;
	HANDLE hSnapshot, hProcess, hThread;
	HMODULE hModule = NULL;
	MODULEENTRY32 me = { sizeof(me) };
	LPTHREAD_START_ROUTINE pThreadProc;
	hSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPMODULE, dwPID);

	bMore = Module32First(hSnapshot, &me);

	for (; bMore; bMore = Module32Next(hSnapshot, &me)) {
		if (!_tcsicmp(me.szModule, szDllName) || !_tcsicmp(me.szExePath, szDllName)) {
			bFound = TRUE;
			break;
		}
	}

	if (!bFound) {
		CloseHandle(hSnapshot);
		return FALSE;
	}

	if (!(hProcess = OpenProcess(PROCESS_ALL_ACCESS, FALSE, dwPID))) {
		wcout << L"OpenProcess(" << dwPID << L") failed!!! [" << GetLastError() << L"]" << endl;
		return FALSE;
	}

	hModule = GetModuleHandle(L"kernel32.dll");
	pThreadProc = reinterpret_cast<LPTHREAD_START_ROUTINE>(GetProcAddress(hModule, "FreeLibrary"));

	hThread = CreateRemoteThread(hProcess, NULL, NULL, pThreadProc, me.modBaseAddr, NULL, NULL);
	WaitForSingleObject(hThread, INFINITE);

	CloseHandle(hThread);
	CloseHandle(hProcess);
	CloseHandle(hSnapshot);

	return TRUE;
}
int _tmain(int argc, TCHAR* argv)
{
	DWORD dwPID = 41524;
	LPCTSTR dllName = L"Dll1.dll";
	if (EjectDll(dwPID, dllName)) {
		wcout << L"EjectDll(" << dwPID << L", " << dllName << L" success!!" << endl;
	}
	else {
		wcout << L"EjectDll(" << dwPID << L", " << dllName << L") failed!!" << endl;
	}
	

	return 1;

}