#ifndef _THREADS_H__INCLUDED_
#define _THREADS_H__INCLUDED_

class CWindowsServiceThreadI
{
public:
	virtual bool CreateAThread() = 0;
	virtual bool IsRunning() = 0;
	virtual bool StopThread() = 0;
	virtual void SetStopEvent(HANDLE hStopEvent) = 0;
	virtual HANDLE GetStopEvent() const = 0;
	virtual DWORD Run() = 0;
};

class CWindowsServiceThread : public CWindowsServiceThreadI
{
public:
	CWindowsServiceThread();
	~CWindowsServiceThread();

	virtual bool CreateAThread();
	virtual bool IsRunning();
	virtual bool StopThread();
	virtual void SetStopEvent(HANDLE hStopEvent) = 0;
	virtual HANDLE GetStopEvent() const = 0;
	virtual DWORD Run() = 0;
protected:
	HANDLE m_hThread;

	static DWORD WINAPI ThreadFunction(void * pParam)
	{
		DWORD dwRes = 0;
		g_Log.WriteMessage("Inside Thread function");
		CWindowsServiceThread * pThread = reinterpret_cast<CWindowsServiceThread*>(pParam);
		if (pThread != NULL)
		{
			dwRes = pThread->Run();
		}
		return dwRes;
	}
};

#endif //_THREADS_H__INCLUDED_


