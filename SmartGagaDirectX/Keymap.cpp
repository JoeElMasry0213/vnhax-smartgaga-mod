#include "Keymap.h"
HDC hDC = NULL;

#define POINTX(x) (float)((float)x/(float)1920)
#define POINTY(y) (float)((float)y/(float)1080)



__declspec(noinline) bool cKeymap::IsWhitePoint(float x, float y, int r, int g, int b)
{
	//printf("Fetching info: 0x%X - %d - %d\n", hDC, (int)(WindowSize.x * x), (int)(WindowSize.y * y));
	COLORREF _color = GetPixel(hDC, (int)(WindowSize.x * x), (int)(WindowSize.y * y));
	//printf("GetLastError = %d\n", GetLastError());
	int _red = GetRValue(_color);
	int _green = GetGValue(_color);
	int _blue = GetBValue(_color);
	//printf("color: %d - %d - %d\n", _red, _green, _blue);
	if (_red >= r && _green >= g && _blue >= b)
		return true;

	return false;
}
__declspec(noinline) bool cKeymap::IsBlackPoint(float x, float y)
{
	COLORREF _color = GetPixel(hDC, WindowSize.x * x, WindowSize.y * y);

	int _red = GetRValue(_color);
	int _green = GetGValue(_color);
	int _blue = GetBValue(_color);
	if (_red <= 40 && _green <= 40 && _blue <= 40)
		return true;

	return false;
}
__declspec(noinline) int cKeymap::TraceForWhitePixel(int StartX, int EndX, int Y, int Threadshold)
{
	int WhitePixelDetected = 0;
	for (int i = StartX; i <= EndX; i++) {
		if (IsWhitePoint(POINTX(i), POINTY(Y)))
			WhitePixelDetected++;

		if (WhitePixelDetected > Threadshold) 
			break;
	}
	return WhitePixelDetected;
}

__declspec(noinline) void cKeymap::SendKey(HWND hwnd, char KeyCode)
{
	CallWindowProc(oWndPrc, hwnd, WM_KEYDOWN, KeyCode, NULL);
	CallWindowProc(oWndPrc, hwnd, WM_KEYUP, KeyCode, NULL);
}
__declspec(noinline) void cKeymap::HandleMainKey(HWND hwnd, WPARAM wParam)
{
	if (wParam == 'F') {
		//	printf("Send\n");
			// Vehicle exit, no way to recognize this
		if (CanSendExitVehicle) {
			CanSendExitVehicle = false;
		//	printf("Exit vehicle\n");
			SendKey(hwnd, VK_NUMPAD2);
		}
		// check for button visible
		if (TraceForWhitePixel(1299, 1310, 202, 1) >= 1) { // F loot
		//	printf("Loot\n");
			SendKey(hwnd, VK_NUMPAD0);
		}
		if (IsWhitePoint(POINTX(1432), POINTY(483)) ||
			IsWhitePoint(POINTX(1444), POINTY(484)) ||
			IsWhitePoint(POINTX(1457), POINTY(483)) ||
			IsWhitePoint(POINTX(1447), POINTY(472))
			) { // F lootbox
		//	printf("Lootbox\n");
			SendKey(hwnd, VK_F10);
		}
		if (IsBlackPoint(POINTX(325), POINTY(701)) ||
			IsBlackPoint(POINTX(344), POINTY(704)) ||
			IsBlackPoint(POINTX(380), POINTY(702))
			) { // F Cancel BOOM
		//	printf("Abort gernade\n");
			SendKey(hwnd, VK_ADD);
		}
		if (IsWhitePoint(POINTX(1456), POINTY(616)) ||
			IsWhitePoint(POINTX(1411), POINTY(618)) ||
			IsWhitePoint(POINTX(1428), POINTY(618)) ||
			IsWhitePoint(POINTX(1507), POINTY(618)) ||
			// for G to enter car
			IsWhitePoint(POINTX(1411), POINTY(642)) ||
			IsWhitePoint(POINTX(1425), POINTY(641))
			) { // F enter vehicle
		//	printf("Enter vehicle\n");
			CanSendExitVehicle = true;
			SendKey(hwnd, VK_NUMPAD4);
		}
		if (IsWhitePoint(POINTX(1408), POINTY(830)) ||
			IsWhitePoint(POINTX(1409), POINTY(821)) ||
			IsWhitePoint(POINTX(1410), POINTY(836))
			) { // F open door
		//	printf("Open door\n");
			SendKey(hwnd, VK_NUMPAD3);
		}
		if (IsWhitePoint(POINTX(1435), POINTY(888))) { // F SKI
		//	printf("Ski\n");
			SendKey(hwnd, VK_NUMPAD6);
		}
		if (IsWhitePoint(POINTX(960), POINTY(768))) { // F revive teammate
		//	printf("Revive teammate\n");
			SendKey(hwnd, VK_NUMPAD5);
		}
		if (TraceForWhitePixel(925, 945, 891, 1) >= 1) { // F cancel
			// F cancel revice / use of medicine
		//	printf("Abort revive\n");
			SendKey(hwnd, VK_NUMPAD1);
		}
		if ( // FOLLOW
			IsWhitePoint(POINTX(264), POINTY(473)) ||
			IsWhitePoint(POINTX(290), POINTY(477)) ||
			IsWhitePoint(POINTX(297), POINTY(477)) ||
			IsWhitePoint(POINTX(325), POINTY(474)) ||
			// JUMP OUT PLANE
			IsWhitePoint(POINTX(301), POINTY(458)) ||
			IsWhitePoint(POINTX(313), POINTY(458)) ||
			IsWhitePoint(POINTX(315), POINTY(482)) ||
			IsWhitePoint(POINTX(337), POINTY(481)) 
			) { // F cancel revice / use of medicine
		//	printf("JUMP OUT PLANE\n");
			SendKey(hwnd, VK_F8);
		}
		/*
		if (// OPEN PARACHUTE
			IsWhitePoint(POINTX(1703), POINTY(884)) ||
			IsWhitePoint(POINTX(1738), POINTY(884)) ||
			IsWhitePoint(POINTX(1721), POINTY(922)) ||
			IsWhitePoint(POINTX(1720), POINTY(884))
		) {
			SendKey(hwnd, VK_F9);
		}
		*/
		/*
		for (BYTE N = VK_NUMPAD0; N < VK_NUMPAD7; N++) {
			CallWindowProc(oWndPrc, hwnd, WM_KEYDOWN, N, NULL);
			CallWindowProc(oWndPrc, hwnd, WM_CHAR, N, NULL);
			CallWindowProc(oWndPrc, hwnd, WM_KEYUP, N, NULL);
		}
		*/
	}
	if (wParam == 'G') {
		if (TraceForWhitePixel(1299, 1310, 202, 1) >= 1) { // G loot
			SendKey(hwnd, VK_NUMPAD7);
		}
		if (TraceForWhitePixel(1404, 1420, 710, 1) >= 1) { // G enter vehicle seat
		//	printf("Enter vehicle\n");
			CanSendExitVehicle = true;
			SendKey(hwnd, VK_NUMPAD8);
		}
	}
	if (wParam == 'B') {
		SendKey(hwnd, VK_NUMPAD9);
		SendKey(hwnd, VK_MULTIPLY);
	}
	if (wParam == 'C') {
		SendKey(hwnd, VK_SUBTRACT);
	}
}
LRESULT APIENTRY WndProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	if (uMsg == WM_KEYUP && wParam == VK_F5)
		GameThreadSuspender->SkipThreadSuspension();

	if (!Keymap->IsCursorShowing()) {
		if (uMsg == WM_KEYUP) {
			Keymap->HandleMainKey(hwnd, wParam);
		}
		if (uMsg == WM_KEYUP || uMsg == WM_KEYDOWN) {
			if (wParam == 'Q') {
				if (Keymap->IsWhitePoint(POINTX(1488), POINTY(749))) {
					char Key = VK_DECIMAL;
					if (uMsg == WM_KEYDOWN) {
						CallWindowProc(Keymap->GetOriginalWindowProc(), hwnd, WM_KEYDOWN, Key, NULL);
						//CallWindowProc(oWndPrc, hwnd, WM_CHAR, Key, NULL);
					}
					else
						CallWindowProc(Keymap->GetOriginalWindowProc(), hwnd, WM_KEYUP, Key, NULL);
				}
			}
			if (wParam == 'E') {
				if (Keymap->IsWhitePoint(POINTX(1632), POINTY(749))) {
					char Key = VK_DIVIDE;
					if (uMsg == WM_KEYDOWN) {
						CallWindowProc(Keymap->GetOriginalWindowProc(), hwnd, WM_KEYDOWN, Key, NULL);
						//	CallWindowProc(oWndPrc, hwnd, WM_CHAR, Key, NULL);
					}
					else
						CallWindowProc(Keymap->GetOriginalWindowProc(), hwnd, WM_KEYUP, Key, NULL);
				}
			}
		}
	}

	if (uMsg == WM_KEYUP) {
		if (wParam == 'M') {
			Keymap->SendKey(hwnd, VK_CONTROL);
		}
		if (wParam == VK_TAB) {
			Keymap->SendKey(hwnd, VK_CONTROL);
		}
	}

	return CallWindowProc(Keymap->GetOriginalWindowProc(), hwnd, uMsg, wParam, lParam);
}
void cKeymap::Init(HWND hwnd)
{
	hEmulatorWindow = hwnd;
	// create mouse monitor thread
	CreateThread(0, 0, (LPTHREAD_START_ROUTINE)StartWorkThread, (void*)this, 0, 0);

}
DWORD WINAPI cKeymap::Workthread()
{
	if (hEmulatorWindow == NULL) 
		return 0;
	// need to put it here, terminate the thread will let the hDC not able to use

	 //HINSTANCE _hGDI = LoadLibrary(VMProtectDecryptStringW(L"gdi32.dll"));
	hDC = GetDC(hEmulatorWindow);
	if (hDC == NULL)
		return 0;
	oWndPrc = (WNDPROC)SetWindowLongPtr(hEmulatorWindow, GWLP_WNDPROC, (__int3264)(LONG_PTR)WndProc);

	while (true) {
		CURSORINFO ci = { sizeof(CURSORINFO) };
		if (GetCursorInfo(&ci)) {
			if (ci.flags == 0)
				CursorShowing = false;
			else
				CursorShowing = true;
		}
		RECT RenderWindowSize;
		GetWindowRect(hEmulatorWindow, &RenderWindowSize);
		WindowSize = { (long)(RenderWindowSize.right - RenderWindowSize.left), (long)(RenderWindowSize.bottom - RenderWindowSize.top) };
		//printf("size %dx%d\n", WindowSize.x, WindowSize.y);
		Sleep(2000);
	}
}