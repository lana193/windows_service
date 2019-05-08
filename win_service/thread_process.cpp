#include "stdafx.h"
#include "psapi.h"
#include "thread_process.h"

std::string ConvertUnsignIntToString(const DWORD cdwProcesses)
{
	std::string sProcesses;
	char szProcessBuffer[26] = { 0 };

	// Convert DWORD of processes to string
	if (_ultoa_s(cdwProcesses, szProcessBuffer, 10) == 0)
	{
		sProcesses = szProcessBuffer;
	}
	return sProcesses;
}

DWORD GetProcessInfo()
{
	DWORD dwProcesses = false;
	DWORD dwProcessBuffer[1024] = { 0 };
	DWORD dwBytesNeeded = 0;

	if (EnumProcesses
	(dwProcessBuffer, // A pointer to an array that receives the list of process identifiers
		sizeof(dwProcessBuffer), 	// The size of the pProcessIds array, in bytes
		&dwBytesNeeded) != 0) // The number of bytes returned in the pProcessIds array
	{
		// Calculate how many process identifiers were returned.
		dwProcesses = dwBytesNeeded / sizeof(DWORD);
	}
	return dwProcesses;
}

CThreadProcess::CThreadProcess()
{
}

CThreadProcess::~CThreadProcess()
{
}

DWORD CThreadProcess::Run()
{
	g_Log.WriteMessage("CThreadProcess::Run Start");

	if (m_hStopEvent == NULL)
	{
		g_Log.WriteErrorMessage("CThreadProcess: Stop Event is not initialized", false);
	}
	else
	{
		const DWORD cdwPeriodMilliSec = 3 * 1000;

		// Periodically check if the service has been requested to stop
		while (WaitForSingleObject(m_hStopEvent, cdwPeriodMilliSec) != WAIT_OBJECT_0)
		{
			g_Log.WriteMessage("Getting process info...");

			// thread function
			g_Log.WriteMessage(ConvertUnsignIntToString(GetProcessInfo()).append
			(" Number of processes").c_str());
		}
	}

	g_Log.WriteMessage("CThreadProcess::Run End");
	return ERROR_SUCCESS;
}

