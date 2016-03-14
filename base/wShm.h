
/**
 * Copyright (C) Anny.
 * Copyright (C) Disvr, Inc.
 */

#ifndef _W_SHM_H_
#define _W_SHM_H_

#include <sys/ipc.h>
#include <sys/shm.h>

#include "wCore.h"
#include "wLog.h"
#include "wNoncopyable.h"

/**
 * 共享内存管理
 *
 * 注意：此对象本身并不共享内存中，所以需在共享内存上额外存储头信息shmhead_t，长度为sizeof(struct shmhead_t)
 */
class wShm : private wNoncopyable
{
	struct shmhead_t
	{
		char *mStart;	//共享内存开始地址
		char *mEnd;		//共享内存结束地址
		char *mUsedOff;	//已被分配共享内存偏移地址offset，即下次可使用的开始地址
	};

	public:
		wShm(const char *filename, int pipeid = 'i', size_t size = MSG_QUEUE_LEN);
		void Initialize();
		virtual ~wShm();

		char *CreateShm();
		char *AttachShm();
		
		/**
		 *  TODO.
		 *  对齐
		 */
		char *AllocShm(size_t size = 0);
		void FreeShm();

	protected:
		char mFilename[255];
		int mPipeId;
		key_t mKey;
		int mShmId;
		
		size_t mSize;
		shmhead_t *mShmhead;	//共享内存头信息

		int mPagesize;
};

#endif