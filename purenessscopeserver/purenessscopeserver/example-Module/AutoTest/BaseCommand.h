#pragma once

#include "IBuffPacket.h"
#include "ClientCommand.h"
#include "IObject.h"

#include <string>

//����ͻ�������(TCP)
#define COMMAND_AUTOTEST_HEAD         0x5000   //��ͷ�������ݰ�
#define COMMAND_AUTOTEST_NOHEAD       0x5001   //����ͷ���ݰ�
#define COMMAND_AUTOTEST_HEADBUFF     0x5002   //��ͷ�������ݰ�
#define COMMAND_AUTOTEST_NOHEADBUFF   0x5003   //����ͷ�������ݰ�

#define COMMAND_AUTOTEST_RETUEN_HEAD         0xf000   
#define COMMAND_AUTOTEST_RETUEN_NOHEAD       0xf001
#define COMMAND_AUTOTEST_RETUEN_HEADBUFF     0xf002
#define COMMAND_AUTOTEST_RETUEN_NOHEADBUFF   0xf003

using namespace std;

class CBaseCommand : public CClientCommand
{
public:
	CBaseCommand(void);
	~CBaseCommand(void);

	int DoMessage(IMessage* pMessage, bool& bDeleteFlag);
	void SetServerObject(CServerObject* pServerObject);

private:
	bool Do_Head(IMessage* pMessage);            //���ذ���ͷ�����ݰ�
	bool Do_NoHead(IMessage* pMessage);          //���ز�����ͷ�����ݰ�
	bool Do_HeadBuff(IMessage* pMessage);        //���ذ�����������ݰ�ͷ�����ݰ�
	bool Do_NoHeadBuff(IMessage* pMessage);      //���ز��������ݰ�ͷ�����ݰ�


private:
	CServerObject* m_pServerObject;
	int            m_nCount;
};