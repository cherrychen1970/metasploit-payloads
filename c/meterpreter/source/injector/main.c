#include <windows.h>

#define METSRV_DLL "metro.x64.dll"

#define METSRV_DLL_ALT "metsrv\\metro.x64.dll"

int main()
{
	HANDLE h = LoadLibraryA(METSRV_DLL);
	if (!h)
		LoadLibraryA(METSRV_DLL_ALT);
	getchar();
}