#ifndef _CONFIG_FILE_H__INCLUDED_
#define _CONFIG_FILE_H__INCLUDED_

class IConfigFile
{
public:
	virtual void SetIniFilePath(const std::string & csFullIniFilePath) = 0;
	virtual std::string GetIniFilePath() const = 0;
	virtual std::string GetStringIniValue(const int ciSectionName, const std::string & csKeyName,
		const std::string & csDefaultValue) = 0;

	virtual int FormatString(const std::string & sValue) = 0;

};

class CConfigFile : public IConfigFile
{
public:
	CConfigFile();
	~CConfigFile();

	virtual void SetIniFilePath(const std::string & csFullIniFilePath)
	{
		m_sFullIniFilePath = csFullIniFilePath;
	}

	virtual std::string GetIniFilePath() const
	{
		return m_sFullIniFilePath;
	}

	virtual std::string GetStringIniValue(const int ciSectionName, const std::string & csKeyName,
		const std::string & csDefaultValue);

	virtual int FormatString(const std::string & sValue);

protected:
	std::string m_sFullIniFilePath;
};

class CReadFromConfig : public CConfigFile
{
public:
	CReadFromConfig();
	~CReadFromConfig();

	void ReadFromIni(const std::string & csFullIniFilePath);
	void ShowIniInfo();

	const char * GetServiceName();
	std::string GetServiceDispName();
	int GetLogLevel();
	int GetMemoryThreadStatus();
	int GetMemoryThreadPeriod();
	int GetProcessThreadStatus();
	int GetProcessThreadPeriod();
private:
	std::string m_sServiceName;
	std::string m_sServiceDispName;
	std::string m_sLogLevel;
	std::string m_sMemoryThreadStatus;
	std::string m_sMemoryThreadPeriod;
	std::string m_sProcessThreadStatus;
	std::string m_sProcessThreadPeriod;

	CConfigFile m_Config;
	CLogger m_Log;
};

#endif //_CONFIG_FILE_H__INCLUDED_


