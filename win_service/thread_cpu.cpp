#include "stdafx.h"
#include "thread_cpu.h"

double ConvertFileTimeToSystemTime(FILETIME time)
{
	SYSTEMTIME systemTime;
	double dConvertedTime;
	if (FileTimeToSystemTime(&time, &systemTime) != 0)
	{
		dConvertedTime = (double)systemTime.wHour * 3600.0 +
			(double)systemTime.wMinute * 60.0 +
			(double)systemTime.wSecond +
			(double)systemTime.wMilliseconds / 1000.0;
	}
	else
	{
		g_Log.WriteErrorMessage("Failed to convert a file time to system time format", true);
	}

	return dConvertedTime;
}

int GetCpuUsage()
{
	FILETIME idleTime, kernelTime, userTime;
	FILETIME lastIdleTime, lastKernelTime, lastUserTime;
	int iCpuUsage;
	bool bRet = false;
	do
	{
		// Get system times first time
		if (GetSystemTimes(&idleTime, &kernelTime, &userTime) == 0)
		{
			g_Log.WriteErrorMessage("Failed to get System times", true);
			break;
		}
		else
		{
			Sleep(250);

			// Get system times again
			if (GetSystemTimes(&lastIdleTime, &lastKernelTime, &lastUserTime) == 0)
			{
				g_Log.WriteErrorMessage("Failed to get System times", true);
				break;
			}
			else
			{
				// Calculate system time (kernel time + user time)
				double dSystemTime = fabs(ConvertFileTimeToSystemTime(kernelTime) - ConvertFileTimeToSystemTime(lastKernelTime))
					+ fabs(ConvertFileTimeToSystemTime(userTime) - ConvertFileTimeToSystemTime(lastUserTime));
				g_Log.WriteMessage(std::to_string(dSystemTime).append("SystemTime").c_str());

				// Calculate idle time
				double dIdleTime = fabs(ConvertFileTimeToSystemTime(idleTime) - ConvertFileTimeToSystemTime(lastIdleTime));
				g_Log.WriteMessage(std::to_string(dIdleTime).append("Iddle").c_str());

				// calculate CPU usage
				iCpuUsage = int((dSystemTime - dIdleTime) * 100.0 / dSystemTime);
				bRet = true;
			}
		}
	} while (false);
	return iCpuUsage;
}

CThreadCpu::CThreadCpu()
{
}

CThreadCpu::~CThreadCpu()
{
}

DWORD CThreadCpu::Run()
{
	g_Log.WriteMessage("CThreadCpu::Run Start");

	if (m_hStopEvent == NULL)
	{
		g_Log.WriteErrorMessage("CThreadCpu: Stop Event is not initialized", false);
	}
	else
	{
		const DWORD cdwPeriodMilliSec = 3 * 1000;

		// Periodically check if the service has been requested to stop
		while (WaitForSingleObject(m_hStopEvent, cdwPeriodMilliSec) != WAIT_OBJECT_0)
		{
			g_Log.WriteMessage("Getting cpu info...");

			// thread function
			g_Log.WriteMessage(std::to_string(GetCpuUsage()).append("%	CPU usage").c_str());
		}
	}

	g_Log.WriteMessage("CThreadCpu::Run End");
	return ERROR_SUCCESS;
}

