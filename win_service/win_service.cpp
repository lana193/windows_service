// win_service.cpp : Defines the entry point for the console application.
// C:\Users\Lana_19\vs_projects\win_service\x64\Debug\

#include "stdafx.h"
#include "threads.h"
#include "thread_process.h"
#include "thread_memory.h"
#include "thread_cpu.h"
#include "config_file.h"

CLogger						g_Log;
CWriteToLogFile				g_LogFile;
CReadFromConfig				g_Config;
SERVICE_STATUS				g_ServiceStatus = { 0 };
SERVICE_STATUS_HANDLE		g_hServiceStatusHandle = NULL;
HANDLE						g_hServiceStopEvent = NULL;
const char g_cszLogFileName[] = "\\win_service.log";
const char g_cszIniFileName[] = "\\win_service.ini";

#define SERVICE_NAME TEXT("Windows_Service")

bool ServiceReportStatus(DWORD dwCurrentState, DWORD dwWin32ExitCode, DWORD dwWaitHint)
{
	g_Log.WriteMessage("ServiceReportStatus Start");
	bool bRet = false;
	// Local variable Definitions
	static DWORD dwCheckPoint = 1;
	BOOL bSetServiceStatus = FALSE;

	// Filling The SERVICE_STATUS Structure
	g_ServiceStatus.dwCurrentState = dwCurrentState;
	g_ServiceStatus.dwWin32ExitCode = dwWin32ExitCode;
	g_ServiceStatus.dwWaitHint = dwWaitHint;

	// Check the Current State of Service
	do
	{
		if (dwCurrentState == SERVICE_START_PENDING) // Service is about to start
		{
			g_ServiceStatus.dwControlsAccepted = 0;
		}
		else
		{
			g_ServiceStatus.dwControlsAccepted = SERVICE_ACCEPT_STOP;
		}

		// Progress for Service operation
		if ((dwCurrentState == SERVICE_RUNNING) || (dwCurrentState == SERVICE_STOPPED))
		{
			g_ServiceStatus.dwCheckPoint = 0;
		}
		else
		{
			g_ServiceStatus.dwCheckPoint = dwCheckPoint++;
		}

		// Notify the current status of SCM
		bSetServiceStatus = SetServiceStatus(g_hServiceStatusHandle, &g_ServiceStatus);
		if (FALSE == bSetServiceStatus)
		{
			g_Log.WriteErrorMessage("Service Status Failed", true);
		}
		else
		{
			g_Log.WriteMessage("Service Status Success");
		}

		bRet = true;
	} while (false);

	g_Log.WriteMessage("ServiceReportStatus End");
	return bRet;
}

bool ServiceInit(DWORD dwArgc, LPTSTR * lpArgv)
{
	bool bRet = false;
	do
	{
		g_Log.WriteMessage("ServiceInit Start");

		if (NULL == g_hServiceStopEvent)
		{
			// Call ServiceReportStatus Fun to Notify SCM for Current Status of Service
			ServiceReportStatus(SERVICE_STOPPED, NO_ERROR, 0);
		}

		else
		{
			// Call ServiceReportStatus Function to Notify SCM for Current Status of Service
			ServiceReportStatus(SERVICE_RUNNING, NO_ERROR, 0);
		}

		bRet = true;
	} while (false);

	g_Log.WriteMessage("ServiceInit End");
	return bRet;
}

void WINAPI ServiceControlHandler(DWORD dwControl)
{
	g_Log.WriteMessage("ServiceControlHandler Start");
	switch (dwControl)
	{
	case SERVICE_CONTROL_STOP:
	{
		// SetEvent
		// This will signal the worker thread to start shutting down
		SetEvent(g_hServiceStopEvent);

		// Call ServiceReportStatus Function
		ServiceReportStatus(SERVICE_STOPPED, NO_ERROR, 0);
		break;
	}
	default:
		break;
	}
}

void WINAPI ServiceMain(DWORD dwArgc, LPTSTR * lpArgv)
{
	g_Log.WriteMessage("ServiceMain Start");
	// Local Variable definitions
	BOOL bServiceStatus = FALSE;
	// Create objects of future threads
	CThreadProcess processThread;
	CThreadMemory memoryThread;
	CThreadCpu cpuThread;
	g_Log.WriteMessage(g_Config.GetServiceName());
	// Create Event
	g_hServiceStopEvent = CreateEvent(
		NULL, // Security Attributes
		TRUE, // MANUAL Reset Event
		FALSE, // No Signaled
		NULL); // Name of Event

	// Registering Service Control Handler Function to SCM
	g_hServiceStatusHandle = RegisterServiceCtrlHandler((SERVICE_NAME), ServiceControlHandler);

	if (NULL == g_hServiceStatusHandle)
	{
		g_Log.WriteErrorMessage("RegisterServiceCtrlHandler Failed", true);
	}
	else
	{
		g_Log.WriteMessage("RegisterServiceCtrlHandler Success");

		// SERVICE_STATUS Initial Setup
		g_ServiceStatus.dwServiceType = SERVICE_WIN32_OWN_PROCESS;
		g_ServiceStatus.dwServiceSpecificExitCode = 0;

		// Call Service Report Status for Notifying Initial Setup
		ServiceReportStatus(SERVICE_START_PENDING, NO_ERROR, 0);

		// Check the Service Status
		bServiceStatus = SetServiceStatus(g_hServiceStatusHandle, &g_ServiceStatus);
		if (FALSE == bServiceStatus)
		{
			g_Log.WriteErrorMessage("Service Status Initial Setup Failed", true);
		}
		else
		{
			g_Log.WriteMessage("Service Status initial Setup Success");
		}

		// Call ServiceInit Function
		ServiceInit(dwArgc, lpArgv);

		// Create additional threads (CThreadProcess and CMemoryThread)
		if (g_Config.GetProcessThreadStatus() == 1)
		{
			printf("%d", g_Config.GetProcessThreadStatus());
			// Assign a Stop Event to future process thread
			processThread.SetStopEvent(g_hServiceStopEvent);
			processThread.SetPeriod(g_Config.GetProcessThreadPeriod());
			printf("%d", g_Config.GetProcessThreadPeriod());

			// Create process thread
			if (processThread.CreateAThread())
			{
				g_Log.WriteMessage("CThreadProcess Created");
			}
			else
			{
				g_Log.WriteErrorMessage("Create CThreadProcess has Failed", true);
			}
		}
		if (g_Config.GetMemoryThreadStatus() == 1)
		{
			// Assign a Stop Event to future memory thread
			memoryThread.SetStopEvent(g_hServiceStopEvent);
			memoryThread.SetPeriod(g_Config.GetMemoryThreadPeriod());

			// Create memory thread
			if (memoryThread.CreateAThread())
			{
				g_Log.WriteMessage("CThreadMemory Created");
			}
			else
			{
				g_Log.WriteErrorMessage("Create CMemoryThread has Failed", true);
			}
		}
		cpuThread.SetStopEvent(g_hServiceStopEvent);

		// Create process thread
		if (cpuThread.CreateAThread())
		{
			g_Log.WriteMessage("CThreadCpu Created");
		}
		else
		{
			g_Log.WriteErrorMessage("Create CThreadCpu has Failed", true);
		}

		// ResetEvent
		if (g_hServiceStopEvent != NULL)
		{
			if (ResetEvent(g_hServiceStopEvent) != 0)
			{
				g_Log.WriteMessage("ResetStopEvent Success");
			}
			else
			{
				g_Log.WriteErrorMessage("ResetStopEvent Failed", true);
			}
		}

		// Check Whether to stop the Service
		// WaitforSingleObject Which wait event to be Signaled
		if (WaitForSingleObject(g_hServiceStopEvent, INFINITE) != WAIT_OBJECT_0)
		{
			// Stop the service
			// Send Report status to SCM
			ServiceReportStatus(SERVICE_STOPPED, NO_ERROR, 0);
		}

		// Close the Handle for Create Event
		if (g_hServiceStopEvent != NULL)
		{
			if (CloseHandle(g_hServiceStopEvent) != 0)
			{
				g_Log.WriteMessage("StopEvent handle has Closed Successfully!");
				g_hServiceStopEvent = NULL;
			}
			else
			{
				g_Log.WriteErrorMessage("Close StopEvent handle Failed", true);
			}
		}

		// Check whether threads Stopped or not
		if (g_Config.GetProcessThreadStatus() == 1)
		{
			if (processThread.IsRunning())
			{
				g_Log.WriteMessage("CThreadProcess is Running");

				if (processThread.StopThread())
				{
					g_Log.WriteMessage("CThreadProcess::StopThread Success");
				}
				else
				{
					g_Log.WriteErrorMessage("CThreadProcess::StopThread Failed", true);
				}
			}
			else
			{
				g_Log.WriteMessage("CThreadProcess isn't Running");
			}
		}

		if (g_Config.GetMemoryThreadStatus() == 1)
		{
			if (memoryThread.IsRunning())
			{
				g_Log.WriteMessage("CThreadMemory is Running");

				if (memoryThread.StopThread())
				{
					g_Log.WriteMessage("CThreadMemory::StopThread Success");
				}
				else
				{
					g_Log.WriteErrorMessage("CThreadMemory::StopThread Failed", true);
				}
			}
			else
			{
				g_Log.WriteMessage("CThreadMemory isn't Running");
			}
		}
		if (cpuThread.StopThread())
		{
			g_Log.WriteMessage("CThreadCpu::StopThread Success");
		}
		else
		{
			g_Log.WriteErrorMessage("CThreadCPU::StopThread Failed", true);
		}
	}
	g_Log.WriteMessage("ServiceMain End");
}
std::string GetExePath()
{
	std::string sPath;
	char szBuffer[MAX_PATH] = { 0 };
	const DWORD cdwCheckPath = GetModuleFileName(NULL, szBuffer, MAX_PATH);
	if (0 != cdwCheckPath)
	{
		sPath = szBuffer;
	}
	return sPath;
}

std::string GetDirPath()
{
	std::string sDirPath;
	const std::string csAllPath = GetExePath();
	const size_t cnPos = csAllPath.find_last_of("/\\");
	if (cnPos != std::string::npos)
	{
		sDirPath = csAllPath.substr(0, cnPos);
	}
	return sDirPath;
}

std::string GetFullIniPath(const std::string & csIniFilePath, const std::string & csIniFileName)
{
	const std::string csFullIniFilePath = csIniFilePath + csIniFileName;
	return csFullIniFilePath;
}

bool ServiceInstall(void)
{
	bool bRet = false;
	g_Log.WriteMessage("ServiceInstall Start");

	// Local Variable Definitions
	SC_HANDLE hScOpenSCManager = NULL;
	SC_HANDLE hScCreateService = NULL;

	do
	{
		// Open the Service Control Manager
		hScOpenSCManager = OpenSCManager(
			NULL, // Local Machine
			NULL, // By default Database i.e. SERVICES_ACTIVE_DATABASE
			SC_MANAGER_ALL_ACCESS);
		if (NULL == hScOpenSCManager)
		{
			g_Log.WriteErrorMessage("OpenSCManager Failed", true);
			break;
		}
		g_Log.WriteMessage("OpenSCManager Success");

		// Create the Service
		hScCreateService = CreateService(
			hScOpenSCManager,
			SERVICE_NAME,
			SERVICE_NAME,
			SERVICE_ALL_ACCESS,
			SERVICE_WIN32_OWN_PROCESS,
			SERVICE_AUTO_START,
			SERVICE_ERROR_NORMAL,
			GetExePath().c_str(),
			NULL,
			NULL,
			NULL,
			NULL,
			NULL);
		if (NULL == hScCreateService)
		{
			g_Log.WriteErrorMessage("CreateService Failed", true);
			break;
		}
		g_Log.WriteMessage("CreateService Success");

		bRet = true;
	} while (false);

	// Close the Handle for OpenSCManager and Create Service
	if (hScCreateService != NULL)
	{
		CloseServiceHandle(hScCreateService);
	}
	if (hScOpenSCManager != NULL)
	{
		CloseServiceHandle(hScOpenSCManager);
	}
	g_Log.WriteMessage("ServiceInstall End");

	return bRet;
}

bool ServiceDelete(void)
{
	bool bRet = false;
	g_Log.WriteMessage("ServiceDelete Start");
	// Local Variable Definitions
	SC_HANDLE hScOpenSCManager = NULL;
	SC_HANDLE hScOpenService = NULL;
	BOOL bDeleteService = FALSE;
	do
	{
		// Open the Service Control Manager
		hScOpenSCManager = OpenSCManager(NULL, NULL, SC_MANAGER_ALL_ACCESS);
		if (NULL == hScOpenSCManager)
		{
			g_Log.WriteErrorMessage("OpenSCManager Failed", true);
			break;
		}
		g_Log.WriteMessage("OpenSCManager Success");

		// Open the Service
		hScOpenService = OpenService(hScOpenSCManager, SERVICE_NAME, SERVICE_ALL_ACCESS);

		if (NULL == hScOpenService)
		{
			g_Log.WriteErrorMessage("OpenService Failed", true);
			break;
		}
		g_Log.WriteMessage("OpenService Success");

		// Delete Service
		bDeleteService = DeleteService(hScOpenService);
		if (FALSE == bDeleteService)
		{
			g_Log.WriteErrorMessage("Delete Service Failed", true);
			break;
		}
		g_Log.WriteMessage("ServiceDelete End");

		bRet = true;
	} while (false);

	// Close the Handle for SCM and OpenService
	if (hScOpenService != NULL)
	{
		CloseServiceHandle(hScOpenService);
	}
	if (hScOpenSCManager != NULL)
	{
		CloseServiceHandle(hScOpenSCManager);
	}

	return bRet;
}

int main(int argc, CHAR * argv[])
{
	g_Config.ReadFromIni(GetFullIniPath(GetDirPath(), g_cszIniFileName));
	g_Config.ShowIniInfo();
	//strcpy(ServiceName, g_Config.GetServiceName().c_str()); 
	g_Log.InitLogFile(GetDirPath(), g_cszLogFileName, argc == 1);
	BOOL bStServiceCtrlDispatcher = FALSE;
	if (argc > 1)
	{
		if (_stricmp(argv[1], "-install") == 0)
		{
			if (ServiceInstall())
			{
				g_Log.WriteMessage("Service successfully installed");
			}
			else
			{
				g_Log.WriteErrorMessage("Failed to install the service", true);
			}
		}

		else if (_stricmp(argv[1], "-uninstall") == 0)
		{
			if (ServiceDelete())
			{
				g_Log.WriteMessage("Service successfully deleted");
			}
			else
			{
				g_Log.WriteErrorMessage("Failed to delete the service", true);
			}
		}

		else if (argc != 2 || (strcmp(argv[1], "-install") != 0
			&& strcmp(argv[1], "-uninstall") != 0))
		{
			g_Log.WriteWarningMessage("Unrecognized command!", false);
			g_Log.WriteMessage("Use -install for service installation or -uninstall for deletion");
		}
	}

	else
	{
		// Fill the Service Table Entry (2D Array)
		SERVICE_TABLE_ENTRY DispatchTable[] =
		{
			{_strdup(SERVICE_NAME), (LPSERVICE_MAIN_FUNCTION)ServiceMain},
			{NULL, NULL}
		};

		// Start Service Control Dispatcher
		bStServiceCtrlDispatcher = StartServiceCtrlDispatcher(DispatchTable);
		if (FALSE == bStServiceCtrlDispatcher)
		{
			g_Log.WriteWarningMessage("StartServiceCtrlDispatcher Failed", true);
		}
		else
		{
			g_Log.WriteMessage("StartServiceCtrlDispatcher Success");
		}
	}
	g_Log.CloseLogFiLe();
	return 0;
}


