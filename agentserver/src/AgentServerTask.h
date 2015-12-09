
/**
 * Copyright (C) Anny.
 * Copyright (C) Disvr, Inc.
 */

#ifndef _AGENT_SERVER_TASK_H_
#define _AGENT_SERVER_TASK_H_

#include <arpa/inet.h>

#include "wType.h"
#include "wHeadCmd.h"
#include "wTcpTask.h"
#include "wLog.h"

class AgentServerTask : public wTcpTask
{
	public:
		AgentServerTask() {}
		~AgentServerTask();
		AgentServerTask(wSocket *pSocket);
		
		virtual int HandleRecvMessage(char * pBuffer, int nLen);
		
		int ParseRecvMessage(struct wHeadCmd* pHeadCmd ,char *pBuffer,int iLen);
};

#endif