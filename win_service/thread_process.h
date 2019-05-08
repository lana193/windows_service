#include "threads.h"

class CThreadProcess : public CWindowsServiceThread
{
public:
	CThreadProcess();
	~CThreadProcess();

	virtual DWORD Run();
	virtual void SetStopEvent(HANDLE hStopEvent)
	{
		m_hStopEvent = hStopEvent;
	}

	virtual HANDLE GetStopEvent() const
	{
		return m_hStopEvent;
	}

	void SetPeriod(const int ciPeriod)
	{
		m_iPeriod = ciPeriod * 1000;
	}

	int GetPeriod() const
	{
		return m_iPeriod;
	}

private:
	HANDLE m_hStopEvent;
	int m_iPeriod;
};


