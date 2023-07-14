#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sal.h>

int __cdecl wmain(int argc, wchar_t *argv[])
{
	unsigned long levels;
	wchar_t *pStart = NULL, *pInc = NULL, wszPath[_MAX_PATH], *pCmd = NULL;
	size_t cb;
	if (argc < 4)
	{
		_putws(L"Usage: SCPDIR N FILENAME SERVERNAME");
		_putws(L" N is the number of directory levels after the drive root on the local file system to not count when generating the remote directory.");
		_putws(L" So, SCPDIR 3 C:\\Dir1\\Dir2\\Dir3\\Dir4\\Dir5\\File.txt SERVERNAME will generate the command:\n");
		_putws(L" scp File.txt SERVERNAME:/Dir4/Dir5");
		return 0;
	}

	levels = wcstoul(argv[1], NULL, 10) + 1;
	wcscpy_s(wszPath, _MAX_PATH, argv[2]);
	
	pStart = argv[2];
	while (levels)
	{
		pStart = wcschr(pStart + 1, L'\\');
		levels--;
	}
	if (!pStart)
	{
		return -1;
	}
	pInc = pStart;
	while (*pInc)
	{
		if (L'\\' == *pInc)
		{
			*pInc = L'/';
		}
		pInc++;
	}
	if ((pInc = wcsrchr(pStart, L'/')) != NULL)
	{
		*pInc = L'\0';
	}
	cb = (wcslen(wszPath) + wcslen(pStart) + 100U) * sizeof(wchar_t);
	pCmd = (wchar_t *) malloc(cb);
	if (NULL == pCmd)
	{
		return -1;
	}
	swprintf(pCmd, cb / sizeof(wchar_t), L"scp %s SERVERNAME:%s", wszPath, pStart);
	_wsystem(pCmd);
	free(pCmd);
	pCmd = NULL;
	return 0;
}