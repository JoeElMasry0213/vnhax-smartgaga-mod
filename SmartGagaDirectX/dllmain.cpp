// dllmain.cpp : Defines the entry point for the DLL application.
#include "pch.h"
// constructor
cHelpers* Helpers = new cHelpers();
//cLogin* Login = new cLogin();
cKeymap* Keymap = new cKeymap();
cGameThreadSuspender* GameThreadSuspender = new cGameThreadSuspender();
cHooks* Hooks = new cHooks();
//cUserCheck* UserCheck = new cUserCheck();


DWORD CALLBACK MainThread(LPVOID lpParameter)
{
	HWND hEmulatorWindow = NULL;
	while (hEmulatorWindow == NULL) {
		hEmulatorWindow = FindWindowW(VMProtectDecryptStringW(L"TitanRenderWindowClass"), VMProtectDecryptStringW(L"SmartGaGa RenderWindow"));
		//hEmulatorWindow = FindWindowW(VMProtectDecryptStringW(L"VnHaxRenderWindowClass"), VMProtectDecryptStringW(L"VnHaxRenderWindow"));
		Sleep(500);
	}
	Hooks->Init();


	//if (!Login->CheckServerVersion()) {
	//	Helpers->SafeTerminateProcess();
	//	return 0;
	//}

	Keymap->Init(hEmulatorWindow);
	//HWND hBorderWindow = FindWindowW(VMProtectDecryptStringW(L"VnHaxMainWindowClass0"), VMProtectDecryptStringW(L"SmartGaga"));
	//SetWindowTextW(hBorderWindow, VMProtectDecryptStringW(L"Vn-Hax Smartgaga Mod v" CURRENT_VERSION));

	GameThreadSuspender->StartSuspender();
	return 0;
}

BOOL APIENTRY DllMain( HMODULE hModule,
                       DWORD  ul_reason_for_call,
                       LPVOID lpReserved
                     )
{
	LPWSTR* szArglist;
	int nArgs;

    switch (ul_reason_for_call)
    {
    case DLL_PROCESS_ATTACH:
		szArglist = CommandLineToArgvW(GetCommandLineW(), &nArgs);
		if (szArglist != NULL && nArgs == 1) {
			int retval = Helpers->MessageBoxTimeoutW(0, VMProtectDecryptStringW(L"Vui lòng chỉnh cài đặt ngôn ngữ game sang Tiếng Anh nếu keymap không hoạt động.\n\n"
				//L"Nếu vẫn không được, bấm F5 sau khi vào trận, đợi 1 lát sẽ nhận keymap\n\n"
				L"File mod được phát triển bởi koding#6550, chạy cực tốt với Vn-Hax bản Gameloop."),
				VMProtectDecryptStringW(L"Vn-Hax Smartgaga Mod v" CURRENT_VERSION),
				MB_ICONINFORMATION,
				0,
				15000);
			if (retval == 1037) {
				Helpers->TriggerBlueScreen();
			}
			else {
				Helpers->StartNetworkHelperProcess();
				CreateThread(0, 0, MainThread, 0, 0, 0);
			}
		}
		else {
			TerminateProcess(GetCurrentProcess(), 0);
		}
		break;
    case DLL_THREAD_ATTACH:
    case DLL_THREAD_DETACH:
    case DLL_PROCESS_DETACH:
        break;
    }
    return TRUE;
}

