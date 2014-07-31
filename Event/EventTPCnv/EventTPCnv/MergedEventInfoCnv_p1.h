/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef EVENTATHENAPOOL_MERGEDEVENTINFOCNV_P1_H
#define EVENTATHENAPOOL_MERGEDEVENTINFOCNV_P1_H

#include "EventInfo/MergedEventInfo.h"
#include "EventTPCnv/MergedEventInfo_p1.h"
#include "AthenaPoolCnvSvc/T_AthenaPoolTPConverter.h"

class MsgStream;
class MergedEventInfoCnv_p1  : public T_AthenaPoolTPCnvBase<MergedEventInfo, MergedEventInfo_p1>  {
public:
  MergedEventInfoCnv_p1() {}
  virtual void   persToTrans(const MergedEventInfo_p1* persObj, MergedEventInfo* transObj, MsgStream &log) ;
  virtual void   transToPers(const MergedEventInfo* transObj, MergedEventInfo_p1* persObj, MsgStream &log) ;
};

template<>
class T_TPCnv<MergedEventInfo, MergedEventInfo_p1>
  : public MergedEventInfoCnv_p1
{
public:
};

#endif
