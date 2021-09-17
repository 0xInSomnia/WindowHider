#include <Windows.h>
#include "IDM.h"

LRESULT CALLBACK WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

int CALLBACK wWinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance, _In_ LPWSTR lpCmdLine, _In_ int nCmdShow)
{
	WNDCLASSEX wx = { sizeof(WNDCLASSEX) };

	wx.lpfnWndProc = WndProc;
	wx.hInstance = hInstance;
	wx.lpszClassName = L"WindowHider";
	RegisterClassEx(&wx);
	HWND hWnd = CreateWindowEx(0, L"WindowHider", L"", 0, 0, 0, 0, 0, HWND_MESSAGE, NULL, NULL, NULL);
	ShowWindow(hWnd, SW_HIDE);

	MSG stMsg;
	while (GetMessage(&stMsg, NULL, 0, 0))
	{
		TranslateMessage(&stMsg);
		DispatchMessage(&stMsg);
	}
	return 0;
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
			if (LOWORD(lParam) == IDM_SHOWALL)
			{
				OutputDebugString(L"Show all");
			}
			else if (LOWORD(lParam) == IDM_EXIT)
			{
				PostMessageW(hWnd, WM_DESTROY, NULL, NULL);
			}
			break;
		}
	}
	return DefWindowProc(hWnd, uMsg, wParam, lParam);
}