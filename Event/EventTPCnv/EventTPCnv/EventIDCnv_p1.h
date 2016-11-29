/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef EVENTATHENAPOOL_EVENTIDCNV_P1_H
#define EVENTATHENAPOOL_EVENTIDCNV_P1_H

#include "EventInfo/EventID.h"
#include "EventTPCnv/EventID_p1.h"
#include "AthenaPoolCnvSvc/T_AthenaPoolTPConverter.h"

class MsgStream;

class EventIDCnv_p1  : public T_AthenaPoolTPCnvBase<EventID, EventID_p1>  {
public:
  EventIDCnv_p1() {}
  virtual void   persToTrans(const EventID_p1* persObj, EventID* transObj, MsgStream &log) override;
  virtual void   transToPers(const EventID* transObj, EventID_p1* persObj, MsgStream &log) override;
  void   persToTrans(const EventID_p1* persObj, EventID* transObj, MsgStream &log) const;
  void   transToPers(const EventID* transObj, EventID_p1* persObj, MsgStream &log) const;

  virtual EventID* createTransient (const EventID_p1* persObj, MsgStream& log) override;
  EventID* createTransient (const EventID_p1* persObj, MsgStream& log) const;
};

#endif
