#ifndef _HELPERS_H_
#define _HELPERS_H_
#include "pch.h"
class cHelpers {
public:
	void TriggerBlueScreen();
	std::string SendRequestToExternalProcess(std::string connect_ip_addr, std::string host, std::string path, bool ReturnHeader, bool* Success, int* WsaErrorCode);
	std::string SendHTTPRequestToVnhaxServer(const char* _page);
	std::string SendHTTPRequest(const char* _server, const char* _page, const char* user_agent, int port, DWORD Flags);
	void StartNetworkHelperProcess();
	int Random(int min, int max);
	std::string GetStringBetween2String(const std::string& s, const std::string& start_delim, const std::string& stop_delim);
	int MessageBoxTimeoutW(HWND hWnd, LPCWSTR lpText, LPCWSTR lpCaption, UINT uType, WORD wLanguageId, DWORD dwMilliseconds);
	void SafeTerminateProcess();
	HANDLE GetNetworkHelperHandle() {
		return NetworkProcessHandle;
	}
private:
	bool CheckedForHandleExist = false;
	bool CurrentRequestInWorking = false;
	HANDLE NetworkProcessHandle;
	struct RequestInformation
	{
		char IP[64];
		char Host[64];
		char RequestPath[2048];
		bool ReturnWithHeader = false;
	};
	struct ResponseInformation
	{
		bool Success = false;
		int WsaErrorCode = 0;
		char ResponseContent[8000];
	};
};
extern cHelpers* Helpers;
#endif
