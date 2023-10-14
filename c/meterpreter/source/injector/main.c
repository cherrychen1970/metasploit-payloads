#include <windows.h>

int main()
{
	HANDLE h = LoadLibraryA("metsrv.x64.dll");
	if (!h)
		LoadLibraryA("metsrv\\metsrv.x64.dll");
	getchar();
}