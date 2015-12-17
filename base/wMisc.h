
/**
 * Copyright (C) Anny.
 * Copyright (C) Disvr, Inc.
 */

#ifndef _W_MISC_H_
#define _W_MISC_H_

#include <sstream>
#include <string>
#include <vector>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/time.h>
#include <string.h>

#include "wCommand.h"
#include "wType.h"

//计算数组中个体的个数
#define CountV(entry_v) (sizeof(entry_v) / sizeof((entry_v)[0]))

inline const char* IP2Text(DWORD ip)
{
	in_addr in;
	in.s_addr = ip;

	return inet_ntoa(in);
}

inline DWORD Text2IP(const char* ipstr)
{
	return inet_addr(ipstr);
}

inline unsigned int HashString(const char* s)
{
	unsigned int hash = 5381;
	while (*s)
	{
		hash += (hash << 5) + (*s ++);
	}
	return hash & 0x7FFFFFFF;
}

inline vector<string> Split(string sStr, string sPattern, bool bRepeat = true)  
{  
    string::size_type iPos, iNextPos;
    vector<string> vResult;
    sStr += sPattern;  
    int iSize = sStr.size();
  
    for(int i = 0; i < iSize; i++)  
    {  
        iPos = iNextPos = sStr.find(sPattern, i);
        /*
        while(bRepeat)
        {
        	i = iNextPos + sPattern.size() - 1;
        	iNextPos = sStr.find(sPattern, i);
        	if(iNextPos + 1 != i)
        	{
        		i -= iNextPos + sPattern.size() - 1;
        		break;
        	}
        }
        */
        if(iPos < iSize)
        {
            string s = sStr.substr(i, iPos - i);
            vResult.push_back(s);
            i = iPos + sPattern.size() - 1;
        }
    }
    return vResult;  
}

//获取毫秒级时间
inline unsigned long long GetTickCount()
{
	struct timeval tv;
	gettimeofday(&tv, NULL);
	return (unsigned long long)tv.tv_sec * 1000 + (unsigned long long)tv.tv_usec / 1000;
}

//变为守护进程
int InitDaemon(const char *filename);

//linux没有这个函数,不好说什么时候就用到了
void itoa(unsigned long val, char *buf, unsigned radix);

inline string Itos(const int &i)
{
	string sTmp;
	stringstream sRet(sTmp);
	sRet << i;
	return sRet.str();
}

template<typename T> char* Serialize(T& vCmd,int& pLen)
{
	int iSize = sizeof(T);
	pLen = iSize + sizeof(int);
	char* pBuffer = new char[pLen];
	*(int*)pBuffer = iSize;
	strncpy(pBuffer + sizeof(int), (char*)&vCmd, iSize);
	return pBuffer;
}

template<typename T> char* Parse(int& pLen)
{
	//
}

#endif
