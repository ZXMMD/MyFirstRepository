#include "stdio.h"  
#include "wchar.h"  
#include "windows.h"  

typedef BOOL(WINAPI* PFSETWINDOWTEXTW)(HWND hWnd, LPWSTR lpString); 

FARPROC g_pOrgFunc = NULL;


BOOL WINAPI MyFunc(HWND hWnd, LPWSTR lpString)
{
	wchar_t* pNum = L"零一二三四五六七八九";
	wchar_t temp[2] = { 0, };
	int i = 0, nLen = 0, nIndex = 0;

	nLen = wcslen(lpString);
	for (i = 0; i < nLen; i++)
	{
		if (L'0' <= lpString[i] && lpString[i] <= L'9')
		{
			temp[0] = lpString[i];
			nIndex = _wtoi(temp);
			lpString[i] = pNum[nIndex];
		}
	}
	return ((PFSETWINDOWTEXTW)g_pOrgFunc)(hWnd, lpString);
}


BOOL hook_iat(LPCSTR szDllName, PROC pfnOrg, PROC pfnNew)
{
	HMODULE hMod;
	LPCSTR szLibName;
	PIMAGE_IMPORT_DESCRIPTOR pImportDesc;
	PIMAGE_THUNK_DATA pThunk;
	DWORD dwOldProtect, dwRVA;
	PBYTE pAddr;
	hMod = GetModuleHandle(NULL);
	pAddr = (PBYTE)hMod;
	pAddr += *((DWORD*)&pAddr[0x3C]);
	dwRVA = *((DWORD*)&pAddr[0x80]);
	pImportDesc = (PIMAGE_IMPORT_DESCRIPTOR)((DWORD)hMod + dwRVA);

	for (; pImportDesc->Name; pImportDesc++)
	{
		szLibName = (LPCSTR)((DWORD)hMod + pImportDesc->Name);
		if (!_stricmp(szLibName, szDllName))
		{
			pThunk = (PIMAGE_THUNK_DATA)((DWORD)hMod +
				pImportDesc->FirstThunk);
			for (; pThunk->u1.Function; pThunk++)
			{
				if (pThunk->u1.Function == (DWORD)pfnOrg)
				{
					VirtualProtect((LPVOID)&pThunk->u1.Function,
						4,
						PAGE_EXECUTE_READWRITE,
						&dwOldProtect);

					pThunk->u1.Function = (DWORD)pfnNew;

					VirtualProtect((LPVOID)&pThunk->u1.Function,
						4,
						dwOldProtect,
						&dwOldProtect);

					return TRUE;
				}
			}
		}
	}

	return FALSE;
}



BOOL WINAPI DllMain(HINSTANCE hinstDLL, DWORD fdwReason, LPVOID lpvReserved)
{
	switch (fdwReason)
	{
	case DLL_PROCESS_ATTACH:
		g_pOrgFunc = GetProcAddress(GetModuleHandle(L"user32.dll"),
			"SetWindowTextW");
		hook_iat("user32.dll", g_pOrgFunc, (PROC)MyFunc);
		break;
	case DLL_PROCESS_DETACH:
		hook_iat("user32.dll", (PROC)MyFunc, g_pOrgFunc);
		break;
	}

	return TRUE;
}
