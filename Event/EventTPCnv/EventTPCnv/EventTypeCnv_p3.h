/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#ifndef EVENTATHENAPOOL_EVENTTYPECNV_P3_H
#define EVENTATHENAPOOL_EVENTTYPECNV_P3_H

#include "EventTPCnv/EventType_p3.h"
#include "EventInfo/EventType.h"
#include "AthenaPoolCnvSvc/T_AthenaPoolTPConverter.h"

class MsgStream;
class EventTypeCnv_p3  : public T_AthenaPoolTPCnvConstBase<EventType, EventType_p3>  {
public:
  EventTypeCnv_p3() {}
  using base_class::persToTrans;
  using base_class::transToPers;

  virtual void   persToTrans(const EventType_p3* persObj, EventType* transObj, MsgStream &log) const override;
  virtual void   transToPers(const EventType* transObj, EventType_p3* persObj, MsgStream &log) const override;

  virtual EventType* createTransientConst (const EventType_p3* persObj, MsgStream& log) const override;
};

#endif
