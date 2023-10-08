#include "metsrv.h"
#include "base_inject.h"
#include "remote_thread.h"
//#include "../../ReflectiveDLLInjection/inject/src/LoadLibraryR.h"
#include <tlhelp32.h>

DWORD inject_via_apcthread(Remote * remote, Packet * response, HANDLE hProcess, DWORD dwProcessID, DWORD dwDestinationArch, LPVOID lpStartAddress, LPVOID lpParameter)
{
    dprintf( "[INJECT] not supported!");
    return ERROR_ACCESS_DENIED;
}

DWORD inject_via_remotethread(Remote * remote, Packet * response, HANDLE hProcess, DWORD dwDestinationArch, LPVOID lpStartAddress, LPVOID lpParameter)
{
    dprintf( "[INJECT] not supported!");
    return ERROR_ACCESS_DENIED;
}

DWORD inject_via_remotethread_wow64(HANDLE hProcess, LPVOID lpStartAddress, LPVOID lpParameter, HANDLE * pThread)
{
    dprintf( "[INJECT] not supported!");
    return ERROR_ACCESS_DENIED;
}

DWORD inject_dll(DWORD dwPid, LPVOID lpDllBuffer, DWORD dwDllLength, LPCSTR reflectiveLoader, char * cpCommandLine)
{
    dprintf( "[INJECT] not supported!");
    return ERROR_ACCESS_DENIED;
}
