#include "stdafx.h"
#include "config_file.h"

const char g_cszTheadsKeyStatus[] = "active";
const char g_cszThreadKeyPeriod[] = "periodInSec";

std::string SetSectionName(const int ciSectionName)
{
	std::string sSectionName;
	switch (ciSectionName)
	{
	case 1: // if ciSectionName == 1
	{
		// define section name
		sSectionName = "service";
		break;
	}
	case 2: // if ciSectionName == 2
	{
		// define section name
		sSectionName = "memorythread";
		break;
	}
	case 3:
		sSectionName = "processthread";
	default:
		break;
	}

	return sSectionName;
}

CConfigFile::CConfigFile()
{
}
CConfigFile::~CConfigFile()
{
}

std::string CConfigFile::GetStringIniValue(const int ciSectionName, const std::string & csKeyName,
	const std::string & csDefaultValue)
{
	std::string sValue;
	char szServiceBuffer[1000] = { 0 };
	int iValue = GetPrivateProfileString((SetSectionName(ciSectionName)).c_str(), csKeyName.c_str(),
		csDefaultValue.c_str(), szServiceBuffer, sizeof(szServiceBuffer), m_sFullIniFilePath.c_str());

	// Check if file has been found
	if (iValue != 0x2)
	{
		sValue = szServiceBuffer;
	}

	return sValue;
}

int CConfigFile::FormatString(const std::string & sValue)
{
	int ciValue = std::stoi(sValue.c_str());
	return ciValue;
}

CReadFromConfig::CReadFromConfig()
{
}
CReadFromConfig::~CReadFromConfig()
{
}

const char * CReadFromConfig::GetServiceName()
{
	const char * ServiceName = m_sServiceName.c_str();
	return ServiceName;
}

std::string CReadFromConfig::GetServiceDispName()
{
	return m_sServiceDispName;
}

int CReadFromConfig::GetLogLevel()
{
	int iLogLevel = m_Config.FormatString(m_sLogLevel.c_str());
	return iLogLevel;
}

int CReadFromConfig::GetMemoryThreadStatus()
{
	int iThreadStatus = m_Config.FormatString(m_sMemoryThreadStatus.c_str());
	return iThreadStatus;
}

int CReadFromConfig::GetMemoryThreadPeriod()
{
	int iThreadPeriod = m_Config.FormatString(m_sMemoryThreadPeriod.c_str());
	return iThreadPeriod;
}

int CReadFromConfig::GetProcessThreadStatus()
{
	int iThreadStatus = m_Config.FormatString(m_sProcessThreadStatus.c_str());
	return iThreadStatus;
}

int CReadFromConfig::GetProcessThreadPeriod()
{
	int iThreadPeriod = m_Config.FormatString(m_sProcessThreadPeriod.c_str());
	return iThreadPeriod;
}

void CReadFromConfig::ReadFromIni(const std::string & csFullIniFilePath)
{
	m_Config.SetIniFilePath(csFullIniFilePath);
	m_sServiceName = m_Config.GetStringIniValue(1, "ServiceName", "Windows_Service");
	m_sServiceDispName = m_Config.GetStringIniValue(1, "ServiceDisplayName", "Windows_Service");
	m_sLogLevel = m_Config.GetStringIniValue(1, "LogLevel", "1");
	m_sMemoryThreadStatus = m_Config.GetStringIniValue(2, g_cszTheadsKeyStatus, "1");
	m_sMemoryThreadPeriod = m_Config.GetStringIniValue(2, "periodInSec", "5");
	m_sProcessThreadStatus = m_Config.GetStringIniValue(3, g_cszTheadsKeyStatus, "1");
	m_sProcessThreadPeriod = m_Config.GetStringIniValue(3, "periodInSec", "5");
}

void CReadFromConfig::ShowIniInfo()
{
	std::string sServiceNameMessage = m_sServiceName + "	Service Name";
	m_Log.WriteMessage(sServiceNameMessage.c_str());
	std::string sServiceDispMessage = m_sServiceDispName + "	Service Display Name";
	m_Log.WriteMessage(sServiceDispMessage.c_str());
	m_Log.WriteMessage(m_sLogLevel.append("	Logging Level").c_str());
	m_Log.WriteMessage(m_sMemoryThreadStatus.append("	Memory Thread Status").c_str());
	m_Log.WriteMessage(m_sMemoryThreadPeriod.append("	Memory Thread Period").c_str());
	m_Log.WriteMessage(m_sProcessThreadStatus.append("	Process Thread Status").c_str());
	m_Log.WriteMessage(m_sProcessThreadPeriod.append("	Process thread Period").c_str());
}

