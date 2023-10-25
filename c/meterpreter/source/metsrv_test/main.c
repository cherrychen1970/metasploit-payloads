#include <stdbool.h>
#include "common.h"
#include "packet.h"
#include "base.h"
#include "core.h"
#include "common_metapi.h"

extern Command *extensionCommands;

// from ./mock.c
Remote *create_mock_remote();
extern DataApi data_api;
extern MetApi *met_api;

/* Sample */
static int dataformat_sample()
{
    void *root = data_api.new_dict();
    void *list = data_api.new_list();

    data_api.dict_add_int32(root,COMMAND_ID_STDAPI_SYS_PROCESS_GET_PROCESSES,0x1010);
    int32_t val;
    data_api.dict_get_int32(root,COMMAND_ID_STDAPI_SYS_PROCESS_GET_PROCESSES,&val);
    dprintf("%x",val);

    data_api.dict_add(root, 0, list);

    {
        void *process = data_api.new_dict();
        data_api.dict_add_string(process, 1, "abc");
        data_api.dict_add_bool(process, 2, false);
        data_api.dict_add_binary(process, 3, "XXXX", 4);
        data_api.list_add(list, process);
    }
    {
        void *process = data_api.new_dict();
        data_api.dict_add_string(process, 1, "abc");
        data_api.dict_add_bool(process, 2, false);
        data_api.dict_add_binary(process, 3, "XXXX", 4);
        data_api.list_add(list, process);
    }
    int len = data_api.list_length(list);

    for (int i = 0; i++; i < len)
    {
        void *process = data_api.list_get(list, i);
    }

    dprintf(data_api.to_string(root));
}

int main(int argc, char *argv[])
{
    // TEST: export data api via met_api.
    //memcpy(&met_api->data, &data_api, sizeof(data_api));
#if 0
    dataformat_sample();
#else
    Remote *remote = create_mock_remote();
    Packet *packet = packet_create(PACKET_TLV_TYPE_PLAIN_REQUEST, COMMAND_ID_STDAPI_SYS_PROCESS_GET_PROCESSES);
    packet_add_request_id(packet);

    void *nonexist = met_api->data->dict_get(packet->data,0);
    dprintf("here %x",nonexist);
    
    command_handle(remote, packet);
#endif
    getchar();
    return 0;
}
