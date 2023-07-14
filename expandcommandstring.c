#include "TCL.h"

BOOL WINAPI ExpandCommandString(_In_ SIZE_T cchIn, _In_reads_or_z_(cchIn) WCHAR *wszIn, _In_ SIZE_T cchOut, _Out_writes_z_(cchOut) WCHAR *wszOut, _In_reads_or_z_(MAX_PATH) WCHAR *wszNotifyingFile)
{
	SIZE_T i, i2 = 0;
	HRESULT hr;
#ifdef _WIN64
	UINT64
#else
	UINT32
#endif
		uLen;
	ZeroMemory(wszOut, cchOut * sizeof(WCHAR));
	
	hr = StringCchLengthW(wszNotifyingFile, MAX_PATH, &uLen);
	if (FAILED(hr))
	{
		SetLastError(ERROR_FILENAME_EXCED_RANGE);
		return FALSE;
	}

	for (i = 0; i < cchIn; i++)
	{
		if (wszIn[i] != L'%')
		{
			wszOut[i2] = wszIn[i];
			i2++;
		}
		else
		{
			if (i != (cchIn - 1))
			{
				WCHAR wcAfter = wszIn[i + 1];
				switch (wcAfter)
				{
				case L'%':
					wszOut[i2] = L'%';
					i++;
					i2++;
					break;
				case L'D':
					PathCchRemoveFileSpec(wszNotifyingFile, uLen);
				case L'F': // Expand to full file path
					hr = StringCchLengthW(wszNotifyingFile, MAX_PATH, &uLen);
					if (FAILED(hr))
					{
						SetLastError(ERROR_FILENAME_EXCED_RANGE);
						return FALSE;
					}
					if (FAILED(StringCchCatW(wszOut, cchOut, wszNotifyingFile)))
					{
						SetLastError(ERROR_INSUFFICIENT_BUFFER);
						return FALSE;
					}
					i2 += uLen;
					i++;
					break;
				default:
					break;
				}
			}
		}
	}

	SetLastError(ERROR_SUCCESS);
	return TRUE;
}