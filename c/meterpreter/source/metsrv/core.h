/*!
 * @file core.h
 * @brief Declarations of core components of the Meterpreter suite.
 * @details Much of what exists in the core files is used in almost every area
 *          of the Meterpreter code base, and hence it's very important. Don't
 *          change this stuff unless you know what you're doing!
 */
#ifndef _METERPRETER_METSRV_CORE_H
#define _METERPRETER_METSRV_CORE_H

#include "common_remote.h"
#include "common_list.h"

/*
 * Core API
 */
HANDLE core_update_thread_token( Remote *remote, HANDLE token );
VOID core_update_desktop( Remote * remote, DWORD dwSessionID, char * cpStationName, char * cpDesktopName );

#endif
