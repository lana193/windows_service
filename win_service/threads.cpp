#include "stdafx.h"
#include "threads.h"

CWindowsServiceThread::CWindowsServiceThread() :
	m_hThread(NULL)
{
}

CWindowsServiceThread::~CWindowsServiceThread()
{
}

bool CWindowsServiceThread::CreateAThread()
{
	bool bRet = false;

	// Create a thread
	m_hThread = CreateThread(NULL, 0, ThreadFunction, (void*)this, NULL, NULL);

	// Check if Thread was created
	if (m_hThread != NULL)
	{
		bRet = true;
	}
	return bRet;
}

bool CWindowsServiceThread::IsRunning()
{
	DWORD dwExitCode = 0;
	if (m_hThread != NULL)
	{
		GetExitCodeThread(m_hThread, &dwExitCode);
		if (dwExitCode == STILL_ACTIVE)
		{
			return true;
		}
	}
	return false;
}

bool CWindowsServiceThread::StopThread()
{
	DWORD dwExitCode = 0;
	bool bRet = false;

	if (m_hThread != NULL)
	{
		GetExitCodeThread(m_hThread, &dwExitCode);
		if (dwExitCode == 0)
		{
			if (dwExitCode == NO_ERROR)
			{
				g_Log.WriteMessage("Thread has Stopped Successfully");
				bRet = true;
			}
			else
			{
				g_Log.WriteWarningMessage("StopThread failed", true);
			}
		}
		else if (dwExitCode == STILL_ACTIVE)
		{
			if (TerminateThread(m_hThread, dwExitCode) != 0)
			{
				g_Log.WriteWarningMessage("The thread has Terminated Unsafely", false);
				bRet = true;
			}
			else
			{
				g_Log.WriteWarningMessage("TerminateThread failed", true);
			}
		}

		if (CloseHandle(m_hThread) != 0)
		{
			g_Log.WriteMessage("Thread handle has Closed Successfully!");
			m_hThread = NULL;
		}
		else
		{
			g_Log.WriteErrorMessage("Close thread handle failed", true);
		}
	}
	return bRet;
}

