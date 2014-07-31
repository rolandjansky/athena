/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef EVENTATHENAPOOL_EVENTTYPECNV_P1_H
#define EVENTATHENAPOOL_EVENTTYPECNV_P1_H

#include "EventTPCnv/EventType_p1.h"
#include "AthenaPoolCnvSvc/T_AthenaPoolTPConverter.h"

class MsgStream;
class EventTypeCnv_p1  : public T_AthenaPoolTPCnvBase<EventType, EventType_p1>  {
public:
  EventTypeCnv_p1() {}
  virtual void   persToTrans(const EventType_p1* persObj, EventType* transObj, MsgStream &log);
  virtual void   transToPers(const EventType* transObj, EventType_p1* persObj, MsgStream &log);
};

#endif
