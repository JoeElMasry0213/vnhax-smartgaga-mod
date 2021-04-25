#include "Hooks.h"
typedef BOOL (WINAPI * __WriteFile)(
    _In_ HANDLE hFile,
    _In_reads_bytes_opt_(nNumberOfBytesToWrite) LPCVOID lpBuffer,
    _In_ DWORD nNumberOfBytesToWrite,
    _Out_opt_ LPDWORD lpNumberOfBytesWritten,
    _Inout_opt_ LPOVERLAPPED lpOverlapped
    );
__WriteFile _WriteFile;

bool DpiInformationHasSentOut = false;

DWORD OnActivityChangeMidHookReturnPoint = 0;

DWORD _ecx = 0;
std::string PubgPackageName[3] = { "com.vng.pubgmobile", "com.rekoo.pubgm", "com.pubg.krmobile" };
bool SpoofedName = false;

std::string cHooks::MakeSetDpiPacket(std::string packageName)
{
    return "setextras:" + packageName + "|renderqualitylevel&es&2|contentscale&es&1.5";
}
void cHooks::SetGameDpi(std::string packageName, HANDLE hFile)
{
    std::string TestPackageName = MakeSetDpiPacket(packageName);
    int DpiPacketLen = TestPackageName.length();
    // create an buffer with these info:
    BYTE* SendBuffer = new BYTE[DpiPacketLen + 4]; // 4 is DpiPacketLen represent in first 4 bytes
    *(DWORD*)(SendBuffer) = DpiPacketLen;
    memcpy((void*)(SendBuffer + 4), TestPackageName.c_str(), DpiPacketLen);
    // now send out
    DWORD written = 0;
    _WriteFile(hFile, SendBuffer, DpiPacketLen + 4, &written, NULL);
    delete[] SendBuffer;
}
BOOL WINAPI WriteFileHk(
    _In_ HANDLE hFile,
    _In_reads_bytes_opt_(nNumberOfBytesToWrite) LPCVOID lpBuffer,
    _In_ DWORD nNumberOfBytesToWrite,
    _Out_opt_ LPDWORD lpNumberOfBytesWritten,
    _Inout_opt_ LPOVERLAPPED lpOverlapped
    )
{
    if (nNumberOfBytesToWrite > 20) {
        DWORD BufferSize = *(DWORD*)(lpBuffer);
        if (nNumberOfBytesToWrite - BufferSize == 4) {
            while (!Login->Authenticated) {
                Sleep(1000);
            }
            VMProtectBeginUltraLockByKey("SET_GAME_DPI");
            // highly possible this packet is setextra props
            if (memcmp((void*)((DWORD)lpBuffer + 4), "setextras", 9) == 0) {
                char* PropInfo = (char*)((DWORD)lpBuffer + 4);
                if (!DpiInformationHasSentOut) {
                    Hooks->SetGameDpi(VMProtectDecryptStringA("com.tencent.ig"), hFile);
                    Hooks->SetGameDpi(VMProtectDecryptStringA("com.vng.pubgmobile"), hFile);
                    Hooks->SetGameDpi(VMProtectDecryptStringA("com.rekoo.pubgm"), hFile);
                    Hooks->SetGameDpi(VMProtectDecryptStringA("com.pubg.krmobile"), hFile);
                    DpiInformationHasSentOut = true;
                }
            }
            VMProtectEnd();
        }
    }
    return _WriteFile(hFile, lpBuffer, nNumberOfBytesToWrite, lpNumberOfBytesWritten, lpOverlapped);
}
bool CurrentProgramIsComTencentIg = false;
std::string LastPackageName;
DWORD WINAPI ShowNotificationAndPatch()
{
    Sleep(2000);
    DWORD ProjectTitanAddr = (DWORD)GetModuleHandle(NULL);
    DWORD oldProtect;
    VirtualProtect((void*)(ProjectTitanAddr + 0x2ADEF), 2, PAGE_EXECUTE_READWRITE, &oldProtect);
    memcpy((void*)(ProjectTitanAddr + 0x2ADEF), "\xEB\x0A", 2);
    VirtualProtect((void*)(ProjectTitanAddr + 0x2ADEF), 2, oldProtect, &oldProtect);
    if (!CurrentProgramIsComTencentIg) {
        // send an enevt signal to app change event handler
       // MessageBoxW(0, VMProtectDecryptStringW(L"Bạn vui lòng thoát GAME (không phải thoát giả lập), xong chạy lại để tránh bị dis game khi chơi"), VMProtectDecryptStringW(L"Lưu ý"), MB_ICONINFORMATION);
        DWORD GlobalEngineClass = *(DWORD*)(ProjectTitanAddr + 0x1DDE14);
        HANDLE AppChangeEventHandle = (HANDLE)*(DWORD*)(GlobalEngineClass + 0x140);

        SetEvent(AppChangeEventHandle);
    }
    return 0;
}
void CheckPackageName()
{
 //   printf("Apk: 0x%X %s\n", _ecx, _ecx);
    std::string CurrentPackageName = (char*)_ecx;
    if (SpoofedName || CurrentPackageName.find(VMProtectDecryptStringA(".GameActivity")) == std::string::npos)
        return;
    
    bool ApproveForPatching = false;
    if (CurrentPackageName.find(VMProtectDecryptStringA("com.tencent.ig")) != std::string::npos) {
        // just patch it, no need to spoof
        ApproveForPatching = true;
        CurrentProgramIsComTencentIg = true;
        goto patch_then_exit;
    }
    if (!SpoofedName) {
        for (auto& PubgPackage : PubgPackageName) {
            int position = CurrentPackageName.find(PubgPackage);
            if (position != std::string::npos) {
                LastPackageName = CurrentPackageName;
                CurrentPackageName.replace(position, PubgPackage.length(), VMProtectDecryptStringA("com.tencent.ig"));

                // free the string
                memset((void*)_ecx, 0, strlen((char*)_ecx));
                // commit change
                memcpy((void*)_ecx, CurrentPackageName.c_str(), CurrentPackageName.length());

                ApproveForPatching = true;
                break;
            }
        }
    }
    // fuck it
    patch_then_exit:
    if (!SpoofedName && ApproveForPatching) {
        SpoofedName = true;
        CreateThread(0, 0, (LPTHREAD_START_ROUTINE)ShowNotificationAndPatch, 0, 0, 0);
    }
  //  printf("Out: %s\n", _ecx);
    _ecx = NULL;
}
void __declspec(naked) NakedOnActivityChange()
{
    __asm
    {
        mov ecx, [esp - 8];
        pushad;
        mov _ecx, ecx;
        //mov _ecx, ecx;
    }
    // now we have ecx, which is "Src", now dump the name
    if (_ecx != NULL) {
        CheckPackageName();
        //printf("Out: %s\n", _ecx);
    }
    __asm{
        popad;
        jmp OnActivityChangeMidHookReturnPoint;
    }
}
void cHooks::Init()
{
    if (MH_Initialize() != MH_OK) 
        return;

    DWORD WriteFileAddr = (DWORD)&WriteFile;
    DWORD EngineDll = (DWORD)GetModuleHandleW(L"Engine.dll");
    DWORD ProjectTitanAddr = (DWORD)GetModuleHandleW(NULL);
    if (EngineDll == NULL) 
        return;
    DWORD OnActivityChangeAddr = ProjectTitanAddr + 0x2A8F2;

    MH_STATUS status = MH_CreateHook((LPVOID)WriteFileAddr, &WriteFileHk, reinterpret_cast<LPVOID*>(&_WriteFile));
    if (status != MH_OK) {
        Helpers->MessageBoxTimeoutW(NULL, L"Error when hooking API (0)", L"Error", MB_ICONERROR, NULL, 15000);
        return;
    }
    status = MH_CreateHook((LPVOID)OnActivityChangeAddr, &NakedOnActivityChange, reinterpret_cast<LPVOID*>(&OnActivityChangeMidHookReturnPoint));
    if (status != MH_OK) {
        Helpers->MessageBoxTimeoutW(NULL, L"Error when hooking API (0_1)", L"Error", MB_ICONERROR, NULL, 15000);
        return;
    }
    status = MH_EnableHook(MH_ALL_HOOKS);
    if (status != MH_OK) {
        Helpers->MessageBoxTimeoutW(NULL, L"Error when hooking API (1)", L"Error", MB_ICONERROR, NULL, 15000);
        return;
    }
}