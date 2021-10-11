/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#ifndef EVENTATHENAPOOL_EVENTINFOCNV_P1_H
#define EVENTATHENAPOOL_EVENTINFOCNV_P1_H

#include "EventInfo/EventInfo.h"
#include "EventTPCnv/EventInfo_p1.h"
#include "AthenaPoolCnvSvc/T_AthenaPoolTPConverter.h"

class MsgStream;
class EventInfoCnv_p1  : public T_AthenaPoolTPCnvConstBase<EventInfo, EventInfo_p1>  {
public:
  EventInfoCnv_p1() {}
  using base_class::persToTrans;
  using base_class::transToPers;

  virtual void   persToTrans(const EventInfo_p1* persObj, EventInfo* transObj, MsgStream &log) const override;
  virtual void   transToPers(const EventInfo* transObj, EventInfo_p1* persObj, MsgStream &log) const override;

  // needed to handle specific default constructor of EventInfo
  virtual EventInfo *createTransientConst( const EventInfo_p1* persObj, MsgStream &log) const override;
};

template<>
class T_TPCnv<EventInfo, EventInfo_p1>
  : public EventInfoCnv_p1
{
public:
};

#endif
