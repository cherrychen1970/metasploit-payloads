#include <windows.h>
#define METSRV_DLL "metsrv\\metro.x64.dll"

int main()
{
	LoadLibraryA(METSRV_DLL);
	Sleep(-1);
}