#include "common.h"

// define this as we are going to be injected via RDI
#define REFLECTIVEDLLINJECTION_VIA_LOADREMOTELIBRARYR

// define this as we want to use our own DllMain function
#define REFLECTIVEDLLINJECTION_CUSTOM_DLLMAIN

#define RDIDLL_NOEXPORT
#include "../ReflectiveDLLInjection/dll/src/ReflectiveLoader.c"

/*
 * The real entrypoint for this app.
 */
VOID test_main()
{
	DWORD dwResult = ERROR_INVALID_PARAMETER;
	ExitThread(0);
}

/*
 * DLL entry point. If we have been injected via RDI, lpReserved will be our command line.
 */
BOOL WINAPI DllMain(HINSTANCE hInstance, DWORD dwReason, LPVOID lpReserved)
{
    BOOL bReturnValue = TRUE;

	switch (dwReason)
    {
		case DLL_PROCESS_ATTACH:
			hAppInstance = hInstance;
			test_main();
			break;
		case DLL_PROCESS_DETACH:
		case DLL_THREAD_ATTACH:
		case DLL_THREAD_DETACH:
            break;
    }

	return bReturnValue;
}

