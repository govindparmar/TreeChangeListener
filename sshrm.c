#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sal.h>

int __cdecl wmain(int argc, wchar_t *argv[])
{
	unsigned long levels;
	wchar_t *pStart = NULL, *pInc = NULL, *pCmd = NULL;
	size_t cb;

	if (argc < 4)
	{
		_putws(L"Usage: SSHRMF N FILENAME SERVERNAME");
		_putws(L" N is the number of directory levels after the drive root on the local file system to not count when generating the remote directory.");
		_putws(L" So, SCPDIR 3 C:\\Dir1\\Dir2\\Dir3\\Dir4\\Dir5\\File.txt SERVERNAME will generate the command:\n");
		_putws(L" ssh SERVERNAME \"rm -rf ~/dir4/dir5/file.txt\"");
		return 0;
	}

	levels = wcstoul(argv[1], NULL, 10) + 1;

	pStart = argv[2];
	while (levels && pStart)
	{
		pStart = wcschr(pStart + 1, L'\\');
		levels--;
	}
	if (!pStart)
	{
		return -1;
	}
	pInc = pStart;
	while (pInc && *pInc)
	{
		if (L'\\' == *pInc)
		{
			*pInc = L'/';
		}
		pInc++;
	}
	cb = (wcslen(argv[3]) + wcslen(pStart) + 100U) * sizeof(wchar_t);
	pCmd = (wchar_t *) malloc(cb);
	if (NULL == pCmd)
	{
		return -1;
	}
	swprintf(pCmd, cb / sizeof(wchar_t), L"ssh %s \"rm -rf ~%s\"", argv[3], pStart);
	_wsystem(pCmd);
	//wprintf(L"ssh %s \"rm -rf ~%s\"", argv[3], pStart);
	free(pCmd);
	pCmd = NULL;
	return 0;
}