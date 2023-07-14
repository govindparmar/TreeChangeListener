#include "TCL.h"

SIZE_T WINAPI GetRequiredOutBufferSize(_In_ SIZE_T cchFormattedString, _In_reads_(cchFormattedString) WCHAR *wszFormattedString)
{
	SIZE_T i = 0, cch = 0;

	while (wszFormattedString[i])
	{
		if (L'%' == wszFormattedString[i] && wszFormattedString[i + 1])
		{
			switch (wszFormattedString[i + 1])
			{
			case L'%':
				cch++;
				break;
			case L'F':
			case L'D':
				cch += MAX_PATH;
				break;
			default:
				SetLastError(ERROR_BAD_ARGUMENTS);
				return (SIZE_T) -1;
			}

			i += 2;
		}
		else
		{
			cch++;
			i++;
		}
	}

	SetLastError(ERROR_SUCCESS);
	return cch;
}