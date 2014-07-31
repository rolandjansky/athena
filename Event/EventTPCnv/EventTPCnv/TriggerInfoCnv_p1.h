/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef EVENTATHENAPOOL_TRIGGERINFOCNV_P1_H
#define EVENTATHENAPOOL_TRIGGERINFOCNV_P1_H

#include "EventTPCnv/TriggerInfo_p1.h"
#include "AthenaPoolCnvSvc/T_AthenaPoolTPConverter.h"

class MsgStream;
class TriggerInfoCnv_p1  : public T_AthenaPoolTPCnvBase<TriggerInfo, TriggerInfo_p1>  {
public:
  TriggerInfoCnv_p1() {}
  virtual void   persToTrans(const TriggerInfo_p1* persObj, TriggerInfo* transObj, MsgStream &log) ;
  virtual void   transToPers(const TriggerInfo* transObj, TriggerInfo_p1* persObj, MsgStream &log) ;
};

#endif

