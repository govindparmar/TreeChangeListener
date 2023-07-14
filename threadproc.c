#include "TCL.h"

DWORD WINAPI ThreadProc(_In_ LPVOID lpParameter)
{
	CONST HANDLE hHeap = GetProcessHeap();
	TCLSETTINGS ts = *(TCLSETTINGS *) lpParameter;
	OVERLAPPED o;
	HANDLE hDir = INVALID_HANDLE_VALUE;
	FILE_NOTIFY_INFORMATION *pfni = NULL;
	BYTE *bBuffer = NULL;
	DWORD dwWait, dwRead, dwLen;
	WCHAR wszFilePath[MAX_PATH], wszTCLDir[MAX_PATH];
	CHANGEINFO lastChange = { 0 };
	WCHAR *wszExpandedC = NULL, *wszExpandedD = NULL;
	SIZE_T cchExpandedC = 0U, cchExpandedD = 0U;
	FILETIME ftChange;
	ULARGE_INTEGER ulLast, ulNow;
	BOOL fRecent;
	PROCESS_INFORMATION pi;
	STARTUPINFOW si;

	hDir = CreateFileW(ts.wszRootDir, FILE_LIST_DIRECTORY, FILE_SHARE_READ | FILE_SHARE_WRITE | FILE_SHARE_DELETE, NULL, OPEN_EXISTING, FILE_FLAG_BACKUP_SEMANTICS | FILE_FLAG_OVERLAPPED, NULL);
	if (INVALID_HANDLE_VALUE == hDir)
	{
		ExitProcess(GetLastError());
	}

	bBuffer = (BYTE *) HeapAlloc(hHeap, HEAP_ZERO_MEMORY, 16384);
	if (NULL == bBuffer)
	{
		ExitProcess(ERROR_OUTOFMEMORY);
	}

	cchExpandedC = GetRequiredOutBufferSize(ts.cchC, ts.wszCommandC);
	wszExpandedC = (WCHAR *) HeapAlloc(hHeap, HEAP_ZERO_MEMORY, cchExpandedC + sizeof(WCHAR));
	if (NULL == wszExpandedC)
	{
		ExitProcess(ERROR_OUTOFMEMORY);
	}

	cchExpandedD = GetRequiredOutBufferSize(ts.cchD, ts.wszCommandD);
	wszExpandedD = (WCHAR *) HeapAlloc(hHeap, HEAP_ZERO_MEMORY, cchExpandedD + sizeof(WCHAR));
	if (NULL == wszExpandedD)
	{
		ExitProcess(ERROR_OUTOFMEMORY);
	}

	GetCurrentDirectoryW(MAX_PATH, wszTCLDir);
	o.hEvent = CreateEventW(NULL, TRUE, FALSE, NULL);
	if (NULL == o.hEvent)
	{
		ExitProcess(GetLastError());
	}
	while (1)
	{
		ReadDirectoryChangesW(hDir, bBuffer, 16384, TRUE, FILE_NOTIFY_CHANGE_FILE_NAME | FILE_NOTIFY_CHANGE_LAST_WRITE, NULL, &o, NULL);
		dwWait = WaitForSingleObject(o.hEvent, INFINITE);
		if (WAIT_OBJECT_0 == dwWait)
		{
			GetOverlappedResult(hDir, &o, &dwRead, TRUE);
			//WaitForSingleObject(o.hEvent, INFINITE);
			pfni = (PFILE_NOTIFY_INFORMATION)&bBuffer[0];
			do
			{
				ZeroMemory(&pi, sizeof(PROCESS_INFORMATION));
				ZeroMemory(&si, sizeof(STARTUPINFOW));
				si.cb = sizeof(STARTUPINFOW);

				dwLen = pfni->FileNameLength / sizeof(WCHAR);
				pfni->FileName[dwLen] = L'\0';
				PathCchCombine(wszFilePath, MAX_PATH, ts.wszRootDir, pfni->FileName);
				GetSystemTimeAsFileTime(&ftChange);
				ulNow.LowPart = ftChange.dwLowDateTime;
				ulNow.HighPart = ftChange.dwHighDateTime;
				ulLast.LowPart = lastChange.ftChange.dwLowDateTime;
				ulLast.HighPart = lastChange.ftChange.dwHighDateTime;
				fRecent = (ulNow.QuadPart - ulLast.QuadPart) <= 30000000UI64;

				switch (pfni->Action)
				{
				case FILE_ACTION_RENAMED_NEW_NAME:
				case FILE_ACTION_ADDED:
				case FILE_ACTION_MODIFIED:
					if (wcscmp(wszFilePath, lastChange.wszFullPath) == 0 && lastChange.fCreative && fRecent)
					{
						//MessageBoxW(NULL, wszFilePath, L"Creative Change (Identical to Last!)", MB_OK | MB_ICONASTERISK);
					}
					else
					{
						ExpandCommandString(ts.cchC, ts.wszCommandC, cchExpandedC, wszExpandedC, wszFilePath);
						//_wsystem(wszExpandedC);
						if (CreateProcessW(
							NULL, wszExpandedC, NULL, NULL, FALSE, CREATE_NEW_PROCESS_GROUP | CREATE_NEW_CONSOLE | CREATE_UNICODE_ENVIRONMENT,
							NULL, wszTCLDir, &si, &pi))
						{
							WaitForSingleObject(pi.hProcess, INFINITE);
							CloseHandle(pi.hThread);
							CloseHandle(pi.hProcess);
						}
						
					}
					lastChange.fCreative = TRUE;
					break;
				case FILE_ACTION_RENAMED_OLD_NAME:
				case FILE_ACTION_REMOVED:
					if (wcscmp(wszFilePath, lastChange.wszFullPath) == 0 && !lastChange.fCreative && fRecent)
					{
						//MessageBoxW(NULL, wszFilePath, L"Destructive Change (Identical to Last!)", MB_OK | MB_ICONASTERISK);
					}
					else
					{
						ExpandCommandString(ts.cchD, ts.wszCommandD, cchExpandedD, wszExpandedD, wszFilePath);
						if (CreateProcessW(
							NULL, wszExpandedD, NULL, NULL, FALSE, CREATE_NEW_PROCESS_GROUP | CREATE_NEW_CONSOLE | CREATE_UNICODE_ENVIRONMENT,
							NULL, wszTCLDir, &si, &pi))
						{
							WaitForSingleObject(pi.hProcess, INFINITE);
							CloseHandle(pi.hThread);
							CloseHandle(pi.hProcess);
						}
					}
					lastChange.fCreative = FALSE;
					break;
				}
				StringCchCopyW(lastChange.wszFullPath, MAX_PATH, wszFilePath);
				lastChange.ftChange = ftChange;

				*((LPBYTE *)pfni) += pfni->NextEntryOffset;
			}
			while (pfni->NextEntryOffset);
		}
	}
	HeapFree(hHeap, 0, wszExpandedD);
	wszExpandedD = NULL;
	HeapFree(hHeap, 0, wszExpandedC);
	wszExpandedC = NULL;
	HeapFree(hHeap, 0, bBuffer);
	bBuffer = NULL;
	return ERROR_SUCCESS;
}