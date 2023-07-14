#pragma once
#include <Windows.h>
#include <CommCtrl.h>
#include <UserEnv.h>
#include <strsafe.h>
#include <PathCch.h>
#include <sal.h>
#include "resource.h"

#if defined _M_IX86
#pragma comment(linker,"/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='x86' publicKeyToken='6595b64144ccf1df' language='*'\"")
#elif defined _M_IA64
#pragma comment(linker,"/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='ia64' publicKeyToken='6595b64144ccf1df' language='*'\"")
#elif defined _M_X64
#pragma comment(linker,"/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='amd64' publicKeyToken='6595b64144ccf1df' language='*'\"")
#else
#pragma comment(linker,"/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='*' publicKeyToken='6595b64144ccf1df' language='*'\"")
#endif

typedef struct _TCLSETTINGS
{
	WCHAR *wszRootDir;
	WCHAR *wszCommandC;
	SIZE_T cchC;
	WCHAR *wszCommandD;
	SIZE_T cchD;
} TCLSETTINGS, *PTCLSETTINGS;

typedef struct _CHANGEINFO
{
	WCHAR wszFullPath[MAX_PATH];
	BOOL fCreative; // FALSE = Destructive
	FILETIME ftChange;
} CHANGEINFO, *PCHANGEINFO;

INT_PTR CALLBACK DialogProc(_In_ HWND hDlg, _In_ UINT uMsg, _In_ WPARAM wParam, _In_ LPARAM lParam);
SIZE_T WINAPI GetRequiredOutBufferSize(_In_ SIZE_T cchFormattedString, _In_reads_(cchFormattedString) WCHAR *wszFormattedString);

_Success_(return)
_Ret_range_(FALSE, TRUE)

BOOL WINAPI GetSettingsCch(_Out_ PSIZE_T pCchRoot, _Out_ PSIZE_T pCchCC, _Out_ PSIZE_T pCchDC);
BOOL WINAPI GetSettings(_Inout_ PTCLSETTINGS pSettings, _In_ SIZE_T cbRoot, _In_ SIZE_T cbCr, _In_ SIZE_T cbDe);
BOOL WINAPI ExpandCommandString(_In_ SIZE_T cchIn, _In_reads_or_z_(cchIn) WCHAR *wszIn, _In_ SIZE_T cchOut, _Out_writes_z_(cchOut) WCHAR *wszOut, _In_reads_or_z_(MAX_PATH) WCHAR *wszNotifyingFile);
DWORD WINAPI ThreadProc(_In_ LPVOID lpParameter);
INT APIENTRY wWinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance, _In_ LPWSTR lpCmdLine, _In_ INT nShowCmd);
