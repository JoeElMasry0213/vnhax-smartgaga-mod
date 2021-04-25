#ifndef _KEYMAP_H_
#define _KEYMAP_H_
#include "pch.h"
class cKeymap {
public:
	bool IsWhitePoint(float x, float y, int r = 200, int g = 200, int b = 200);
	bool IsBlackPoint(float x, float y);
	int TraceForWhitePixel(int StartX, int EndX, int Y, int Threadshold = 999);
	void SendKey(HWND hwnd, char KeyCode);
	void HandleMainKey(HWND hwnd, WPARAM wParam);
	void Init(HWND hwnd);
	DWORD WINAPI Workthread();

	WNDPROC GetOriginalWindowProc() {
		return oWndPrc;
	}
	bool IsCursorShowing() {
		return CursorShowing;
	}
private:
	static DWORD WINAPI StartWorkThread(void* Param)
	{
		cKeymap* This = (cKeymap*)Param;
		return This->Workthread();
	}
	WNDPROC oWndPrc;
	bool CursorShowing = true;
	bool CanSendExitVehicle = false;

	HWND hEmulatorWindow = NULL;
	POINT WindowSize;
};
extern cKeymap* Keymap;
#endif
