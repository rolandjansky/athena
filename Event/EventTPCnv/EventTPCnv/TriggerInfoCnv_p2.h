/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#ifndef EVENTATHENAPOOL_TRIGGERINFOCNV_P2_H
#define EVENTATHENAPOOL_TRIGGERINFOCNV_P2_H

#include "EventTPCnv/TriggerInfo_p2.h"
#include "EventInfo/TriggerInfo.h"
#include "AthenaPoolCnvSvc/T_AthenaPoolTPConverter.h"

class MsgStream;
class TriggerInfoCnv_p2  : public T_AthenaPoolTPCnvConstBase<TriggerInfo, TriggerInfo_p2>  {
public:
  TriggerInfoCnv_p2() {}
  using base_class::persToTrans;
  using base_class::transToPers;

  virtual void   persToTrans(const TriggerInfo_p2* persObj, TriggerInfo* transObj, MsgStream &log) const override;
  virtual void   transToPers(const TriggerInfo* transObj, TriggerInfo_p2* persObj, MsgStream &log) const override;

  virtual TriggerInfo* createTransientConst (const TriggerInfo_p2* persObj, MsgStream& log) const override;
};

#endif

