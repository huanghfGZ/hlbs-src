
/**
 * Copyright (C) Anny.
 * Copyright (C) Disvr, Inc.
 */

#include <algorithm>

#include "SvrQos.h"

SvrQos::SvrQos()
{
	Initialize();
}

SvrQos::~SvrQos() {}

void SvrQos::Initialize()
{
	//
}











int SvrQos::InitSvr(SvrNet_t* pSvr, int iNum)
{
	if (iNum <= 0)
	{
		return -1;
	}
	Final();

	Svr_t *pTmpSvr;
	for(int i = 0; i < iNum ; i++)
	{
		if (pSvr[i] == NULL)
		{
			continue;
		}

		pTmpSvr = new Svr_t(pSvr[i]);	//SvrNet -> Svr
		if (pTmpSvr == NULL)
		{
			LOG_ERROR(ELOG_KEY, "[runtime] init id(%d) svr failed", pSvr[i]->mId);
			continue;
		}
		mSvr.push_back(pTmpSvr);
	}

	SvrRebuild();
	return 0;
}

int SvrQos::ReloadSvr(SvrNet_t *pSvr, int iNum)
{
	return InitSvr(pSvr, iNum);
}

//更新不同配置 & 添加新配置
int SvrQos::SyncSvr(SvrNet_t *pSvr, int iNum)
{
	if (iNum <= 0)
	{
		return -1;
	}

	int j = 0;
	Svr_t *pTmpSvr;
	for(int i = 0; i < iNum ; i++)
	{
		vector<Svr_t*>::iterator it = GetItById((pSvr+i)->mId);
		if (it != mSvr.end())
		{
			if(IsChangeSvr(*it, pSvr+i))
			{
				//更新配置
				SAFE_DELETE(*it);
				mSvr.erase(it);
				pTmpSvr = new Svr_t(pSvr[i]);	//SvrNet -> Svr
				if (pTmpSvr == NULL)
				{
					LOG_ERROR(ELOG_KEY, "[runtime] init id(%d) svr failed", pSvr[i]->mId);
					continue;
				}
				mSvr.push_back(pTmpSvr);
				j++;
			}
		}
		else
		{
			//添加新配置
			pTmpSvr = new Svr_t(pSvr[i]);
			if (pTmpSvr == NULL)
			{
				LOG_ERROR(ELOG_KEY, "[runtime] init id(%d) svr failed", pSvr[i]->mId);
				continue;
			}
			mSvr.push_back(pTmpSvr);
			j++;
		}
	}
	SvrRebuild();	
	return j;
}

int SvrQos::GetSvrAll(SvrNet_t* pBuffer)
{
	vector<Svr_t*>::iterator it = mSvr.begin();
	if (mSvr.size() < 1)
	{
		return 0;
	}

	for(int i = 0; it != mSvr.end(); i++, it++)
	{
		pBuffer[i] = ((struct SvrNet_t)**it);   //Svr_t => SvrNet_t
	}
	return mSvr.size();
}

int SvrQos::GetAllSvrByGXid(SvrNet_t* pBuffer, int iGid, int iXid)
{
	string sGid = Itos(iGid);
	string sXid = Itos(iXid);
	string sGXid = sGid + "-" + sXid;
	
	vector<Svr_t*> vSvr;
	int iNum = 0;
	map<string, vector<Svr_t*> >::iterator mn = mSvrByGXid.find(sGXid);
	if(mn != mSvrByGXid.end())
	{
		vSvr = mn->second;	//已排序
		iNum = vSvr.size();

		vector<Svr_t*>::iterator it = vSvr.begin();
		for(int i = 0; i < iNum && it != vSvr.end(); i++, it++)
		{
			pBuffer[i] = ((struct SvrNet_t)**it);   //Svr_t => SvrNet_t
		}
	}
	else
	{
		iNum = 0;
	}
	return iNum;
}

//获取gid、xid下的某一svr
int SvrQos::GetSvrByGXid(SvrNet_t* pBuffer, int iGid, int iXid)
{
	string sGid = Itos(iGid);
	string sXid = Itos(iXid);
	string sGXid = sGid + "-" + sXid;
	
	int iId = 0;
	map<string, vector<Svr_t*> >::iterator mn = mSvrByGXid.find(sGXid);
	if(mn != mSvrByGXid.end())
	{
		iId = GXidWRRSvr(pBuffer, sGXid, mn->second);
	}
	return iId;
}

//加权轮询
int SvrQos::GXidWRRSvr(SvrNet_t* pBuffer, string sKey, vector<Svr_t*> vSvr)
{
	map<string, StatcsGXid_t*>::iterator mn = mStatcsGXid.find(sKey);
	int iLen = vSvr.size();
	if (iLen <= 0 || mn == mStatcsGXid.end())
	{
		return -1;	//讲道理的话，这里不会被执行
	}
	int iLoop = iLen;
	StatcsGXid_t *pStatcs = mn->second;

	LOG_DEBUG(ELOG_KEY, "[runtime] WRR init id,%d", pStatcs->mIdx);
	LOG_DEBUG(ELOG_KEY, "[runtime] WRR init cur,%d", pStatcs->mCur);
	LOG_DEBUG(ELOG_KEY, "[runtime] WRR init gcd,%d", pStatcs->mGcd);

	while(iLoop-- >= 0)
	{
		pStatcs->mIdx = (pStatcs->mIdx + 1) % iLen;
		if (pStatcs->mIdx == 0)
		{
			pStatcs->mCur = pStatcs->mCur - pStatcs->mGcd;
			if (pStatcs->mCur <= 0)
			{
				pStatcs->mCur = vSvr[0]->mWeight; //最大值
				if (pStatcs->mCur <= 0)
				{
					return -1;
				}
			}
		}
		//权重为0，禁用
		if (vSvr[pStatcs->mIdx]->mWeight == 0)
		{
			continue;
		}
		else if (vSvr[pStatcs->mIdx]->mShutdown == 1)	//是否故障
		{
			vSvr[pStatcs->mIdx]->mRfuNum++;
		}
		else if (vSvr[pStatcs->mIdx]->mWeight >= pStatcs->mCur)
		{
			if (vSvr[pStatcs->mIdx]->mPreNum <= 0 && (vSvr[pStatcs->mIdx]->mOverLoad == 1 || CalcOverLoad(vSvr[pStatcs->mIdx]) == 1))	//过载
			{
				vSvr[pStatcs->mIdx]->mRfuNum++;
			} 
			else
			{
				if(vSvr[pStatcs->mIdx]->mOverLoad == 1 || CalcOverLoad(vSvr[pStatcs->mIdx]) == 1)
				{
					vSvr[pStatcs->mIdx]->mPreNum--;
				}
				pStatcs->mSumConn++;
				vSvr[pStatcs->mIdx]->mUsedNum++;
				
				*pBuffer = (struct SvrNet_t) *vSvr[pStatcs->mIdx];	//Svr_t => SvrNet_t
				return vSvr[pStatcs->mIdx]->mId;
			}
		}
	}
	return -1;
}

//接受上报数据
void SvrQos::ReportSvr(SvrReportReqId_t *pReportSvr)
{
	vector<Svr_t*>::iterator it = GetItById(pReportSvr->mId);
	if (it != mSvr.end() && pReportSvr->mDelay != 0 && pReportSvr->mStu != SVR_UNKNOWN)
	{
		SetGXDirty((*it), 1);	//设置同组所有Svr更新位(重新设置对应权值、错误率)
		(*it)->mDelay = pReportSvr->mDelay;
		if (pReportSvr->mStu == SVR_SUC)	//成功
		{
			//(*it)->mOkNum++;
		}
		else if(pReportSvr->mStu == SVR_ERR)	//失败
		{
			//(*it)->mErrNum++;
		}

		//释放连接
		ReleaseConn(*it);
		LOG_DEBUG(ELOG_KEY,"[runtime] recvive a report message(shm), Ok(%d),Err(%d)", (*it)->mOkNum, (*it)->mErrNum);
	}
}

void SvrQos::Statistics()
{
	if(mSvr.size() <= 0) return;

	int iSunConn = -1;
	vector<Svr_t*>::iterator it;
	for (it = mSvr.begin(); it != mSvr.end(); it++)
	{
		if ((*it)->mDirty == 1)
		{
			iSunConn = GetSumConn(*it);
			(*it)->mOkRate = (float)(*it)->mOkNum / iSunConn;
			(*it)->mErrRate = (float)(*it)->mErrNum / iSunConn;
			(*it)->mRfuRate = (float)(*it)->mRfuNum / iSunConn;

			(*it)->mPreNum = CalcPre(*it);	//每一周期预取数重置为1 （后期实现扩张、收缩）
			(*it)->mShutdown = CalcShutdown(*it);	//非压力故障
			(*it)->mWeight = CalcWeight(*it);
			
			(*it)->ClearStatistics();	//清除统计数据
		}
	}
	SvrRebuild();
	LOG_DEBUG(ELOG_KEY,"[runtime] statistics svr success");
}

short SvrQos::CalcPre(Svr_t* stSvr)
{
	return 1;
}

short SvrQos::CalcShutdown(Svr_t* stSvr)
{
	return 0;
}

//计算动态权重
int SvrQos::CalcWeight(Svr_t* stSvr)
{
	if (stSvr->mDelay == 0 && stSvr->mOkRate == 0)	//从未访问过
	{
		return stSvr->mWeight;
	}

	Svr_t pSvr[MAX_SVR_NUM];
	int iNum = GetAllSvrByGXid(pSvr, stSvr->mGid, stSvr->mXid);
	if (iNum <= 0)
	{
		return stSvr->mWeight;	//讲道理的话，这里不可能执行到（至少有其自身）
	}

	int iDelay = stSvr->mDelay;
	float fOkRate = stSvr->mOkRate;
	for (int i = 0; i < iNum; ++i)
	{
		if (iDelay > pSvr[i].mDelay && pSvr[i].mDelay != 0)
		{
			iDelay = pSvr[i].mDelay;	//最小延时
		}
		if (fOkRate < pSvr[i].mOkRate && pSvr[i].mOkRate != 0)
		{
			fOkRate = pSvr[i].mOkRate;	//最大成功率
		}
	}

	float fLoadWeight = 1/(((float)stSvr->mDelay/iDelay) * (fOkRate/stSvr->mOkRate));
	return (int)(fLoadWeight * ZOOM_WEIGHT * stSvr->mSWeight);	//放大
}

//检测是否过载
//TODO
short SvrQos::CalcOverLoad(Svr_t* stSvr)
{
	if (stSvr->mErrNum > 0 && stSvr->mPreNum <= 0)	//本周期
	{
		stSvr->mOverLoad = 1;	//过载
	}
	else if(stSvr->mErrRate > ERR_RATE_THRESHOLD)	//上一周期错误率
	{
		stSvr->mOverLoad = 1; //过载(存在故障可能性)
		//stSvr->mShutdown = 1;
	}
	else
	{
		stSvr->mOverLoad = 0;
	}
	return stSvr->mOverLoad;
}

//设置同组所有svr更新位
void SvrQos::SetGXDirty(Svr_t* stSvr, int iDirty)
{
	Svr_t pSvr[MAX_SVR_NUM];
	int iNum = GetAllSvrByGXid(pSvr, stSvr->mGid, stSvr->mXid);
	if (iNum <= 0)
	{
		return;	//讲道理的话，这里不可能执行到（至少有其自身）
	}
	for (int i = 0; i < iNum; ++i)
	{
		 pSvr[i].mDirty = iDirty;
	}
}

vector<Svr_t*>::iterator SvrQos::GetItFromV(Svr_t* pSvr)
{
	vector<Svr_t*>::iterator it;
	for (it = mSvr.begin(); it != mSvr.end(); it++)
	{
		if (**it == *pSvr)
		{
			break;
		}
	}
	return it;
}

vector<Svr_t*>::iterator SvrQos::GetItById(int iId)
{
	vector<Svr_t*>::iterator it;
	for (it = mSvr.begin(); it != mSvr.end(); it++)
	{
		if ((*it)->mId == iId)
		{
			break;
		}
	}
	return it;
}

bool SvrQos::IsChangeSvr(const SvrNet_t* pR1, const SvrNet_t* pR2)
{
	if (pR1->mVersion!=pR2->mVersion || pR1->mGid!=pR2->mGid || pR1->mXid!=pR2->mXid || pR1->mSWeight!=pR2->mSWeight || pR1->mPort!=pR2->mPort || strncmp(pR1->mIp,pR2->mIp,MAX_SVR_IP_LEN)!=0)
	{
		return true;
	}
	return false;
}

void SvrQos::DelContainer()
{
	mSvrByGXid.clear();
	mStatcsGXid.clear();
}

void SvrQos::Final()
{
	DelContainer();
	for(vector<Svr_t*>::iterator it = mSvr.begin(); it != mSvr.end(); it++)
	{
		SAFE_DELETE(*it);
	}
	for(map<string, StatcsGXid_t*>::iterator it = mStatcsGXid.begin(); it != mStatcsGXid.end(); it++)
	{
		SAFE_DELETE(it->second);
	}
	mSvr.clear();
}

//整理容器 & 同步其他容器
void SvrQos::SvrRebuild()
{
	if(mSvr.size() <= 0) return;
	sort(mSvr.begin(), mSvr.end(), GreaterSvr);	//降序排序

	DelContainer();
	string sGid, sXid, sGXid;
	vector<Svr_t*> vSvr;
	for(vector<Svr_t*>::iterator it = mSvr.begin(); it != mSvr.end(); it++)
	{
		sGid = Itos((*it)->mGid); 
		sXid = Itos((*it)->mXid);
		sGXid = sGid + "-" + sXid;

		//mSvrByGXid
		map<string, vector<Svr_t*> >::iterator mg = mSvrByGXid.find(sGXid);
		vSvr.clear();
		if(mg != mSvrByGXid.end())
		{
			vSvr = mg->second;
			mSvrByGXid.erase(mg);
		}
		vSvr.push_back(*it);
		mSvrByGXid.insert(pair<string, vector<Svr_t*> >(sGXid, vSvr));		
	}

	StatcsGXid_t *pStatcsGXid = 0;
	map<string, vector<Svr_t*> >::iterator it = mSvrByGXid.begin();
	for(; it != mSvrByGXid.end(); it++)
	{
		map<string, StatcsGXid_t*>::iterator mgx = mStatcsGXid.find(it->first);
		if(mgx == mStatcsGXid.end())
		{
			pStatcsGXid = new StatcsGXid_t();
		}
		else
		{
			pStatcsGXid = mgx->second;
			//mStatcsGXid.erase(mgx);
		}
		pStatcsGXid->mGcd = GcdWeight(it->second, it->second.size());
		mStatcsGXid.insert(pair<string, StatcsGXid_t*>(it->first, pStatcsGXid));
	}
}

void SvrQos::ReleaseConn(Svr_t* stSvr)
{
	string sGid, sXid, sGXid;
	sGid = Itos(stSvr->mGid); 
	sXid = Itos(stSvr->mXid);
	sGXid = sGid + "-" + sXid;
	map<string, StatcsGXid_t*>::iterator mgx = mStatcsGXid.find(sGXid);
	StatcsGXid_t *pStatcsGXid = 0;
	if(mgx == mStatcsGXid.end())
	{
		pStatcsGXid = mgx->second;
		pStatcsGXid->mSumConn--;
	}
}

int SvrQos::GetSumConn(Svr_t* stSvr)
{
	string sGid, sXid, sGXid;
	sGid = Itos(stSvr->mGid); 
	sXid = Itos(stSvr->mXid);
	sGXid = sGid + "-" + sXid;

	map<string, StatcsGXid_t*>::iterator mgx = mStatcsGXid.find(sGXid);
	StatcsGXid_t *pStatcsGXid = 0;
	if(mgx == mStatcsGXid.end())
	{
		pStatcsGXid = mgx->second;
		return pStatcsGXid->mSumConn;
	}
	return -1;
}

int SvrQos::GcdWeight(vector<Svr_t*> vSvr, int n)
{
	if (n == 1) 
	{
		return vSvr[0]->mWeight;
	}
	return Gcd(vSvr[n-1]->mWeight, GcdWeight(vSvr, n - 1));
}