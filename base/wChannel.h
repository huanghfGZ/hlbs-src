
/**
 * Copyright (C) Anny.
 * Copyright (C) Disvr, Inc.
 */

#ifndef _W_CHANNEL_H_
#define _W_CHANNEL_H_

#include <sys/socket.h>
#include <sys/un.h>
#include <sys/uio.h>

#include "wCore.h"
#include "wIO.h"
#include "wLog.h"
#include "wMisc.h"

class wChannel : public wIO
{
	public:	
		wChannel();
		virtual ~wChannel();
		void Initialize();
		
		/**
		 * 创建非阻塞channel
		 * @return 0成功 -1发生错误
		 */
		virtual int Open();
		
		/**
		 * 接受channel数据
		 * @param  iFD  channel FD
		 * @param  pCh  接受缓冲
		 * @param  size 缓冲长度
		 * @return      <0 失败 >0实际接受长度
		 */		
		virtual ssize_t RecvBytes(char *vArray, size_t vLen);
		
		/**
		 * 发送channel_t消息
		 * @param  iFD  channel FD
		 * @param  pCh  channel_t消息
		 * @param  size channel_t长度
		 * @return 0成功，-1失败     
		 */
		virtual ssize_t SendBytes(char *vArray, size_t vLen);

		/**
		 * 关闭channel[0] channel[1] 描述符
		 */
		virtual void Close();

		int &operator[](int i);

	protected:
		int	mCh[2];
		int mErr;
};

#endif
