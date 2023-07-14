#include "TCL.h"

INT APIENTRY wWinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance, _In_ LPWSTR lpCmdLine, _In_ INT nShowCmd)
{
	CONST HANDLE hHeap = GetProcessHeap();
#ifdef _WIN64
	UINT64
#else
	UINT32
#endif
		uLen;
	HRESULT hr;
	HWND hWnd;
	MSG Msg;
	WNDCLASSEXW wcex;
	HANDLE hThread;
	TCLSETTINGS settings;
	SIZE_T cbRoot, cbCr, cbDe;
	INITCOMMONCONTROLSEX iccx;

	iccx.dwICC = ICC_STANDARD_CLASSES;
	iccx.dwSize = sizeof(INITCOMMONCONTROLSEX);
	InitCommonControlsEx(&iccx);
	hr = StringCchLengthW(lpCmdLine, 50, &uLen);
	if (SUCCEEDED(hr))
	{
		if ((L'-' == lpCmdLine[0] || L'/' == lpCmdLine[0]) && _wcsicmp(lpCmdLine + 1, L"config") == 0)
		{
			
			DialogBoxW(hInstance, MAKEINTRESOURCEW(IDD_SETTINGS), NULL, DialogProc);
			return 0;
		}
	}

	ZeroMemory(&wcex, sizeof(WNDCLASSEXW));
	wcex.cbSize = sizeof(WNDCLASSEXW);
	wcex.hInstance = hInstance;
	wcex.lpszClassName = L"TREE_CHANGE_LISTENER_MO_CLASS";
	wcex.lpfnWndProc = DefWindowProcW;

	RegisterClassExW(&wcex);
	hWnd = CreateWindowW(wcex.lpszClassName, L"", 0, 0, 0, 0, 0, HWND_MESSAGE, NULL, hInstance, NULL);
	if (NULL == hWnd)
	{
		MessageBoxW(NULL, L"Could not initialize tree change listener thread", L"Error", MB_OK | MB_ICONSTOP);
		return -1;
	}
	GetSettingsCch(&cbRoot, &cbCr, &cbDe);
	settings.wszRootDir = (WCHAR *) HeapAlloc(hHeap, HEAP_ZERO_MEMORY, cbRoot);
	if (NULL == settings.wszRootDir)
	{
		return ERROR_OUTOFMEMORY;
	}

	settings.wszCommandC = (WCHAR *) HeapAlloc(hHeap, HEAP_ZERO_MEMORY, cbCr);
	if (NULL == settings.wszCommandC)
	{
		return ERROR_OUTOFMEMORY;
	}
	settings.cchC = cbCr / sizeof(WCHAR);
	settings.wszCommandD = (WCHAR *) HeapAlloc(hHeap, HEAP_ZERO_MEMORY, cbDe);
	if (NULL == settings.wszCommandD)
	{
		return ERROR_OUTOFMEMORY;
	}
	settings.cchD = cbDe / sizeof(WCHAR);

	GetSettings(&settings, cbRoot, cbCr, cbDe);
	hThread = CreateThread(NULL, 0, ThreadProc, &settings, 0, NULL);
	if (NULL == hThread)
	{
		ExitProcess(GetLastError());
	}

	while (GetMessageW(&Msg, NULL, 0, 0) > 0)
	{
		TranslateMessage(&Msg);
		DispatchMessageW(&Msg);
	}

	WaitForSingleObject(hThread, INFINITE);
	return (INT) Msg.wParam;
}