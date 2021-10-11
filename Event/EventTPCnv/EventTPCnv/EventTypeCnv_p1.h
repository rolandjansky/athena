/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#ifndef EVENTATHENAPOOL_EVENTTYPECNV_P1_H
#define EVENTATHENAPOOL_EVENTTYPECNV_P1_H

#include "EventTPCnv/EventType_p1.h"
#include "EventInfo/EventType.h"
#include "AthenaPoolCnvSvc/T_AthenaPoolTPConverter.h"

class MsgStream;
class EventTypeCnv_p1  : public T_AthenaPoolTPCnvConstBase<EventType, EventType_p1>  {
public:
  EventTypeCnv_p1() {}
  using base_class::persToTrans;
  using base_class::transToPers;

  virtual void   persToTrans(const EventType_p1* persObj, EventType* transObj, MsgStream &log) const override;
  virtual void   transToPers(const EventType* transObj, EventType_p1* persObj, MsgStream &log) const override;

  virtual EventType* createTransientConst (const EventType_p1* persObj, MsgStream& log) const override;
};

#endif
