
/**
 * Copyright (C) Anny Wang.
 * Copyright (C) Hupu, Inc.
 */

#ifndef _ROUTER_SERVER_H_
#define _ROUTER_SERVER_H_

#include <arpa/inet.h>

#include "wCore.h"
#include "wMisc.h"
#include "wLog.h"
#include "wSingleton.h"
#include "wTask.h"
#include "wSocket.h"
#include "wServer.h"
#include "RouterConfig.h"

class RouterServer: public wServer<RouterServer>
{
	public:
		RouterServer();
		virtual void Run();
		
		virtual wTask* NewTcpTask(wSocket *pSocket);
		void CheckModSvr();	//检测配置文件是否修改
		
	protected:
		RouterConfig *mConfig {NULL};
};

#endif
