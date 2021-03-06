#include "MainConfig.h"

CMainConfig::CMainConfig(void)
{
	m_szError[0] = '\0';

	m_u4MsgHighMark           = 0;
	m_u4MsgLowMark            = 0;
	m_u4MsgThreadCount        = 0;
	m_u4MsgMaxQueue           = 0;
	m_nEncryptFlag            = 0;
	m_nEncryptOutFlag         = 0;
	m_u4ReactorCount          = 3;
	m_u4SendTimeout           = MAX_QUEUE_TIMEOUT;
	m_u4RecvBuffSize          = 0;
	m_u2ThreadTimuOut         = 0;
	m_u2ThreadTimeCheck       = 0;
	m_u2PacketTimeOut         = 0;
	m_u2SendAliveTime         = 0;
	m_u2HandleCount           = 0;
	m_u2MaxHanderCount        = 0;
	m_u2MaxConnectTime        = 0;
	m_u1CommandAccount        = 0;
	m_u1ServerType            = 0;
	m_u1Debug                 = DEBUG_OFF;
	m_u1Monitor               = 0;
	m_u4SendDatamark          = MAX_BLOCK_SIZE;
	m_u4CoreFileSize          = 0;
	m_u2TcpNodelay            = TCP_NODELAY_ON;

	m_u1ConsoleSupport        = 0;
	m_nConsolePort            = 0;
	m_u1ConsoleIPType         = TYPE_IPV4; 
	m_u4ConnectServerTimerout = 0;
	m_u2ConnectServerCheck    = CONNECT_LIMIT_RETRY;
	m_u4ConnectServerRecvBuff = MAX_BUFF_1024;
	m_u4ServerRecvBuff        = MAX_BUFF_1024;

	m_u2ValidConnectCount   = MAX_CONNECT_COUNT;
	m_u1Valid               = 1;
	m_u4ValidPacketCount    = 0;
	m_u4ValidRecvSize       = 0;
	m_u2ForbiddenTime       = 0;
	m_u2RecvQueueTimeout    = MAX_QUEUE_TIMEOUT;
	m_u2SendQueueTimeout    = MAX_QUEUE_TIMEOUT;
	m_u2SendQueueCount      = SENDQUEUECOUNT;
	m_u2SendQueuePutTime    = (uint16)MAX_MSG_PUTTIMEOUT;
	m_u2WorkQueuePutTime    = (uint16)MAX_MSG_PUTTIMEOUT;
	m_u2Backlog             = (uint16)MAX_ASYNCH_BACKLOG;
	m_u4TrackIPCount        = (uint32)MAX_ASYNCH_BACKLOG;

	//默认的CPU监控和内存监控的上限
	m_u4MaxCpu              = 90;
	m_u4MaxMemory           = 2000;

	m_u1CommandFlow         = 0;

	m_u1NetworkMode         = (uint8)NETWORKMODE_PRO_IOCP;

	m_szServerName[0]         = '\0';
	m_szServerVersion[0]      = '\0';
	m_szPacketVersion[0]      = '\0';
	m_szModulePath[0]         = '\0';
	m_szResourceName[0]       = '\0';
	m_szEncryptPass[0]        = '\0';

	m_szWindowsServiceName[0] = '\0';
	m_szDisplayServiceName[0] = '\0';

	m_u1WTAI                  = (uint8)0;   //AI默认为关闭
	m_u4WTCheckTime           = 0;
	m_u4WTTimeoutCount        = 0;
	m_u4WTStopTime            = 0;
	m_u1WTReturnDataType      = 0;
	m_szWTReturnData[0]       = '\0';
	m_blByteOrder             = false;      //默认为主机字序

	m_u1DebugTrunOn           = 0;          //0为关闭，1为打开
	m_szDeubgFileName[0]      = '\0';
	m_u4ChkInterval           = 600;
	m_u4LogFileMaxSize        = MAX_BUFF_1024*10;
	m_u4LogFileMaxCnt         = 3;
	m_szDebugLevel[0]         = '\0';

	//判定字节序
	if(O32_HOST_ORDER == O32_LITTLE_ENDIAN)
	{
		m_u1CharOrder = SYSTEM_LITTLE_ORDER;
		OUR_DEBUG((LM_INFO, "[CMainConfig::CMainConfig]SYSYTEM SYSTEM_LITTLE_ORDER.\n"));
	}
	else
	{
		m_u1CharOrder = SYSTEM_BIG_ORDER;
		OUR_DEBUG((LM_INFO, "[CMainConfig::CMainConfig]SYSYTEM SYSTEM_BIG_ORDER.\n"));
	}
}

CMainConfig::~CMainConfig(void)
{
	OUR_DEBUG((LM_INFO, "[CMainConfig::~CMainConfig].\n"));
	m_vecServerInfo.clear();
	m_vecUDPServerInfo.clear();
	OUR_DEBUG((LM_INFO, "[CMainConfig::~CMainConfig]End.\n"));
}

const char* CMainConfig::GetError()
{
	return m_szError;
}

bool CMainConfig::Init(const char* szConfigPath)
{
	//获得数据解析包的版本号
	CPacketParse objPacketParse;
	sprintf_safe(m_szPacketVersion, MAX_BUFF_20, "%s", objPacketParse.GetPacketVersion());

	char* pData = NULL;
	OUR_DEBUG((LM_INFO, "[CMainConfig::Init]Filename = %s.\n", szConfigPath));
	if(false == m_MainConfig.Init(szConfigPath))
	{
		OUR_DEBUG((LM_INFO, "[CMainConfig::Init]File Read Error = %s.\n", szConfigPath));
		return false;
	}

	//获得当前网络模型
	pData = m_MainConfig.GetData("NetWorkMode", "Mode");
	if(NULL != pData)
	{
		if(ACE_OS::strcmp(pData, "Iocp") == 0)
		{
			m_u1NetworkMode = (uint8)NETWORKMODE_PRO_IOCP;
		}
		else if(ACE_OS::strcmp(pData, "Select") == 0)
		{
			m_u1NetworkMode = (uint8)NETWORKMODE_RE_SELECT;
		}
		else if(ACE_OS::strcmp(pData, "Poll") == 0)
		{
			m_u1NetworkMode = (uint8)NETWORKMODE_RE_TPSELECT;
		}
		else if(ACE_OS::strcmp(pData, "Epoll") == 0)
		{
			m_u1NetworkMode = (uint8)NETWORKMODE_RE_EPOLL;
		}
		else
		{
			OUR_DEBUG((LM_INFO, "[CMainConfig::Init]NetworkMode is Invalid!!, please read main.xml desc.\n", szConfigPath));
			return false;
		}
	}
	else
	{
		OUR_DEBUG((LM_INFO, "[CMainConfig::Init]NetworkMode is Invalid!!, please read main.xml desc.\n", szConfigPath));
		return false;
	}

	pData = m_MainConfig.GetData("NetWorkMode", "BackLog");
	if(NULL != pData)
	{
		m_u2Backlog = (uint16)ACE_OS::atoi(pData);
	}

	pData = m_MainConfig.GetData("NetWorkMode", "ByteOrder");
	if(NULL != pData)
	{
		if(ACE_OS::strcmp(pData, "NET_ORDER") == 0)
		{
			m_blByteOrder = true;
		}
		else
		{
			m_blByteOrder = false;
		}
	}

	//获得服务器基础属性
	pData = m_MainConfig.GetData("ServerType", "Type");
	if(NULL != pData)
	{
		m_u1ServerType = (uint8)ACE_OS::atoi(pData);
	}

	pData = m_MainConfig.GetData("ServerType", "Debug");
	if(NULL != pData)
	{
		m_u1Debug = (uint8)ACE_OS::atoi(pData);
	}

	pData = m_MainConfig.GetData("ServerType", "name");
	if(NULL != pData)
	{
		sprintf_safe(m_szWindowsServiceName, MAX_BUFF_50, "%s", pData);
	}

	pData = m_MainConfig.GetData("ServerType", "displayname");
	if(NULL != pData)
	{
		sprintf_safe(m_szDisplayServiceName, MAX_BUFF_50, "%s", pData);
	}

	pData = m_MainConfig.GetData("ServerID", "id");
	if(NULL != pData)
	{
		m_nServerID = ACE_OS::atoi(pData);
	}

	pData = m_MainConfig.GetData("ServerName", "name");
	if(NULL != pData)
	{
		sprintf_safe(m_szServerName, MAX_BUFF_20, "%s", pData);
	}

	pData = m_MainConfig.GetData("ServerVersion", "Version");
	if(NULL != pData)
	{
		sprintf_safe(m_szServerVersion, MAX_BUFF_20, "%s", pData);
	}

	//获得监听端口信息
	_ServerInfo serverinfo;

	m_vecServerInfo.clear();
	TiXmlElement* pNextTiXmlElementIP     = NULL;
	TiXmlElement* pNextTiXmlElementPort   = NULL;
	TiXmlElement* pNextTiXmlElementIpType = NULL;
	while(true)
	{
		pData = m_MainConfig.GetData("TCPServerIP", "ip", pNextTiXmlElementIP);
		if(pData != NULL)
		{
			sprintf_safe(serverinfo.m_szServerIP, MAX_BUFF_50, "%s", pData);
		}
		else
		{
			break;
		}

		pData = m_MainConfig.GetData("TCPServerIP", "port", pNextTiXmlElementPort);
		if(pData != NULL)
		{
			serverinfo.m_nPort = ACE_OS::atoi(pData);
		}
		else
		{
			break;
		}

		pData = m_MainConfig.GetData("TCPServerIP", "ipType", pNextTiXmlElementIpType);
		if(pData != NULL)
		{
			if(ACE_OS::strcmp(pData, "IPV6") == 0)
			{
				serverinfo.m_u1IPType = TYPE_IPV6;
			}
			else
			{
				serverinfo.m_u1IPType = TYPE_IPV4;
			}
		}
		else
		{
			break;
		}

		m_vecServerInfo.push_back(serverinfo);
	}

	//开始获得消息处理线程参数
	pData = m_MainConfig.GetData("Message", "Msg_High_mark");
	if(pData != NULL)
	{
		m_u4MsgHighMark = (uint32)ACE_OS::atoi(pData);
	}
	pData = m_MainConfig.GetData("Message", "Msg_Low_mark");
	if(pData != NULL)
	{
		m_u4MsgLowMark = (uint32)ACE_OS::atoi(pData);
	}
	pData = m_MainConfig.GetData("Message", "Msg_Thread");
	if(pData != NULL)
	{
		m_u4MsgThreadCount = (uint32)ACE_OS::atoi(pData);
	}

	pData = m_MainConfig.GetData("Message", "Msg_MaxQueue");
	if(pData != NULL)
	{
		m_u4MsgMaxQueue = (uint32)ACE_OS::atoi(pData);
	}

	//开始获得UDP服务器相关参数
	m_vecUDPServerInfo.clear();
	pNextTiXmlElementIP     = NULL;
	pNextTiXmlElementPort   = NULL;
	pNextTiXmlElementIpType = NULL;
	while(true)
	{
		pData = m_MainConfig.GetData("UDPServerIP", "uip", pNextTiXmlElementIP);
		if(pData != NULL)
		{
			sprintf_safe(serverinfo.m_szServerIP, MAX_BUFF_20, "%s", pData);
		}
		else
		{
			break;
		}

		pData = m_MainConfig.GetData("UDPServerIP", "uport", pNextTiXmlElementPort);
		if(pData != NULL)
		{
			serverinfo.m_nPort = ACE_OS::atoi(pData);
		}
		else
		{
			break;
		}

		pData = m_MainConfig.GetData("UDPServerIP", "uipType", pNextTiXmlElementIpType);
		if(pData != NULL)
		{
			if(ACE_OS::strcmp(pData, "IPV6") == 0)
			{
				serverinfo.m_u1IPType = TYPE_IPV6;
			}
			else
			{
				serverinfo.m_u1IPType = TYPE_IPV4;
			}
		}
		else
		{
			break;
		}


		m_vecUDPServerInfo.push_back(serverinfo);
	}

	//开始获得加载模块参数
	pData = m_MainConfig.GetData("Module", "ModulePath");
	if(pData != NULL)
	{
		sprintf_safe(m_szModulePath, MAX_BUFF_200, "%s", pData);
	}

	pData = m_MainConfig.GetData("Module", "ModuleString");
	if(pData != NULL)
	{
		sprintf_safe(m_szResourceName, MAX_BUFF_200, "%s", pData);
	}
	
	//开始获得Core相关设定(目前仅限Linux)
	pData = m_MainConfig.GetData("CoreSetting", "CoreSize");
	if(NULL != pData)
	{
		m_u4CoreFileSize = (uint16)ACE_OS::atoi(pData);
	}		

	//开始获得发送和接受阀值
	pData = m_MainConfig.GetData("SendInfo", "SendTimeout");
	if(pData != NULL)
	{
		m_u4SendTimeout = (int)ACE_OS::atoi(pData);
	}
	pData = m_MainConfig.GetData("SendInfo", "TcpNodelay");
	if(pData != NULL)
	{
		if((uint16)ACE_OS::atoi(pData) != TCP_NODELAY_ON)
		{
			m_u2TcpNodelay = TCP_NODELAY_OFF;
		}
	}
	pData = m_MainConfig.GetData("RecvInfo", "RecvBuffSize");
	if(pData != NULL)
	{
		m_u4RecvBuffSize = (int)ACE_OS::atoi(pData);
	}
	pData = m_MainConfig.GetData("SendInfo", "SendQueueMax");
	if(pData != NULL)
	{
		m_u2SendQueueMax = (uint16)ACE_OS::atoi(pData);
	}
	pData = m_MainConfig.GetData("ThreadInfo", "ThreadTimeout");
	if(pData != NULL)
	{
		m_u2ThreadTimuOut = (uint16)ACE_OS::atoi(pData);
	}
	pData = m_MainConfig.GetData("ThreadInfo", "ThreadTimeCheck");
	if(pData != NULL)
	{
		m_u2ThreadTimeCheck = (uint16)ACE_OS::atoi(pData);
	}
	pData = m_MainConfig.GetData("ThreadInfo", "DisposeTimeout");
	if(pData != NULL)
	{
		m_u2PacketTimeOut = (uint16)ACE_OS::atoi(pData);
	}
	pData = m_MainConfig.GetData("SendInfo", "SendAliveTime");
	if(pData != NULL)
	{
		m_u2SendAliveTime = (uint16)ACE_OS::atoi(pData);
	}
	pData = m_MainConfig.GetData("SendInfo", "PutQueueTimeout");
	if(pData != NULL)
	{
		m_u2SendQueuePutTime = (uint16)ACE_OS::atoi(pData);
	}
	pData = m_MainConfig.GetData("SendInfo", "SingleConnectionMaxSendMask");
	if(pData != NULL)
	{
		m_u4SendDatamark = (int)ACE_OS::atoi(pData);
	}


	//线程相关
	pData = m_MainConfig.GetData("ThreadInfo", "PutQueueTimeout");
	if(pData != NULL)
	{
		m_u2WorkQueuePutTime = (uint16)ACE_OS::atoi(pData);
	}

	//连接对象缓冲相关
	pData = m_MainConfig.GetData("ClientInfo", "HandlerCount");
	if(pData != NULL)
	{
		m_u2HandleCount = (uint16)ACE_OS::atoi(pData);
	}
	pData = m_MainConfig.GetData("ClientInfo", "MaxHandlerCount");
	if(pData != NULL)
	{
		m_u2MaxHanderCount = (uint16)ACE_OS::atoi(pData);
	}
	pData = m_MainConfig.GetData("ClientInfo", "MaxConnectTime");
	if(pData != NULL)
	{
		m_u2MaxConnectTime = (uint16)ACE_OS::atoi((char*)pData);
	}
	pData = m_MainConfig.GetData("ClientInfo", "MaxBuffRecv");
	if(pData != NULL)
	{
		m_u4ServerRecvBuff = (uint32)ACE_OS::atoi(pData);
	}

	//接收客户端信息相关配置
	pData = m_MainConfig.GetData("RecvInfo", "RecvQueueTimeout");
	if(pData != NULL)
	{
		m_u2RecvQueueTimeout = (uint16)ACE_OS::atoi(pData);
	}
	pData = m_MainConfig.GetData("SendInfo", "SendQueueTimeout");
	if(pData != NULL)
	{
		m_u2SendQueueTimeout = (uint16)ACE_OS::atoi(pData);
	}
	pData = m_MainConfig.GetData("SendInfo", "SendQueueCount");
	if(pData != NULL)
	{
		m_u2SendQueueCount = (uint16)ACE_OS::atoi(pData);
	}


	//开始获得Console服务器相关配置信息
	pData = m_MainConfig.GetData("Console", "support");
	if(pData != NULL)
	{
		m_u1ConsoleSupport = (uint8)ACE_OS::atoi(pData);
	}
	pData = m_MainConfig.GetData("Console", "sip");
	if(pData != NULL)
	{
		sprintf_safe(m_szConsoleIP, MAX_BUFF_100, "%s", pData);
	}
	pData = m_MainConfig.GetData("Console", "sport");
	if(pData != NULL)
	{
		m_nConsolePort = (int)ACE_OS::atoi(pData);
	}
	pData = m_MainConfig.GetData("Console", "sipType");
	if(pData != NULL)
	{
		if(ACE_OS::strcmp(pData, "IPV6") == 0)
		{
			m_u1ConsoleIPType = TYPE_IPV6;
		}
		else
		{
			m_u1ConsoleIPType = TYPE_IPV4;
		}
	}

	//获得Console可接受的客户端IP
	m_vecConsoleClientIP.clear();

	pNextTiXmlElementIP   = NULL;

	while(true)
	{
		_ConsoleClientIP ConsoleClientIP;
		pData = m_MainConfig.GetData("ConsoleClient", "cip", pNextTiXmlElementIP);
		if(NULL != pData)
		{
			sprintf_safe(ConsoleClientIP.m_szServerIP, MAX_BUFF_20, "%s", pData);
		}
		else
		{
			break;
		}

		m_vecConsoleClientIP.push_back(ConsoleClientIP);
	}

	//获得允许的Console链接key值
	m_vecConsoleKey.clear();
	TiXmlElement* pNextTiXmlElementKey  = NULL;

	while(true)
	{
		_ConsoleKey objConsoleKey;
		pData = m_MainConfig.GetData("ConsoleKey", "Key", pNextTiXmlElementKey);
		if(NULL != pData)
		{
			sprintf_safe(objConsoleKey.m_szKey, MAX_BUFF_100, "%s", pData);
		}
		else
		{
			break;
		}

		m_vecConsoleKey.push_back(objConsoleKey);
	}

	//开始获得ConnectValid对应的参数
	pData = m_MainConfig.GetData("ConnectValid", "ConnectCount");
	if(NULL != pData)
	{
		m_u2ValidConnectCount = (uint16)ACE_OS::atoi(pData);
	}
	pData = m_MainConfig.GetData("ConnectValid", "ConnectValid");
	if(NULL != pData)
	{
		m_u1Valid = (uint8)ACE_OS::atoi(pData);
	}
	pData = m_MainConfig.GetData("ConnectValid", "ConnectPacketCount");
	if(NULL != pData)
	{
		m_u4ValidPacketCount = (uint32)ACE_OS::atoi(pData);
	}
	pData = m_MainConfig.GetData("ConnectValid", "ConnectRecvSize");
	if(NULL != pData)
	{
		m_u4ValidRecvSize = (uint32)ACE_OS::atoi(pData);
	}
	pData = m_MainConfig.GetData("ConnectValid", "ForbiddenTime");
	if(NULL != pData)
	{
		m_u2ForbiddenTime = (uint16)ACE_OS::atoi(pData);
	}
	pData = m_MainConfig.GetData("ConnectValid", "TrackIPCount");
	if(NULL != pData)
	{
		m_u4TrackIPCount = (uint16)ACE_OS::atoi(pData);
	}

	//开始获得ConnectServer相关信息
	pData = m_MainConfig.GetData("ConnectServer", "TimeInterval");
	if(pData != NULL)
	{
		m_u4ConnectServerTimerout = (uint32)ACE_OS::atoi(pData);
	}

	pData = m_MainConfig.GetData("ConnectServer", "TimeCheck");
	if(pData != NULL)
	{
		m_u2ConnectServerCheck = (uint32)ACE_OS::atoi(pData);
	}

	pData = m_MainConfig.GetData("ConnectServer", "Recvbuff");
	if(pData != NULL)
	{
		m_u4ConnectServerRecvBuff = (uint32)ACE_OS::atoi(pData);
	}

	//开始获得监控数据
	pData = m_MainConfig.GetData("Monitor", "CpuAndMemory");
	if(pData != NULL)
	{
		m_u1Monitor = (uint8)ACE_OS::atoi(pData);
	}

	pData = m_MainConfig.GetData("Monitor", "CpuMax");
	if(pData != NULL)
	{
		m_u4MaxCpu = (uint32)ACE_OS::atoi(pData);
	}

	pData = m_MainConfig.GetData("Monitor", "MemoryMax");
	if(pData != NULL)
	{
		m_u4MaxMemory = (uint32)ACE_OS::atoi(pData);
	}

	//开始得到命令统计相关开关
	pData = m_MainConfig.GetData("CommandAccount", "Account");
	if(pData != NULL)
	{
		m_u1CommandAccount = (uint8)ACE_OS::atoi(pData);
	}

	pData = m_MainConfig.GetData("CommandAccount", "FlowAccount");
	if(pData != NULL)
	{
		m_u1CommandFlow = (uint8)ACE_OS::atoi(pData);
	}

	//开始获得工作线程监控相关
	pData = m_MainConfig.GetData("ThreadInfoAI", "AI");
	if(pData != NULL)
	{
		m_u1WTAI = (uint8)ACE_OS::atoi(pData);
	}

	pData = m_MainConfig.GetData("ThreadInfoAI", "CheckTime");
	if(pData != NULL)
	{
		m_u4WTCheckTime = (uint32)ACE_OS::atoi(pData);
	}

	pData = m_MainConfig.GetData("ThreadInfoAI", "TimeoutCount");
	if(pData != NULL)
	{
		m_u4WTTimeoutCount = (uint32)ACE_OS::atoi(pData);
	}

	pData = m_MainConfig.GetData("ThreadInfoAI", "StopTime");
	if(pData != NULL)
	{
		m_u4WTStopTime = (uint32)ACE_OS::atoi(pData);
	}

	pData = m_MainConfig.GetData("ThreadInfoAI", "ReturnDataType");
	if(pData != NULL)
	{
		m_u1WTReturnDataType = (uint8)ACE_OS::atoi(pData);
	}

	pData = m_MainConfig.GetData("ThreadInfoAI", "ReturnData");
	if(pData != NULL)
	{
		sprintf_safe(m_szWTReturnData, MAX_BUFF_1024, "%s", pData);
	}

	//ACE_DEBUG相关设置信息
	pData = m_MainConfig.GetData("AceDebug", "TrunOn");
	if(pData != NULL)
	{
		m_u1DebugTrunOn = (uint8)ACE_OS::atoi(pData);
	}

	pData = m_MainConfig.GetData("AceDebug", "DebugName");
	if(pData != NULL)
	{
		sprintf_safe(m_szDeubgFileName, MAX_BUFF_100, "%s", pData);
	}

	pData = m_MainConfig.GetData("AceDebug", "ChkInterval");
	if(pData != NULL)
	{
		m_u4ChkInterval = (uint32)ACE_OS::atoi(pData);
	}

	pData = m_MainConfig.GetData("AceDebug", "LogFileMaxSize");
	if(pData != NULL)
	{
		m_u4LogFileMaxSize = (uint32)ACE_OS::atoi(pData);
	}

	pData = m_MainConfig.GetData("AceDebug", "LogFileMaxCnt");
	if(pData != NULL)
	{
		m_u4LogFileMaxCnt = (uint32)ACE_OS::atoi(pData);
	}

	pData = m_MainConfig.GetData("AceDebug", "Level");
	if(pData != NULL)
	{
		sprintf_safe(m_szDebugLevel, MAX_BUFF_100, "%s", pData);
	}

	return true;
}

void CMainConfig::Display()
{
	OUR_DEBUG((LM_INFO, "[CMainConfig::Display]m_nServerID = %d.\n", m_nServerID));
	OUR_DEBUG((LM_INFO, "[CMainConfig::Display]m_szServerName = %s.\n", m_szServerName));
	OUR_DEBUG((LM_INFO, "[CMainConfig::Display]m_szServerVersion = %s.\n", m_szServerVersion));
	OUR_DEBUG((LM_INFO, "[CMainConfig::Display]m_szPacketVersion = %s.\n", m_szPacketVersion));
	OUR_DEBUG((LM_INFO, "[CMainConfig::Display]m_u1Debug = %d.\n", m_u1Debug));

	for(int i = 0; i < (int)m_vecServerInfo.size(); i++)
	{
		OUR_DEBUG((LM_INFO, "[CMainConfig::Display]ServerIP%d = %s.\n", i, m_vecServerInfo[i].m_szServerIP));
		OUR_DEBUG((LM_INFO, "[CMainConfig::Display]ServerPort%d = %d.\n", i, m_vecServerInfo[i].m_nPort));
	}

	OUR_DEBUG((LM_INFO, "[CMainConfig::Display]m_u4MsgHighMark = %d.\n", m_u4MsgHighMark));
	OUR_DEBUG((LM_INFO, "[CMainConfig::Display]m_u4MsgLowMark = %d.\n", m_u4MsgLowMark));
	OUR_DEBUG((LM_INFO, "[CMainConfig::Display]m_u4MsgThreadCount = %d.\n", m_u4MsgThreadCount));
	OUR_DEBUG((LM_INFO, "[CMainConfig::Display]m_u4MsgMaxQueue = %d.\n", m_u4MsgMaxQueue));
	OUR_DEBUG((LM_INFO, "[CMainConfig::Display]m_szModulePath = %s.\n", m_szModulePath));
	OUR_DEBUG((LM_INFO, "[CMainConfig::Display]m_szResourceName = %s.\n", m_szResourceName));
	OUR_DEBUG((LM_INFO, "[CMainConfig::Display]m_nEncryptFlag = %d.\n", m_nEncryptFlag));
	OUR_DEBUG((LM_INFO, "[CMainConfig::Display]m_szEncryptPass = %s.\n", m_szEncryptPass));
	OUR_DEBUG((LM_INFO, "[CMainConfig::Display]m_nEncryptOutFlag = %d.\n", m_nEncryptOutFlag));
	OUR_DEBUG((LM_INFO, "[CMainConfig::Display]m_u4SendTimeout = %d.\n", m_u4SendTimeout));
	OUR_DEBUG((LM_INFO, "[CMainConfig::Display]m_u4RecvBuffSize = %d.\n", m_u4RecvBuffSize));
	OUR_DEBUG((LM_INFO, "[CMainConfig::Display]m_u2SendQueueMax = %d.\n", m_u2SendQueueMax));
	OUR_DEBUG((LM_INFO, "[CMainConfig::Display]m_u2ThreadTimuOut = %d.\n", m_u2ThreadTimuOut));
	OUR_DEBUG((LM_INFO, "[CMainConfig::Display]m_u2ThreadTimeCheck = %d.\n", m_u2ThreadTimeCheck));
	OUR_DEBUG((LM_INFO, "[CMainConfig::Display]m_u2PacketTimeOut = %d.\n", m_u2PacketTimeOut));
	OUR_DEBUG((LM_INFO, "[CMainConfig::Display]m_u2SendAliveTime = %d.\n", m_u2SendAliveTime));
	OUR_DEBUG((LM_INFO, "[CMainConfig::Display]m_u2HandleCount = %d.\n", m_u2HandleCount));
	OUR_DEBUG((LM_INFO, "[CMainConfig::Display]m_u2MaxHanderCount = %d.\n", m_u2MaxHanderCount));
	OUR_DEBUG((LM_INFO, "[CMainConfig::Display]m_u2RecvQueueTimeout = %d.\n", m_u2RecvQueueTimeout));
	OUR_DEBUG((LM_INFO, "[CMainConfig::Display]m_u2SendQueueTimeout = %d.\n", m_u2SendQueueTimeout));
	OUR_DEBUG((LM_INFO, "[CMainConfig::Display]m_u2SendQueueCount = %d.\n", m_u2SendQueueCount));
	OUR_DEBUG((LM_INFO, "[CMainConfig::Display]m_u2SendQueuePutTime = %d.\n", m_u2SendQueuePutTime));
	OUR_DEBUG((LM_INFO, "[CMainConfig::Display]m_u2WorkQueuePutTime = %d.\n", m_u2WorkQueuePutTime));
	OUR_DEBUG((LM_INFO, "[CMainConfig::Display]m_u1ServerType = %d.\n", m_u1ServerType));
	OUR_DEBUG((LM_INFO, "[CMainConfig::Display]m_u4SendDatamark = %d.\n", m_u4SendDatamark));
	OUR_DEBUG((LM_INFO, "[CMainConfig::Display]m_u2TcpNodelay = %d.\n", m_u2TcpNodelay));
	OUR_DEBUG((LM_INFO, "[CMainConfig::Display]m_u4TrackIPCount = %d.\n", m_u4TrackIPCount));

	for(int i = 0; i < (int)m_vecUDPServerInfo.size(); i++)
	{
		OUR_DEBUG((LM_INFO, "[CMainConfig::Display]ServerIP%d = %s.\n", i, m_vecUDPServerInfo[i].m_szServerIP));
		OUR_DEBUG((LM_INFO, "[CMainConfig::Display]ServerPort%d = %d.\n", i, m_vecUDPServerInfo[i].m_nPort));
	}

	OUR_DEBUG((LM_INFO, "[CMainConfig::Display]m_u1ConsoleSupport = %d.\n", m_u1ConsoleSupport));
	OUR_DEBUG((LM_INFO, "[CMainConfig::Display]m_szConsoleIP = %s.\n", m_szConsoleIP));
	OUR_DEBUG((LM_INFO, "[CMainConfig::Display]m_nConsolePort = %d.\n", m_nConsolePort));

	OUR_DEBUG((LM_INFO, "[CMainConfig::Display]m_u2ValidConnectCount = %d.\n", m_u2ValidConnectCount));
	OUR_DEBUG((LM_INFO, "[CMainConfig::Display]m_u1Valid = %d.\n", m_u1Valid));
	OUR_DEBUG((LM_INFO, "[CMainConfig::Display]m_u4ValidPacketCount = %d.\n", m_u4ValidPacketCount));
	OUR_DEBUG((LM_INFO, "[CMainConfig::Display]m_u4ValidRecvSize = %d.\n", m_u4ValidRecvSize));
	OUR_DEBUG((LM_INFO, "[CMainConfig::Display]m_u2ForbiddenTime = %d.\n", m_u2ForbiddenTime));
	OUR_DEBUG((LM_INFO, "[CMainConfig::Display]m_u4ConnectServerTimerout = %d.\n", m_u4ConnectServerTimerout));
	OUR_DEBUG((LM_INFO, "[CMainConfig::Display]m_u2ConnectServerCheck = %d.\n", m_u2ConnectServerCheck));
	OUR_DEBUG((LM_INFO, "[CMainConfig::Display]m_u4ConnectServerRecvBuff = %d.\n", m_u4ConnectServerRecvBuff));
	OUR_DEBUG((LM_INFO, "[CMainConfig::Display]m_u1CommandFlow = %d.\n", m_u1CommandFlow));
	OUR_DEBUG((LM_INFO, "[CMainConfig::Display]m_u1CommandAccount = %d.\n", m_u1CommandAccount));
	OUR_DEBUG((LM_INFO, "[CMainConfig::Display]m_u4CoreFileSize = %d.\n", m_u4CoreFileSize));
	OUR_DEBUG((LM_INFO, "[CMainConfig::Display]m_u2Backlog = %d.\n", m_u2Backlog));
}

const char* CMainConfig::GetServerName()
{
	return m_szServerName;
}

uint16 CMainConfig::GetServerID()
{
	return (uint16)m_nServerID;
}

uint16 CMainConfig::GetServerPortCount()
{
	return (uint16)m_vecServerInfo.size();
}

uint16 CMainConfig::GetHandleCount()
{
	return m_u2HandleCount;
}

_ServerInfo* CMainConfig::GetServerPort(int nIndex)
{
	if(nIndex > (uint16)m_vecServerInfo.size())
	{
		return NULL;
	}

	return &m_vecServerInfo[nIndex];
}

uint32 CMainConfig::GetMgsHighMark()
{
	return m_u4MsgHighMark;
}

uint32 CMainConfig::GetMsgLowMark()
{
	return m_u4MsgLowMark;
}

uint32 CMainConfig::GetThreadCount()
{
	return m_u4MsgThreadCount;
}

uint32 CMainConfig::GetMsgMaxQueue()
{
	return m_u4MsgMaxQueue;
}

const char* CMainConfig::GetModulePath()
{
	return m_szModulePath;
}

const char* CMainConfig::GetModuleString()
{
	return m_szResourceName;
}

int CMainConfig::GetEncryptFlag()
{
	return m_nEncryptFlag;
}

const char* CMainConfig::GetEncryptPass() 
{
	return m_szEncryptPass;
}

int CMainConfig::GetEncryptOutFlag()
{
	return m_nEncryptOutFlag;
}

uint32 CMainConfig::GetReactorCount()
{
	return m_u4ReactorCount;
}

uint16 CMainConfig::GetUDPServerPortCount()
{
	return (uint16)m_vecUDPServerInfo.size();
}

_ServerInfo* CMainConfig::GetUDPServerPort(int nIndex)
{
	if(nIndex > (uint16)m_vecUDPServerInfo.size())
	{
		return NULL;
	}

	return &m_vecUDPServerInfo[nIndex];
}

uint32 CMainConfig::GetSendTimeout()
{
	return m_u4SendTimeout;
}

uint32 CMainConfig::GetRecvBuffSize()
{
	return m_u4RecvBuffSize;
}

uint16 CMainConfig::GetSendQueueMax()
{
	return m_u2SendQueueMax;
}

uint16 CMainConfig::GetThreadTimuOut()
{
	return m_u2ThreadTimuOut;
}

uint16 CMainConfig::GetThreadTimeCheck()
{
	return m_u2ThreadTimeCheck;
}

uint16 CMainConfig::GetPacketTimeOut()
{
	return m_u2PacketTimeOut;
}

uint16 CMainConfig::GetSendAliveTime()
{
	return m_u2SendAliveTime;
};

uint16 CMainConfig::GetMaxHandlerCount()
{
	return m_u2MaxHanderCount;
}

uint16 CMainConfig::GetMaxConnectTime()
{
	return m_u2MaxConnectTime;
}

uint8 CMainConfig::GetConsoleSupport()
{
	return m_u1ConsoleSupport;
}

int CMainConfig::GetConsolePort()
{
	return m_nConsolePort;
}

const char* CMainConfig::GetConsoleIP()
{
	return m_szConsoleIP;
}

uint16 CMainConfig::GetRecvQueueTimeout()
{
	return m_u2RecvQueueTimeout;
}

uint16 CMainConfig::GetSendQueueTimeout()
{
	return m_u2SendQueueTimeout;
}

uint16 CMainConfig::GetSendQueueCount()
{
	return m_u2SendQueueCount;
}

bool CMainConfig::CompareConsoleClinetIP(const char* pConsoleClientIP)
{
	//如果没有配置IP，则默认全部可以
	if((int)m_vecConsoleClientIP.size() == 0)
	{
		return true;
	}

	for(int i = 0; i < (int)m_vecConsoleClientIP.size(); i++)
	{
		if(ACE_OS::strcmp(m_vecConsoleClientIP[i].m_szServerIP, pConsoleClientIP) == 0)
		{
			return true;
		}
	}

	return false;
}

uint16 CMainConfig::GetValidConnectCount()
{
	return m_u2ValidConnectCount;
}

uint8 CMainConfig::GetValid()
{
	return m_u1Valid;
}

uint32 CMainConfig::GetValidPacketCount()
{
	return m_u4ValidPacketCount;
}

uint32 CMainConfig::GetValidRecvSize()
{
	return m_u4ValidRecvSize;
}

uint16 CMainConfig::GetForbiddenTime()
{
	return m_u2ForbiddenTime;
}

uint8 CMainConfig::GetCommandAccount()
{
	return m_u1CommandAccount;
}

const char* CMainConfig::GetServerVersion()
{
	return m_szServerVersion;
}

const char* CMainConfig::GetPacketVersion()
{
	return m_szPacketVersion;
}

vecConsoleKey* CMainConfig::GetConsoleKey()
{
	return &m_vecConsoleKey;
}

uint32 CMainConfig::GetConnectServerTimeout()
{
	return m_u4ConnectServerTimerout;
}

uint16 CMainConfig::GetConnectServerCheck()
{
	return m_u2ConnectServerCheck;
}

uint16 CMainConfig::GetSendQueuePutTime()
{
	return m_u2SendQueuePutTime;
}

uint16 CMainConfig::GetWorkQueuePutTime()
{
	return m_u2WorkQueuePutTime;
}

uint8 CMainConfig::GetServerType()
{
	return m_u1ServerType;
}

const char* CMainConfig::GetWindowsServiceName()
{
	return m_szWindowsServiceName;
}

const char* CMainConfig::GetDisplayServiceName()
{
	return m_szDisplayServiceName;
}

uint8 CMainConfig::GetDebug()
{
	return m_u1Debug;
}

void CMainConfig::SetDebug(uint8 u1Debug)
{
	m_u1Debug = u1Debug;
}

uint8 CMainConfig::GetNetworkMode()
{
	return m_u1NetworkMode;
}

uint32 CMainConfig::GetConnectServerRecvBuffer()
{
	return m_u4ConnectServerRecvBuff;
}

uint8 CMainConfig::GetMonitor()
{
	return m_u1Monitor;
}

uint32 CMainConfig::GetServerRecvBuff()
{
	return m_u4ServerRecvBuff;
}

uint8 CMainConfig::GetConsoleIPType()
{
	return m_u1ConsoleIPType;
}

uint8 CMainConfig::GetCommandFlow()
{
	return m_u1CommandFlow;
}

uint32 CMainConfig::GetSendDataMask()
{
	return m_u4SendDatamark;
}

uint32 CMainConfig::GetCoreFileSize()
{
	return m_u4CoreFileSize;
}

uint16 CMainConfig::GetTcpNodelay()
{
	return m_u2TcpNodelay;
}

uint16 CMainConfig::GetBacklog()
{
	return m_u2Backlog;
}

ENUM_CHAR_ORDER CMainConfig::GetCharOrder()
{
	return m_u1CharOrder;
}

uint16 CMainConfig::GetTrackIPCount()
{
	return m_u4TrackIPCount;
}

uint32 CMainConfig::GetCpuMax()
{
	return m_u4MaxCpu;
}

uint32 CMainConfig::GetMemoryMax()
{
	return m_u4MaxMemory;
}

uint8 CMainConfig::GetWTAI()
{
	return m_u1WTAI;
}

uint32 CMainConfig::GetWTCheckTime()
{
	return m_u4WTCheckTime;
}

uint32 CMainConfig::GetWTTimeoutCount()
{
	return m_u4WTTimeoutCount;
}

uint32 CMainConfig::GetWTStopTime()
{
	return m_u4WTStopTime;
}

uint8 CMainConfig::GetWTReturnDataType()
{
	return m_u1WTReturnDataType;
}

char* CMainConfig::GetWTReturnData()
{
	return (char* )m_szWTReturnData;
}

bool CMainConfig::GetByteOrder()
{
	return m_blByteOrder;
}

uint8 CMainConfig::GetDebugTrunOn()
{
	return m_u1DebugTrunOn;
}

char* CMainConfig::GetDebugFileName()
{
	return m_szDeubgFileName;
}

uint32 CMainConfig::GetChkInterval()
{
	return m_u4ChkInterval;
}

uint32 CMainConfig::GetLogFileMaxSize()
{
	return m_u4LogFileMaxSize;
}

uint32 CMainConfig::GetLogFileMaxCnt()
{
	return m_u4LogFileMaxCnt;
}

char* CMainConfig::GetDebugLevel()
{
	return m_szDebugLevel;
}
