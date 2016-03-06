
/**
 * Copyright (C) Anny.
 * Copyright (C) Disvr, Inc.
 */

#ifndef _W_LOG_H_
#define _W_LOG_H_

#include <stdarg.h>
#include "wType.h"

/**
 * 基于log4cpp的日志管理库
 * 日志系统开关
 */

#ifdef USE_LOG4CPP
#	define INIT_ROLLINGFILE_LOG		InitLog					//初始化一种日志类型（基于回卷文件)
#	define LOG_SHUTDOWN_ALL			ShutdownAllLog()		//关闭所有类型日志

#ifdef _DEBUG_
#	define LOG_DEBUG				LogDebug
#else
#	define LOG_DEBUG				LogDebug
#endif

#	define LOG_NOTICE				LogNotice
#	define LOG_INFO					LogInfo
#	define LOG_WARN					LogWarn
#	define LOG_ERROR				LogError
#	define LOG_FATAL				LogFatal
#	define LOG						Log
#	define RE_INIT_LOG				ReInitLog

#else
#	define INIT_ROLLINGFILE_LOG	
#	define LOG_SHUTDOWN_ALL		
#	define LOG_DEBUG
#	define LOG_NOTICE
#	define LOG_INFO	
#	define LOG_WARN
#	define LOG_ERROR
#	define LOG_FATAL
#	define LOG
#	define RE_INIT_LOG				ReInitLog
#endif


//日志等级
//NOTSET <  DEBUG < INFO  < WARN < LEVEL_NOTICE < ERROR  < FATAL 
enum LogLevel
{	
	LEVEL_FATAL  = 0,
	LEVEL_ERROR  = 300,
	LEVEL_WARN   = 400,
	LEVEL_NOTICE = 500,
	LEVEL_INFO   = 600,
	LEVEL_DEBUG  = 700,
	LEVEL_NOTSET = 800,
};


//初始化一种类型的日志：如果该类型日志已存在，
//则重新初始化，如果不存在，则创建。
int InitLog( const char*	vLogName,						/*日志类型的名称(关键字,由此定位到日志文件)*/
			const char*		vLogDir,						/*文件名称(路径)*/
			LogLevel		vPriority = LEVEL_NOTSET,		/*日志等级*/
			unsigned int	vMaxFileSize = 10*1024*1024,	/*回卷文件最大长度*/
			unsigned int	vMaxBackupIndex = 1,			/*回卷文件个数*/
			bool			vAppend = true);				/*是否截断(默认即可)*/



//重新给已存在的日志赋值，
//但是不能改变日志的名称，以及定位的文件。
int ReInitLog( const char* vLogName, 
			  LogLevel		vPriority = LEVEL_NOTSET,		/*日志等级*/
			  unsigned int	vMaxFileSize = 10*1024*1024,	/*回卷文件最大长度*/
			  unsigned int	vMaxBackupIndex = 1);			/*回卷文件个数*/
						


//关闭所有类新的日志，
//(包括文件句柄和清除相关对象),在程序退出前用.
int ShutdownAllLog( );


//具体日志记录函数，写入创建时关联的文件.
int LogDebug( const char* vLogName, const char* vFmt, ... );
int LogInfo	( const char* vLogName, const char* vFmt, ... );
int LogNotice( const char* vLogName, const char* vFmt, ... );
int LogWarn( const char* vLogName, const char* vFmt, ... );
int LogError( const char* vLogName, const char* vFmt, ... );
int LogFatal( const char* vLogName, const char* vFmt, ... );
int log( const char* vLogName, int vPriority, const char* vFmt, ... );

void Log_bin( const char* vLogName, void* vBin, int vBinLen );

int LogDebug_va( const char* vLogName, const char* vFmt, va_list ap);
int LogNotice_va( const char* vLogName, const char* vFmt, va_list ap);
int LogInfo_va( const char* vLogName, const char* vFmt, va_list ap );
int LogWarn_va( const char* vLogName, const char* vFmt, va_list ap );
int LogError_va( const char* vLogName, const char* vFmt, va_list ap );
int LogFatal_va( const char* vLogName, const char* vFmt, va_list ap );
int log_va( const char* vLogName, int vPriority, const char* vFmt, va_list ap );


//error log
#define ELOG_KEY	"error"
#define ELOG_LEVEL	LEVEL_INFO
#define ELOG_FILE	"log/error.log"
#define ELOG_FSIZE 10*1024*1024
#define ELOG_BACKUP	20

#endif
