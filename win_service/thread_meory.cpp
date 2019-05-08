#include "stdafx.h"
#include "psapi.h"
#include "threads.h"
#include "thread_memory.h"

std::string ConvertUnsignIntToString(const DWORDLONG cdwParam)
{
	std::string sParam;
	char szBuffer[MAX_PATH] = { 0 };
	if (_ui64toa_s(cdwParam, szBuffer, _countof(szBuffer), 10) == 0)
	{
		sParam = szBuffer;
	}
	return sParam;
}

DWORDLONG GetMemoryInfo(const int ciMemoryType)
{
	DWORDLONG dwMemoryRet = false;
	MEMORYSTATUSEX statusEx;

	statusEx.dwLength = sizeof(statusEx); // The size of the structure, in bytes
	if (GlobalMemoryStatusEx(&statusEx) != 0)
	{
		switch (ciMemoryType)
		{
		case 1: // get total memory
		{
			// calculate total physical memory
			dwMemoryRet = statusEx.ullTotalPhys / 1024;
			break;
		}
		case 2: // get used memory
		{
			// calculate physical memory in Use
			dwMemoryRet = (statusEx.ullTotalPhys - statusEx.ullAvailPhys) / 1024;
			break;
		}
		default:
			break;
		}
	}
	return dwMemoryRet;
}

CThreadMemory::CThreadMemory()
{
}
CThreadMemory::~CThreadMemory()
{
}

DWORD CThreadMemory::Run()
{
	g_Log.WriteMessage("CThreadMemory::Run Start");

	if (m_hStopEvent == NULL)
	{
		g_Log.WriteErrorMessage("CThreadMemory: Stop Event is not initialized", false);
	}
	else
	{
		const DWORD cdwPeriodMilliSec = 3 * 1000;
		//CSocketClient client;
		//client.Start();
		//client.CreateSocket();
		//client.ReceiveLine();
		// Periodically check if the service has been requested to stop
		while (WaitForSingleObject(m_hStopEvent, cdwPeriodMilliSec) != WAIT_OBJECT_0)
		{
			g_Log.WriteMessage("Getting memory info...");

			//char buf[256];
			//char pattern[]  = "%10s %10s     %7.2f %7.2f %7.2f %7.2f %7.2f %7.2f %7.2f %7.2f %7.2f";
			//sprintf(buf, pattern, "Bob",  "Doe",     10.96,      7.61,     14.39,      2.11,     47.30,     14.21,     44.58,      5.00,     60.23);


			// thread function
			char szBuffer[256] = { 0 };
			const char szPattern[] = "%s %10s %15s %10s";
			sprintf_s(szBuffer, szPattern, ConvertUnsignIntToString(GetMemoryInfo(1)).c_str(), "|Total physical memory|",
				ConvertUnsignIntToString(GetMemoryInfo(2)).c_str(), " Memory in use");
			g_Log.WriteMessage(szBuffer);
			//const std::string csMemory = ConvertUnsignIntToString(GetMemoryInfo(1)) + 
			//	" Total physical memory;	" + ConvertUnsignIntToString(GetMemoryInfo(2)) + 
			//	" Memory in use";|
			//g_Log.WriteMessage(csMemory.c_str());
		}
		//client.End();
		//client.Close();
	}

	g_Log.WriteMessage("CThreadMemory::Run End");
	return ERROR_SUCCESS;
}

