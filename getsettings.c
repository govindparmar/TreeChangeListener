#include "TCL.h"

_Success_(return)
_Ret_range_(FALSE, TRUE)

BOOL WINAPI GetSettingsCch(_Out_ PSIZE_T pCchRoot, _Out_ PSIZE_T pCchCC, _Out_ PSIZE_T pCchDC)
{
	HKEY hKey;
	LSTATUS ls;
	DWORD dwCreate = 0, dwCB = 0;

	*pCchRoot = *pCchCC = *pCchDC = 0;

	ls = RegCreateKeyExW(HKEY_CURRENT_USER, L"SOFTWARE\\GovindParmar\\TreeChangeListener", 0, NULL, 0, KEY_QUERY_VALUE, NULL, &hKey, &dwCreate);
	if (ls != ERROR_SUCCESS)
	{
		return FALSE;
	}

	if (REG_CREATED_NEW_KEY == dwCreate)
	{
		return TRUE;
	}

	RegQueryValueExW(hKey, L"TreeRoot", NULL, NULL, NULL, &dwCB);
	*pCchRoot = (SIZE_T)(dwCB + sizeof(WCHAR));

	RegQueryValueExW(hKey, L"CCommand", NULL, NULL, NULL, &dwCB);
	*pCchCC = (SIZE_T) (dwCB + sizeof(WCHAR));

	RegQueryValueExW(hKey, L"DCommand", NULL, NULL, NULL, &dwCB);
	*pCchDC = (SIZE_T) (dwCB + sizeof(WCHAR));

	RegCloseKey(hKey);
	return TRUE;
}

BOOL WINAPI GetSettings(_Inout_ PTCLSETTINGS pSettings, _In_ SIZE_T cbRoot, _In_ SIZE_T cbCr, _In_ SIZE_T cbDe)
{
	HKEY hKey;
	LSTATUS ls;
	DWORD dwCB;

	ls = RegCreateKeyExW(HKEY_CURRENT_USER, L"SOFTWARE\\GovindParmar\\TreeChangeListener", 0, NULL, 0, KEY_QUERY_VALUE, NULL, &hKey, NULL);
	if (ls != ERROR_SUCCESS)
	{
		return FALSE;
	}
	dwCB = (DWORD) cbRoot;
	ls = RegQueryValueExW(hKey, L"TreeRoot", NULL, NULL, (LPBYTE) pSettings->wszRootDir, &dwCB);
	if (ls != ERROR_SUCCESS && ls != ERROR_FILE_NOT_FOUND)
	{
		return FALSE;
	}

	dwCB = (DWORD) cbCr;
	ls = RegQueryValueExW(hKey, L"CCommand", NULL, NULL, (LPBYTE) pSettings->wszCommandC, &dwCB);
	if (ls != ERROR_SUCCESS && ls != ERROR_FILE_NOT_FOUND)
	{
		return FALSE;
	}

	dwCB = (DWORD) cbDe;
	ls = RegQueryValueExW(hKey, L"DCommand", NULL, NULL, (LPBYTE) pSettings->wszCommandD, &dwCB);
	if (ls != ERROR_SUCCESS && ls != ERROR_FILE_NOT_FOUND)
	{
		return FALSE;
	}

	RegCloseKey(hKey);
	return TRUE;
}