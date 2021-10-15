#include <Windows.h>
#include <vector>
#include "IDM.h"

HINSTANCE gInstance;
std::vector<HWND> windows;

void hideWindow();
void showWindow();
void showAll();
LRESULT CALLBACK WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

int CALLBACK wWinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance, _In_ LPWSTR lpCmdLine, _In_ int nCmdShow)
{
	gInstance = hInstance;
	WNDCLASSEX wx = { sizeof(WNDCLASSEX) };

	wx.lpfnWndProc = WndProc;
	wx.hInstance = hInstance;
	wx.lpszClassName = L"WindowHider";
	RegisterClassEx(&wx);
	HWND hWnd = CreateWindowEx(0, L"WindowHider", L"", 0, 0, 0, 0, 0, HWND_MESSAGE, NULL, NULL, NULL);
	ShowWindow(hWnd, SW_HIDE);
	RegisterHotKey(hWnd, IDM_HIDE, MOD_CONTROL | MOD_NOREPEAT, '1');
	RegisterHotKey(hWnd, IDM_SHOW, MOD_CONTROL | MOD_NOREPEAT, '2');

	MSG msg;
	while (GetMessage(&msg, NULL, 0, 0))
	{
		if (msg.message == WM_HOTKEY)
		{
			if (msg.wParam == IDM_HIDE)
			{
				hideWindow();
			}
			else if (msg.wParam == IDM_SHOW)
			{
				showWindow();
			}
		}
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}
	return 0;
}

void hideWindow()
{
	HWND window = GetForegroundWindow();
	ShowWindow(window, SW_HIDE);
	windows.push_back(window);
}

void showWindow()
{
	if (!windows.empty())
	{
		HWND window = windows.back();
		ShowWindow(window, SW_SHOW);
		SetForegroundWindow(window);
		windows.pop_back();
	}
}

void showAll()
{
	for (auto& window : windows)
	{
		ShowWindow(window, SW_SHOW);
		windows.pop_back();
	}
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	static NOTIFYICONDATAW niData = { sizeof(NOTIFYICONDATAW) };

	switch (uMsg)
	{
		case WM_CREATE:
		{
			niData.uID = 1;
			niData.uFlags = NIF_ICON | NIF_MESSAGE | NIF_TIP;
			niData.hIcon = LoadIcon(NULL, IDI_SHIELD);
			niData.hWnd = hWnd;
			niData.uCallbackMessage = WM_USER + 1;

			Shell_NotifyIcon(NIM_ADD, &niData);
			return 0;
		}
		case WM_DESTROY:
		{
			Shell_NotifyIcon(NIM_DELETE, &niData);
			PostQuitMessage(0);
			return 0;
		}
		case WM_USER + 1:
		{
			if (LOWORD(lParam) == WM_RBUTTONUP || LOWORD(lParam) == WM_LBUTTONUP)
			{
				HMENU menu = CreatePopupMenu();
				POINT pClickPoint;
				GetCursorPos(&pClickPoint);

				InsertMenu(menu, 0, MF_BYPOSITION | MF_STRING, IDM_SHOWALL, L"Show All");
				InsertMenu(menu, 1, MF_SEPARATOR | MF_BYPOSITION, 0, NULL);
				InsertMenu(menu, 2, MF_BYPOSITION | MF_STRING, IDM_EXIT, L"Exit");
				SetForegroundWindow(hWnd);
				TrackPopupMenu(menu, TPM_LEFTALIGN | TPM_LEFTBUTTON | TPM_BOTTOMALIGN, pClickPoint.x, pClickPoint.y, 0, hWnd, NULL);
			}
			break;
		}
		case WM_COMMAND:
		{
			if (LOWORD(wParam) == IDM_SHOWALL)
			{
				showAll();
			}
			else if (LOWORD(wParam) == IDM_EXIT)
			{
				showAll();
				PostMessageW(hWnd, WM_DESTROY, NULL, NULL);
			}
			break;
		}
	}
	return DefWindowProc(hWnd, uMsg, wParam, lParam);
}