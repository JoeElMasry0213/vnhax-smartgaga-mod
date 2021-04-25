#ifndef _GAMETHREADSUSPEND_H_
#define _GAMETHREADSUSPEND_H_
#include "pch.h"
struct SuspectedThread {
    DWORD ID;
    __int64 ExecutionTime;
};


class cGameThreadSuspender {
public:
    unsigned long FindGameProcessID(const wchar_t* processName, unsigned long* parentProcessId);
    void GetAndroidHomeThreadList();
    void StartSuspender();
    void SkipThreadSuspension() {
        OverrideThreadSuspension = true;
    }
private:
    bool OverrideThreadSuspension = false;
    std::vector<SuspectedThread> SuspectedThreadList;
    DWORD AndroidHomePID = 0;
};
extern cGameThreadSuspender* GameThreadSuspender;
#endif
