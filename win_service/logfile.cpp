#include "stdafx.h"
#include <stdio.h>

CWriteToLogFile::CWriteToLogFile() :
	m_pLogFile(NULL),
	m_hMutex(NULL)
{
}

CWriteToLogFile::~CWriteToLogFile()
{
}

bool CWriteToLogFile::InitLogFile(const std::string & csLogFilePath,
	const std::string & csLogFileName)
{
	const std::string csFullLogFilePath = csLogFilePath + csLogFileName;
	if (fopen_s(&m_pLogFile, csFullLogFilePath.c_str(), "a+") != 0)
	{
		return false;
	}
	m_hMutex = CreateMutex(NULL, FALSE, "Mutex_m_pLogFile");
	return true;
}

void CWriteToLogFile::WriteMessage(const char * szcMessage)
{
	if (m_pLogFile != NULL && szcMessage != NULL && m_hMutex != NULL)
	{
		WaitForSingleObject(m_hMutex, INFINITE);
		fprintf_s(m_pLogFile, "%s	%s\n", WriteTimeToLogFile().c_str(), szcMessage);
		ReleaseMutex(m_hMutex);
	}
}

void CWriteToLogFile::WriteErrorMessage(const char * szcMessage, const bool cbGetError = false)
{
	WaitForSingleObject(m_hMutex, INFINITE);
	if (cbGetError)
	{
		const int ciError = GetLastError();
		fprintf_s(m_pLogFile, "%s	ERROR !	%s	LastErrorCode = %d\n",
			WriteTimeToLogFile().c_str(), szcMessage, ciError);
	}
	else
	{
		fprintf_s(m_pLogFile, "%s	ERROR !	%s\n", WriteTimeToLogFile().c_str(), szcMessage);
	}
	ReleaseMutex(m_hMutex);

}

void CWriteToLogFile::WriteWarningMessage(const char * szcMessage, const bool cbGetError = false)
{
	if (m_pLogFile != NULL && szcMessage != NULL && m_hMutex != NULL)
	{
		WaitForSingleObject(m_hMutex, INFINITE);
		if (cbGetError)
		{
			const int ciError = GetLastError();
			fprintf_s(m_pLogFile, "%s	WARNING !	%s	LastErrorCode = %d\n",
				WriteTimeToLogFile().c_str(), szcMessage, ciError);
		}
		else
		{
			fprintf_s(m_pLogFile, "%s	WARNING !	%s\n",
				WriteTimeToLogFile().c_str(), szcMessage);
		}
		ReleaseMutex(m_hMutex);
	}
}

bool CWriteToLogFile::CloseLogFiLe()
{
	bool bRet = false;
	if (m_hMutex != NULL)
	{
		CloseHandle(m_hMutex);
		m_hMutex = NULL;
	}
	if (m_pLogFile != NULL)
	{
		bRet = (fclose(m_pLogFile) == NULL);
		m_pLogFile = NULL;
	}
	else
	{
		bRet = true;
	}
	return bRet;
}

CWriteToConsole::CWriteToConsole()
{
}

CWriteToConsole::~CWriteToConsole()
{
}

void CWriteToConsole::WriteMessage(const char * szcMessage)
{
	if (szcMessage != NULL)
	{
		printf_s("%s\n", szcMessage);
	}
}

void CWriteToConsole::WriteErrorMessage(const char * szcMessage, const bool cbGetError)
{
	if (szcMessage != NULL)
	{
		if (cbGetError)
		{
			const int ciError = GetLastError();
			printf_s("ERROR !	%s	LastError code = %d\n", szcMessage, ciError);
		}
		else
		{
			printf_s("ERROR !	%s\n", szcMessage);
		}
	}
}

void CWriteToConsole::WriteWarningMessage(const char * szcMessage, const bool cbGetError = false)
{
	if (szcMessage != NULL)
	{
		if (cbGetError)
		{
			const int ciError = GetLastError();
			printf_s("WARNING !	%s	LastErrorCode = %d\n", szcMessage, ciError);
		}
		else
		{
			printf_s("WARNING !	%s\n", szcMessage);
		}
	}
}

CLogger::CLogger() :
	m_bFile(false),
	m_iLogLevel(1)
{
}

CLogger::~CLogger()
{
}

bool CLogger::InitLogFile(const std::string & csLogFilePath, const std::string & csFileName,
	const bool cbFile)
{
	bool bRet = false;
	m_bFile = cbFile;
	if (m_bFile)
	{
		bRet = m_FileLog.InitLogFile(csLogFilePath, csFileName);
	}
	else
	{
		bRet = m_ConsoleLog.InitLogFile(csLogFilePath, csFileName);
	}
	return bRet;
}

void CLogger::WriteMessage(const char * szcMessage)
{
	if (m_bFile)
	{
		m_FileLog.WriteMessage(szcMessage);
	}
	else
	{
		m_ConsoleLog.WriteMessage(szcMessage);
	}
}

void CLogger::WriteErrorMessage(const char * szcMessage, const bool cbGetError = false)
{
	if (m_bFile)
	{
		m_FileLog.WriteErrorMessage(szcMessage, cbGetError);
	}
	else
	{
		m_ConsoleLog.WriteErrorMessage(szcMessage, cbGetError);
	}
}

void CLogger::WriteWarningMessage(const char * szcMessage, const bool cbGetError = false)
{
	if (m_bFile)
	{
		if (m_iLogLevel == 1)
		{
			m_FileLog.WriteWarningMessage(szcMessage, cbGetError);
		}
	}
	else
	{
		m_ConsoleLog.WriteWarningMessage(szcMessage, cbGetError);
	}
}

void CLogger::CloseLogFiLe()
{
	if (m_bFile)
	{
		m_FileLog.CloseLogFiLe();
	}
	else
	{
		m_ConsoleLog.CloseLogFiLe();
	}
}

