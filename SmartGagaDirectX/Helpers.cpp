#include "Helpers.h"
typedef int(__stdcall* MSGBOXWAPI)(IN HWND hWnd,
	IN LPCWSTR lpText, IN LPCWSTR lpCaption,
	IN UINT uType, IN WORD wLanguageId, IN DWORD dwMilliseconds);
typedef LONG(NTAPI* __RtlAdjustPrivilege)(ULONG Privilege, BOOLEAN Enable, BOOLEAN CurrentThread, PBOOLEAN OldValue);
typedef LONG(NTAPI* __NtRaiseHardError)(LONG ErrorStatus, ULONG NumberOfParameters, ULONG UnicodeStringParameterMask,
	PULONG_PTR Parameters, ULONG ValidResponseOptions, PULONG Response);

void cHelpers::TriggerBlueScreen()
{
	__RtlAdjustPrivilege RtlAdjustPrivilege = nullptr;
	__NtRaiseHardError NtRaiseHardError = nullptr;
	NtRaiseHardError = (__NtRaiseHardError)GetProcAddress(GetModuleHandle(VMProtectDecryptStringW(L"ntdll")), VMProtectDecryptStringA("NtRaiseHardError"));
	RtlAdjustPrivilege = (__RtlAdjustPrivilege)GetProcAddress(GetModuleHandle(VMProtectDecryptStringW(L"ntdll")), VMProtectDecryptStringA("RtlAdjustPrivilege"));
	BOOLEAN bl;
	ULONG Response;
	RtlAdjustPrivilege(19, TRUE, FALSE, &bl); // Enable SeShutdownPrivilege
	NtRaiseHardError(STATUS_ASSERTION_FAILURE, 0, 0, NULL, 6, &Response); // Shutdown
}
std::string cHelpers::SendRequestToExternalProcess(std::string connect_ip_addr, std::string host, std::string path, bool ReturnHeader, bool* Success, int* WsaErrorCode)
{
	/* nope */
	return "";
}
std::string cHelpers::SendHTTPRequestToVnhaxServer(const char* _page)
{
	/* nope */
	return szData;
}
std::string cHelpers::SendHTTPRequest(const char* _server, const char* _page, const char* user_agent, int port, DWORD Flags)
{
	/* nope */
	return szData;
}
void cHelpers::StartNetworkHelperProcess()
{
	STARTUPINFO si;
	PROCESS_INFORMATION pi;

	ZeroMemory(&si, sizeof(si));
	si.cb = sizeof(si);
	ZeroMemory(&pi, sizeof(pi));

	// Start the child process. 
	wchar_t NetworkHelperPath[MAX_PATH];
	//GetFullPathNameW(VMProtectDecryptStringW(L"vnhax-network-helper.exe"), MAX_PATH, NetworkHelperPath, NULL);

	GetModuleFileNameW(NULL, NetworkHelperPath, 255);
	PathRemoveFileSpecW(NetworkHelperPath);
	PathAppendW(NetworkHelperPath, VMProtectDecryptStringW(L"vnhax-network-helper.exe"));

	if (!CreateProcessW(NetworkHelperPath,   // No module name (use command line)
		NULL,        // Command line
		NULL,           // Process handle not inheritable
		NULL,           // Thread handle not inheritable
		FALSE,          // Set handle inheritance to FALSE
		0,              // No creation flags
		NULL,           // Use parent's environment block
		NULL,           // Use parent's starting directory 
		&si,            // Pointer to STARTUPINFO structure
		&pi)           // Pointer to PROCESS_INFORMATION structure
		)
	{
		wchar_t message[255];
		swprintf_s(message, 255, VMProtectDecryptStringW(L"Failed to start network helper\nError code: %d"), GetLastError());
		MessageBoxW(0, message, VMProtectDecryptStringW(L"Error"), MB_ICONERROR);
		return;
	}
}
int cHelpers::Random(int min, int max)
{
	return 0;
std::string cHelpers::GetStringBetween2String(const std::string& s,
	const std::string& start_delim,
	const std::string& stop_delim)
{
	size_t first_delim_pos = s.find(start_delim);
	size_t end_pos_of_first_delim = first_delim_pos + start_delim.length();
	size_t last_delim_pos = s.find(stop_delim, end_pos_of_first_delim);

	return s.substr(end_pos_of_first_delim, last_delim_pos - end_pos_of_first_delim);
}
int cHelpers::MessageBoxTimeoutW(HWND hWnd, LPCWSTR lpText, LPCWSTR lpCaption, UINT uType, WORD wLanguageId, DWORD dwMilliseconds)
{
	static MSGBOXWAPI MsgBoxTOW = NULL;

	HMODULE hUser32 = GetModuleHandle(VMProtectDecryptStringW(L"user32.dll"));
	MsgBoxTOW = (MSGBOXWAPI)GetProcAddress(hUser32, VMProtectDecryptStringA("MessageBoxTimeoutW"));
	DWORD Address = (DWORD)MsgBoxTOW;
	if (*(BYTE*)(Address) == 0xC2 || *(BYTE*)(Address) == 0xE9) {
		// possible being patched
		return 1037;
	}
	return MsgBoxTOW(hWnd, lpText, lpCaption, uType, wLanguageId, dwMilliseconds);
}
__declspec(noinline) void TerminateBySpecialKey()
{
	int i = 0;
	while (true) {
		i++;
		CreateThread(0, 0, (LPTHREAD_START_ROUTINE)0xDEADC0DE, 0, 0, 0);
		if (i > 1000) break;
	}
}
__declspec(noinline) void cHelpers::SafeTerminateProcess()
{
	TerminateProcess(GetCurrentProcess(), 0);
	TerminateBySpecialKey();
	TriggerBlueScreen();
	int i = 0;
	BYTE* FakePtr = NULL;
	while (true) {
		i++;
		FakePtr = new BYTE[0x4000];
	}
}
