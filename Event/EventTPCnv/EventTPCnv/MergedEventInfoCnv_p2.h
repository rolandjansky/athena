/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#ifndef EVENTATHENAPOOL_MERGEDEVENTINFOCNV_P2_H
#define EVENTATHENAPOOL_MERGEDEVENTINFOCNV_P2_H

#include "EventInfo/MergedEventInfo.h"
#include "EventTPCnv/MergedEventInfo_p2.h"
#include "AthenaPoolCnvSvc/T_AthenaPoolTPConverter.h"

class MsgStream;
class MergedEventInfoCnv_p2  : public T_AthenaPoolTPCnvConstBase<MergedEventInfo, MergedEventInfo_p2>  {
public:
  MergedEventInfoCnv_p2() {}
  using base_class::persToTrans;
  using base_class::transToPers;

  virtual void   persToTrans(const MergedEventInfo_p2* persObj, MergedEventInfo* transObj, MsgStream &log) const override;
  virtual void   transToPers(const MergedEventInfo* transObj, MergedEventInfo_p2* persObj, MsgStream &log) const override;
};

template<>
class T_TPCnv<MergedEventInfo, MergedEventInfo_p2>
  : public MergedEventInfoCnv_p2
{
public:
};

#endif
