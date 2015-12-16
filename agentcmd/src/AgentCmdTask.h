
/**
 * Copyright (C) Anny.
 * Copyright (C) Disvr, Inc.
 */

#ifndef _AGENT_SERVER_TASK_H_
#define _AGENT_SERVER_TASK_H_

#include <arpa/inet.h>

#include "wType.h"
#include "wTcpTask.h"
#include "wLog.h"

#include "wCommand.h"
//#include "RtblCommand.h"

class AgentCmdTask : public wTcpTask
{
	public:
		AgentCmdTask() {}
		~AgentCmdTask();
		AgentCmdTask(wSocket *pSocket);
		
		virtual int HandleRecvMessage(char * pBuffer, int nLen);
		
		int ParseRecvMessage(struct wCommand* pCommand ,char *pBuffer,int iLen);
};

#endif