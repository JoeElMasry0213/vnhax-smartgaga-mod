#include "GameThreadSuspender.h"

bool CompareExecutionTime(SuspectedThread i1, SuspectedThread i2)
{
	return (i1.ExecutionTime > i2.ExecutionTime);
}

__declspec(noinline) unsigned long cGameThreadSuspender::FindGameProcessID(const wchar_t* processName, unsigned long* parentProcessId)
{
	PROCESSENTRY32 pe32;
	pe32.dwSize = sizeof(PROCESSENTRY32);
	PROCESS_MEMORY_COUNTERS_EX pmc;

	DWORD PID = 0;

	HANDLE hSnapShot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
	if (hSnapShot == NULL)
		return 0;
	do {
		if (wcscmp(pe32.szExeFile, processName) == 0) {
			HANDLE temp = OpenProcess(PROCESS_ALL_ACCESS, false, pe32.th32ProcessID);
			if (GetProcessMemoryInfo(temp, (PROCESS_MEMORY_COUNTERS*)&pmc, sizeof(pmc))) {
				SIZE_T PrivateMem = (pmc.PrivateUsage / 1024) / 1024;
				if (PrivateMem > 500) {
					PID = pe32.th32ProcessID;
					*parentProcessId = pe32.th32ParentProcessID;
					break;
				}
			}
			CloseHandle(temp);
		}
	} while (Process32Next(hSnapShot, &pe32));
	CloseHandle(hSnapShot);

	return PID;
}
__declspec(noinline) void cGameThreadSuspender::GetAndroidHomeThreadList()
{
	HANDLE hThreadSnap = INVALID_HANDLE_VALUE;
	THREADENTRY32 te32;
	hThreadSnap = CreateToolhelp32Snapshot(TH32CS_SNAPTHREAD, 0);
	te32.dwSize = sizeof(THREADENTRY32);
	Thread32First(hThreadSnap, &te32);
	do
	{
		if (te32.th32OwnerProcessID == AndroidHomePID)
		{
			// push suspected thread into our vector
			HANDLE thrd = OpenThread(THREAD_ALL_ACCESS, false, te32.th32ThreadID);
			if (GetThreadPriority(thrd) == THREAD_PRIORITY_ABOVE_NORMAL) {
				SuspectedThread SuspectThread;
				SuspectThread.ID = te32.th32ThreadID;
				SuspectThread.ExecutionTime = 0;
				SuspectedThreadList.push_back(SuspectThread);
			}
			CloseHandle(thrd);
		}
	} while (Thread32Next(hThreadSnap, &te32));
	CloseHandle(hThreadSnap);
}

DWORD PossibleThreadId = 0;
void CpuLowerAndroidHomeThread()
{
	HANDLE ShittyThread = OpenThread(THREAD_ALL_ACCESS, false, PossibleThreadId);
	while (true) {
		SuspendThread(ShittyThread);
		Sleep(60);
		ResumeThread(ShittyThread);
		Sleep(5); // 5
	}
}
void cGameThreadSuspender::StartSuspender()
{
	DWORD GamePID = 0;
	DWORD ParentPID = 0;
	//AllocConsole();
	//freopen("CONOUT$", "w", stdout);


	while (GamePID == 0) {
		GamePID = FindGameProcessID(L"AndroidProcess.exe", &ParentPID);
		if (GamePID != 0)
			break;
		Sleep(5000);
	}

	PROCESSENTRY32 pe32;
	pe32.dwSize = sizeof(PROCESSENTRY32);

	// trace the first UI process
	HANDLE hSnapShot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
	Process32First(hSnapShot, &pe32);

	do {
		if (pe32.th32ParentProcessID == ParentPID) {
			AndroidHomePID = pe32.th32ProcessID;
			break;
		}
	} while (Process32Next(hSnapShot, &pe32));

	CloseHandle(hSnapShot);


	//DWORD PossibleThreadId = 0;
	int PossibleThreadIdShowCount = 0;
	bool SuspendNow = true;

snap_again:
	SuspectedThreadList.clear();
	GetAndroidHomeThreadList();
	// make sure it has at least 2 threads
	if (SuspectedThreadList.size() < 2) {
		Sleep(10000);
		goto snap_again;
	}
	// now loop through suspected thread, find out what thread is running top most

	FILETIME CreationTime, ExitTime, UserTime, KernelTime;
	float ScaleFraction = 0;
sync_cpu_usage:
	if (OverrideThreadSuspension)
		return;
	//printf("---------------------\n");
	for (auto& AndroidHomeThread : SuspectedThreadList) {
		HANDLE Thread = OpenThread(THREAD_ALL_ACCESS, false, AndroidHomeThread.ID);
		GetThreadTimes(Thread, &CreationTime, &ExitTime, &KernelTime, &UserTime);
		ULARGE_INTEGER ul;
		ul.LowPart = UserTime.dwLowDateTime;
		ul.HighPart = UserTime.dwHighDateTime;
		__int64 fileTime64 = ul.QuadPart;
		AndroidHomeThread.ExecutionTime = fileTime64;
		//	printf("Thread %d time %d\n", AndroidHomeThread.ID, AndroidHomeThread.ExecutionTime);
	}
	//printf("---------------------\n");
	std::sort(SuspectedThreadList.begin(), SuspectedThreadList.end(), CompareExecutionTime);
	//printf("---------------------\n");
	//for (auto& AndroidHomeThread : SuspectedThreadList) {
	//	printf("Thread %d time %d\n", AndroidHomeThread.ID, AndroidHomeThread.ExecutionTime);
	//}
	//printf("---------------------\n");
	if (SuspectedThreadList[0].ExecutionTime > 100000) {
		if (SuspectedThreadList[1].ExecutionTime > 1) {
			ScaleFraction = (float)(SuspectedThreadList[0].ExecutionTime / SuspectedThreadList[1].ExecutionTime);
			//printf("%d | %d => %f\n", SuspectedThreadList[0].ExecutionTime, SuspectedThreadList[1].ExecutionTime, ScaleFraction);
			if (ScaleFraction > 3.0f) {
				PossibleThreadId = SuspectedThreadList[0].ID;
			}
			else {
				Sleep(5000);
				goto sync_cpu_usage;
			}
		}
		else {
			if (SuspectedThreadList[0].ExecutionTime > 500000) {
				PossibleThreadId = SuspectedThreadList[0].ID;
			}
			else {
				Sleep(5000);
				goto sync_cpu_usage;
			}
		}
	}
	else {
		Sleep(5000);
		goto sync_cpu_usage;
	}
	if (PossibleThreadId == 0) {
		Sleep(5000);
		goto sync_cpu_usage;
	}
	//	printf("Being suspended\n");
		// take action on shitty thread
	CreateThread(0, 0, (LPTHREAD_START_ROUTINE)CpuLowerAndroidHomeThread, 0, 0, 0);
	/*
	HANDLE ShittyThread = OpenThread(THREAD_ALL_ACCESS, false, PossibleThreadId);
	bool Suspended = false;
	while (true) {
		GamePID = FindGameProcessID(L"AndroidProcess.exe", &ParentPID);
		if (GamePID == NULL && Suspended) {
			ResumeThread(ShittyThread);
			Suspended = false;
		}
		if (GamePID != NULL && !Suspended) {
			if (!SuspendNow) 
				Sleep(10000);
			SuspendNow = false;
			SuspendThread(ShittyThread);
			Suspended = true;
		}
		Sleep(5000);
	}
	*/
}