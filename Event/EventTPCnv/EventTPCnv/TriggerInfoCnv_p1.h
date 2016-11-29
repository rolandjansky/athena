/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef EVENTATHENAPOOL_TRIGGERINFOCNV_P1_H
#define EVENTATHENAPOOL_TRIGGERINFOCNV_P1_H

#include "EventTPCnv/TriggerInfo_p1.h"
#include "EventInfo/TriggerInfo.h"
#include "AthenaPoolCnvSvc/T_AthenaPoolTPConverter.h"

class MsgStream;
class TriggerInfoCnv_p1  : public T_AthenaPoolTPCnvBase<TriggerInfo, TriggerInfo_p1>  {
public:
  TriggerInfoCnv_p1() {}
  virtual void   persToTrans(const TriggerInfo_p1* persObj, TriggerInfo* transObj, MsgStream &log) override;
  virtual void   transToPers(const TriggerInfo* transObj, TriggerInfo_p1* persObj, MsgStream &log) override;
  void   persToTrans(const TriggerInfo_p1* persObj, TriggerInfo* transObj, MsgStream &log) const;
  void   transToPers(const TriggerInfo* transObj, TriggerInfo_p1* persObj, MsgStream &log) const;

  virtual TriggerInfo* createTransient (const TriggerInfo_p1* persObj, MsgStream& log) override;
  TriggerInfo* createTransient (const TriggerInfo_p1* persObj, MsgStream& log) const;

  virtual TriggerInfo_p1* createPersistent (const TriggerInfo* transObj, MsgStream& log) override;
  TriggerInfo_p1* createPersistent (const TriggerInfo* transObj, MsgStream& log) const;
};

#endif

