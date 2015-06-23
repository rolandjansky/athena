/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef EVENTATHENAPOOL_TRIGGERINFOCNV_P2_H
#define EVENTATHENAPOOL_TRIGGERINFOCNV_P2_H

#include "EventTPCnv/TriggerInfo_p2.h"
#include "AthenaPoolCnvSvc/T_AthenaPoolTPConverter.h"

class MsgStream;
class TriggerInfoCnv_p2  : public T_AthenaPoolTPCnvBase<TriggerInfo, TriggerInfo_p2>  {
public:
  TriggerInfoCnv_p2() {}
  virtual void   persToTrans(const TriggerInfo_p2* persObj, TriggerInfo* transObj, MsgStream &log) ;
  virtual void   transToPers(const TriggerInfo* transObj, TriggerInfo_p2* persObj, MsgStream &log) ;
};

#endif

