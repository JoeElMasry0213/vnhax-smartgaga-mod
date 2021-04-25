#ifndef _HOOKS_H_
#define _HOOKS_H_
#include "pch.h"
class cHooks {
public:
	void SetGameDpi(std::string packageName, HANDLE hFile);
	void Init();
private:
	std::string MakeSetDpiPacket(std::string packageName);
};
extern cHooks* Hooks;
#endif
