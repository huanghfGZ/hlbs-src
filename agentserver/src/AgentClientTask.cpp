
/**
 * Copyright (C) Anny Wang.
 * Copyright (C) Hupu, Inc.
 */

#include "AgentClientTask.h"
#include "AgentConfig.h"
#include "LoginCmd.h"
#include "SvrCmd.h"

AgentClientTask::AgentClientTask()
{
    Initialize();
}

AgentClientTask::AgentClientTask(wSocket *pSocket) : wTcpTask(pSocket)
{
    Initialize();
}

void AgentClientTask::Initialize()
{
	REG_DISP(mDispatch, "AgentClientTask", CMD_SVR_RES, SVR_RES_INIT, &AgentClientTask::InitSvrRes);
	REG_DISP(mDispatch, "AgentClientTask", CMD_SVR_RES, SVR_RES_RELOAD, &AgentClientTask::ReloadSvrRes);
	REG_DISP(mDispatch, "AgentClientTask", CMD_SVR_RES, SVR_RES_SYNC, &AgentClientTask::SyncSvrRes);
}

int AgentClientTask::VerifyConn()
{
	if(!AGENT_LOGIN) return 0;
	
	//验证登录消息
	char pBuffer[sizeof(struct LoginReqToken_t)];
	int iLen = SyncRecv(pBuffer, sizeof(struct LoginReqToken_t));
	if (iLen > 0)
	{
		struct LoginReqToken_t *pLoginRes = (struct LoginReqToken_t*) pBuffer;
		if (strcmp(pLoginRes->mToken, "Anny") == 0)
		{
			LOG_ERROR(ELOG_KEY, "[client] receive client and verify success from ip(%s) port(%d) with token(%s)", mSocket->Host().c_str(), mSocket->Port(), pLoginRes->mToken);
			//mConnType = pLoginRes->mConnType;
			return 0;
		}
		LOG_ERROR(ELOG_KEY, "[client] receive client and verify failed from ip(%s) port(%d) with token(%s)", mSocket->Host().c_str(), mSocket->Port(), pLoginRes->mToken);
	}
	return -1;
}

int AgentClientTask::Verify()
{
	if(!ROUTER_LOGIN) return 0;
	
	//验证登录
	struct LoginReqToken_t stLoginReq;
	//stLoginReq.mConnType = SERVER_AGENT;
	memcpy(stLoginReq.mToken, "Anny", 4);
	SyncSend((char*)&stLoginReq, sizeof(stLoginReq));
	return 0;
}

int AgentClientTask::HandleRecvMessage(char *pBuffer, int nLen)
{
	W_ASSERT(pBuffer != NULL, return -1);
	struct wCommand *pCommand = (struct wCommand*) pBuffer;
	return ParseRecvMessage(pCommand , pBuffer, nLen);
}

int AgentClientTask::ParseRecvMessage(struct wCommand *pCommand, char *pBuffer, int iLen)
{
	if (pCommand->GetId() == W_CMD(CMD_NULL, PARA_NULL))
	{
		//空消息(心跳返回)
		mHeartbeatTimes = 0;
	}
	else
	{
		auto pF = mDispatch.GetFuncT("AgentClientTask", pCommand->GetId());

		if (pF != NULL)
		{
			pF->mFunc(pBuffer, iLen);
		}
		else
		{
			LOG_ERROR(ELOG_KEY, "[system] client send a invalid msg fd(%d) id(%d)", mSocket->FD(), pCommand->GetId());
		}
	}
	return 0;
}

//router发来init相响应
int AgentClientTask::InitSvrRes(char *pBuffer, int iLen)
{
	AgentConfig *pConfig = AgentConfig::Instance();
	SvrResInit_t *pCmd = (struct SvrResInit_t*) pBuffer;
	
	if(pCmd->mNum > 0) 
	{
		for(int i = 0; i < pCmd->mNum; i++)
		{
			LOG_DEBUG(ELOG_KEY, "[system] init svr from router gid(%d),xid(%d),host(%s),port(%d),weight(%d),ver(%d)", 
				pCmd->mSvr[i].mGid, pCmd->mSvr[i].mXid, pCmd->mSvr[i].mHost, pCmd->mSvr[i].mPort, pCmd->mSvr[i].mWeight, pCmd->mSvr[i].mVersion);
			
			pConfig->Qos()->SaveNode(pCmd->mSvr[i]);
		}
	}
	return 0;
}

//router发来reload响应
int AgentClientTask::ReloadSvrRes(char *pBuffer, int iLen)
{
	AgentConfig *pConfig = AgentConfig::Instance();
	struct SvrResReload_t *pCmd = (struct SvrResReload_t* )pBuffer;

	if(pCmd->mNum > 0) 
	{
		pConfig->Qos()->CleanNode();	
		for(int i = 0; i < pCmd->mNum; i++)
		{
			LOG_DEBUG(ELOG_KEY, "[system] reload svr from router gid(%d),xid(%d),host(%s),port(%d),weight(%d),ver(%d)", 
				pCmd->mSvr[i].mGid, pCmd->mSvr[i].mXid, pCmd->mSvr[i].mHost, pCmd->mSvr[i].mPort, pCmd->mSvr[i].mWeight, pCmd->mSvr[i].mVersion);
			
			pConfig->Qos()->SaveNode(pCmd->mSvr[i]);
		}
	}
	return 0;
}

//router发来sync响应(增量同步)
int AgentClientTask::SyncSvrRes(char *pBuffer, int iLen)
{
	AgentConfig *pConfig = AgentConfig::Instance();
	struct SvrResSync_t *pCmd = (struct SvrResSync_t* )pBuffer;

	if(pCmd->mNum > 0) 
	{
		for(int i = 0; i < pCmd->mNum; i++)
		{
			LOG_DEBUG(ELOG_KEY, "[system] sync svr from router gid(%d),xid(%d),host(%s),port(%d),weight(%d),ver(%d)", 
				pCmd->mSvr[i].mGid, pCmd->mSvr[i].mXid, pCmd->mSvr[i].mHost, pCmd->mSvr[i].mPort, pCmd->mSvr[i].mWeight, pCmd->mSvr[i].mVersion);
			
			pConfig->Qos()->ModNode(pCmd->mSvr[i]);
		}
	}
	return 0;
}
