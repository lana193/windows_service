#ifndef _LOGFILE_H__INCLUDED_
#define _LOGFILE_H__INCLUDED_

#include <string>
#include <ctime>

class ILogger
{
public:
	virtual bool InitLogFile(const std::string & csLogFilePath, const std::string & csFileName) = 0;
	virtual void WriteMessage(const char * szcMessage) = 0;
	virtual void WriteErrorMessage(const char * szcMessage, const bool cbGetError) = 0;
	virtual void WriteWarningMessage(const char * szcMessage, const bool cbGetError) = 0;
	virtual bool CloseLogFiLe() = 0;
};

class CWriteToLogFile : public ILogger
{
public:
	CWriteToLogFile();
	~CWriteToLogFile();

	virtual bool InitLogFile(const std::string & csLogFilePath, const std::string & csFileName);
	virtual void WriteMessage(const char * szcMessage);
	virtual void WriteErrorMessage(const char * szcMessage, const bool cbGetError);
	virtual void WriteWarningMessage(const char * szcMessage, const bool cbGetError);
	virtual bool CloseLogFiLe();

private:
	FILE * m_pLogFile;
	HANDLE m_hMutex;

	std::string WriteTimeToLogFile()
	{
		const time_t cTimeLine = time(NULL);
		struct tm TimeInfo;
		char szBuffer[26] = { 0 };
		// localtime_s, Microsoft version (returns zero on success, an error code on failure)
		const errno_t cErr = localtime_s(&TimeInfo, &cTimeLine);
		if (cErr == 0)
		{
			strftime(szBuffer, sizeof(szBuffer), "%Y-%m-%d %H:%M:%S", &TimeInfo);
		}
		return std::string(szBuffer);
	}
};

class CWriteToConsole : public ILogger
{
public:
	CWriteToConsole();
	~CWriteToConsole();

	virtual bool InitLogFile(const std::string & csLogFilePath, const std::string & csFileName)
	{
		return true;
	}
	virtual void WriteMessage(const char * szcMessage);
	virtual void WriteErrorMessage(const char * szcMessage, const bool cbGetError);
	virtual void WriteWarningMessage(const char * szcMessage, const bool cbGetError);
	virtual bool CloseLogFiLe()
	{
		return true;
	}
};

class CLogger
{
public:
	CLogger();
	~CLogger();

	bool InitLogFile(const std::string & csLogFilePath, const std::string & csFileName,
		const bool cbFile);
	void WriteMessage(const char * szcMessage);
	void WriteErrorMessage(const char * szcMessage, const bool cbGetError);
	void WriteWarningMessage(const char * szcMessage, const bool cbGetError);
	void CloseLogFiLe();
private:
	bool m_bFile;
	int m_iLogLevel;
	CWriteToLogFile m_FileLog;
	CWriteToConsole m_ConsoleLog;
};

extern CLogger g_Log;

#endif //_LOGFILE_H__INCLUDED_

