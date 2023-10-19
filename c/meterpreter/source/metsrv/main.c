#include "metsrv.h"
#include "json.h"

#define PRINT_LINE             dprintf("%d",__LINE__)
// from server_setup.c
DWORD server_sessionid();
Remote *create_remote();
 
int main(int argc, char *argv[])
{
    Remote *remote = create_remote();
    Packet *packet = packet_create(PACKET_TLV_TYPE_PLAIN_REQUEST, COMMAND_ID_CORE_MACHINE_ID);

    const char *json = json_to_string(packet->handle);
    dprintf("here %s",json);
    UINT commandId = packet_get_tlv_value_uint(packet, TLV_TYPE_COMMAND_ID);
    dprintf("here %x: %x",COMMAND_ID_CORE_MACHINE_ID, commandId);
    command_handle(remote, packet);
    getchar();
    return 0;
}

// for simple connection without config.
Remote *create_remote()
{
    THREAD *serverThread = NULL;
    Remote *remote = NULL;
    char stationName[256] = {0};
    char desktopName[256] = {0};
    DWORD res = 0;

    disable_thread_error_reporting();
    srand((unsigned int)time(NULL));

    __try
    {
        do
        {
            // Open a THREAD item for the servers main thread, we use this to manage migration later.
            serverThread = thread_open();
            dprintf("[SERVER] main server thread: handle=0x%08X id=0x%08X sigterm=0x%08X", serverThread->handle, serverThread->id, serverThread->sigterm);

            if (!(remote = remote_allocate()))
            {
                SetLastError(ERROR_NOT_ENOUGH_MEMORY);
                break;
            }

            remote->sess_expiry_time = 3600;
            remote->sess_start_time = current_unix_timestamp();
            if (remote->sess_expiry_time)
            {
                remote->sess_expiry_end = remote->sess_start_time + remote->sess_expiry_time;
            }
            else
            {
                remote->sess_expiry_end = 0;
            }

            dprintf("[DISPATCH] Session going for %u seconds from %u to %u", remote->sess_expiry_time, remote->sess_start_time, remote->sess_expiry_end);
#if 0
			DWORD transportSize = 0;
			MetsrvTransportCommon transport;
			wcsncpy(transport.url, url, UA_SIZE);
			transport.comms_timeout = 30;
			transport.retry_total = 600;
			transport.retry_wait = 10;


			// Set up the transport creation function pointer
			remote->trans_create = create_transport;
			// Set up the transport removal function pointer
			remote->trans_remove = remove_transport;
			// and the config creation pointer
			remote->config_create = config_create;
#endif
            // Store our thread handle
            remote->server_thread = serverThread->handle;

            dprintf("[SERVER] Registering dispatch routines...");
            register_dispatch_routines();

#if 0 // test later
			HMODULE hLibrary = LoadLibraryA("stdapi\\ext_server_stdapi.x64.dll");
			dprintf("%x",hLibrary);
			load_extension(hLibrary, FALSE, remote, NULL, extensionCommands);
#endif

            // Store our process token
            if (!OpenThreadToken(remote->server_thread, TOKEN_ALL_ACCESS, TRUE, &remote->server_token))
            {
                OpenProcessToken(GetCurrentProcess(), TOKEN_ALL_ACCESS, &remote->server_token);
            }

#if 1
            if (scheduler_initialize(remote) != ERROR_SUCCESS)
            {
                SetLastError(ERROR_BAD_ENVIRONMENT);
                break;
            }
#endif
            // Copy it to the thread token
            remote->thread_token = remote->server_token;

            // Save the initial session/station/desktop names...
            remote->orig_sess_id = server_sessionid();
            remote->curr_sess_id = remote->orig_sess_id;
            GetUserObjectInformation(GetProcessWindowStation(), UOI_NAME, &stationName, 256, NULL);
            remote->orig_station_name = _strdup(stationName);
            remote->curr_station_name = _strdup(stationName);
            GetUserObjectInformation(GetThreadDesktop(GetCurrentThreadId()), UOI_NAME, &desktopName, 256, NULL);
            remote->orig_desktop_name = _strdup(desktopName);
            remote->curr_desktop_name = _strdup(desktopName);
            remote->sess_start_time = current_unix_timestamp();
            dprintf("%d",__LINE__);

            return remote;
        } while (0);
    }
    __except (EXCEPTION_EXECUTE_HANDLER)
    {
        dprintf("[SERVER] *** exception triggered!");
        thread_kill(serverThread);
    }
    return NULL;
}
