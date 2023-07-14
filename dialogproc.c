#include "TCL.h"

INT_PTR CALLBACK DialogProc(_In_ HWND hDlg, _In_ UINT uMsg, _In_ WPARAM wParam, _In_ LPARAM lParam)
{
	CONST HANDLE hHeap = GetProcessHeap();
	HWND hEditRoot = FindWindowExW(hDlg, NULL, L"Edit", NULL);
	HWND hEditCommand = FindWindowExW(hDlg, hEditRoot, L"Edit", NULL);
	HWND hEditDestructiveCommand = FindWindowExW(hDlg, hEditCommand, L"Edit", NULL);
	HWND hBtnOk = FindWindowExW(hDlg, NULL, L"Button", NULL);
	WCHAR *wszRoot = NULL, *wszCommand = NULL, *wszDestructiveCommand = NULL;
	INT nRoot, nCommand, nDestructiveCommand;
	DWORD dwCB = 0;
	LSTATUS ls = -1;
	HKEY hKey;
	HRESULT hr;
#ifdef _WIN64
	UINT64
#else
	UINT32
#endif
		uLen;

	switch (uMsg)
	{
	case WM_COMMAND:
		if ((HWND)lParam == hBtnOk)
		{
			nRoot = GetWindowTextLengthW(hEditRoot) + 1;
			nCommand = GetWindowTextLengthW(hEditCommand) + 1;
			nDestructiveCommand = GetWindowTextLengthW(hEditDestructiveCommand) + 1;

			wszRoot = (WCHAR *) HeapAlloc(hHeap, HEAP_ZERO_MEMORY, (SIZE_T) (nRoot + 1) * sizeof(WCHAR));
			if (NULL == wszRoot)
			{
				EndDialog(hDlg, ERROR_OUTOFMEMORY);
				break;
			}
			GetWindowTextW(hEditRoot, wszRoot, nRoot);
			
			wszCommand = (WCHAR *) HeapAlloc(hHeap, HEAP_ZERO_MEMORY, (SIZE_T) (nCommand + 1) * sizeof(WCHAR));
			if (NULL == wszCommand)
			{
				EndDialog(hDlg, ERROR_OUTOFMEMORY);
				break;
			}
			GetWindowTextW(hEditCommand, wszCommand, nCommand);

			wszDestructiveCommand = (WCHAR *) HeapAlloc(hHeap, HEAP_ZERO_MEMORY, (SIZE_T) (nDestructiveCommand + 1) * sizeof(WCHAR));
			if (NULL == wszDestructiveCommand)
			{
				EndDialog(hDlg, ERROR_OUTOFMEMORY);
				break;
			}
			GetWindowTextW(hEditDestructiveCommand, wszDestructiveCommand, nDestructiveCommand);

			ls = RegCreateKeyExW(HKEY_CURRENT_USER, L"SOFTWARE\\GovindParmar\\TreeChangeListener", 0, NULL, 0, KEY_SET_VALUE, NULL, &hKey, NULL);
			if (ls != ERROR_SUCCESS)
			{
				EndDialog(hDlg, ls);
				break;
			}

			hr = StringCbLengthW(wszRoot, (SIZE_T)(nRoot + 1) * sizeof(WCHAR), &uLen);
			if (FAILED(hr))
			{
				EndDialog(hDlg, ERROR_BUFFER_OVERFLOW);
				break;
			}
			uLen += sizeof(WCHAR);
			ls = RegSetValueExW(hKey, L"TreeRoot", 0, REG_SZ, (CONST BYTE *)wszRoot, (DWORD)uLen);
			if (ls != ERROR_SUCCESS)
			{
				EndDialog(hDlg, ls);
				break;
			}

			hr = StringCbLengthW(wszCommand, (SIZE_T) (nCommand + 1) * sizeof(WCHAR), &uLen);
			if (FAILED(hr))
			{
				EndDialog(hDlg, ERROR_BUFFER_OVERFLOW);
				break;
			}
			uLen += sizeof(WCHAR);
			ls = RegSetValueExW(hKey, L"CCommand", 0, REG_SZ, (CONST BYTE *)wszCommand, (DWORD)uLen);
			if (ls != ERROR_SUCCESS)
			{
				EndDialog(hDlg, ls);
				break;
			}

			hr = StringCbLengthW(wszDestructiveCommand, (SIZE_T) (nDestructiveCommand + 1) * sizeof(WCHAR), &uLen);
			if (FAILED(hr))
			{
				EndDialog(hDlg, ERROR_BUFFER_OVERFLOW);
				break;
			}
			uLen += sizeof(WCHAR);
			ls = RegSetValueExW(hKey, L"DCommand", 0, REG_SZ, (CONST BYTE *)wszDestructiveCommand, (DWORD) uLen);
			if (ls != ERROR_SUCCESS)
			{
				EndDialog(hDlg, ls);
				break;
			}

			RegCloseKey(hKey);
			HeapFree(hHeap, 0, wszRoot);
			wszRoot = NULL;
			HeapFree(hHeap, 0, wszCommand);
			wszCommand = NULL;
			HeapFree(hHeap, 0, wszDestructiveCommand);
			wszDestructiveCommand = NULL;
			EndDialog(hDlg, ERROR_SUCCESS);
		}
		return (INT_PTR) TRUE;
	case WM_INITDIALOG:
	{
		ls = RegCreateKeyExW(HKEY_CURRENT_USER, L"SOFTWARE\\GovindParmar\\TreeChangeListener", 0, NULL, 0, KEY_SET_VALUE | KEY_QUERY_VALUE, NULL, &hKey, NULL);
		if (ls != ERROR_SUCCESS)
		{
			return (INT_PTR) FALSE;
		}

		ls = RegQueryValueExW(hKey, L"TreeRoot", NULL, NULL, NULL, &dwCB);
		if (ls != ERROR_SUCCESS && ls != ERROR_MORE_DATA && ls != ERROR_FILE_NOT_FOUND)
		{
			return (INT_PTR) FALSE;
		}

		wszRoot = (WCHAR *) HeapAlloc(hHeap, HEAP_ZERO_MEMORY, dwCB);
		if (NULL == wszRoot)
		{
			return (INT_PTR) FALSE;
		}

		ls = RegQueryValueExW(hKey, L"TreeRoot", NULL, NULL, (LPBYTE) wszRoot, &dwCB);
		if (ls != ERROR_SUCCESS && ls != ERROR_FILE_NOT_FOUND)
		{
			return (INT_PTR) FALSE;
		}

		ls = RegQueryValueExW(hKey, L"CCommand", NULL, NULL, NULL, &dwCB);
		if (ls != ERROR_SUCCESS && ls != ERROR_MORE_DATA && ls != ERROR_FILE_NOT_FOUND)
		{
			return (INT_PTR) FALSE;
		}

		wszCommand = (WCHAR *) HeapAlloc(hHeap, HEAP_ZERO_MEMORY, dwCB);
		if (NULL == wszCommand)
		{
			return (INT_PTR) FALSE;
		}

		ls = RegQueryValueExW(hKey, L"CCommand", NULL, NULL, (LPBYTE) wszCommand, &dwCB);
		if (ls != ERROR_SUCCESS && ls != ERROR_FILE_NOT_FOUND)
		{
			return (INT_PTR) FALSE;
		}

		ls = RegQueryValueExW(hKey, L"DCommand", NULL, NULL, NULL, &dwCB);
		if (ls != ERROR_SUCCESS && ls != ERROR_MORE_DATA && ls != ERROR_FILE_NOT_FOUND)
		{
			return (INT_PTR) FALSE;
		}

		wszDestructiveCommand = (WCHAR *) HeapAlloc(hHeap, HEAP_ZERO_MEMORY, dwCB);
		if (NULL == wszDestructiveCommand)
		{
			return (INT_PTR) FALSE;
		}

		ls = RegQueryValueExW(hKey, L"DCommand", NULL, NULL, (LPBYTE) wszDestructiveCommand, &dwCB);
		if (ls != ERROR_SUCCESS && ls != ERROR_FILE_NOT_FOUND)
		{
			return (INT_PTR) FALSE;
		}
		RegCloseKey(hKey);

		SetWindowTextW(hEditRoot, wszRoot);
		SetWindowTextW(hEditCommand, wszCommand);
		SetWindowTextW(hEditDestructiveCommand, wszDestructiveCommand);
		HeapFree(hHeap, 0, wszRoot);
		wszRoot = NULL;
		HeapFree(hHeap, 0, wszCommand);
		wszCommand = NULL;



	}
	return (INT_PTR) TRUE;
	case WM_CLOSE:
	case WM_DESTROY:
		EndDialog(hDlg, ERROR_SUCCESS);
	default:
		return (INT_PTR) FALSE;
	}

	return (INT_PTR) FALSE;
}