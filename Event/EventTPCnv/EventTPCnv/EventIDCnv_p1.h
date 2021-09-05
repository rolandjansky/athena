/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#ifndef EVENTATHENAPOOL_EVENTIDCNV_P1_H
#define EVENTATHENAPOOL_EVENTIDCNV_P1_H

#include "EventInfo/EventID.h"
#include "EventTPCnv/EventID_p1.h"
#include "AthenaPoolCnvSvc/T_AthenaPoolTPConverter.h"

class MsgStream;

class EventIDCnv_p1  : public T_AthenaPoolTPCnvConstBase<EventID, EventID_p1>  {
public:
  EventIDCnv_p1() {}
  using base_class::persToTrans;
  using base_class::transToPers;

  virtual void   persToTrans(const EventID_p1* persObj, EventID* transObj, MsgStream &log) const override;
  virtual void   transToPers(const EventID* transObj, EventID_p1* persObj, MsgStream &log) const override;

  virtual EventID* createTransientConst (const EventID_p1* persObj, MsgStream& log) const override;
};

#endif
