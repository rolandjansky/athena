/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// Author: David Cote <david.cote@cern.ch>
#ifndef EVENTBOOKKEEPERCNV_P1_H
#define EVENTBOOKKEEPERCNV_P1_H

#include "EventBookkeeperMetaData/EventBookkeeper.h"
#include "EventBookkeeperTPCnv/EventBookkeeper_p1.h"
#include "AthenaPoolCnvSvc/T_AthenaPoolTPConverter.h"

class MsgStream;
class EventBookkeeperCnv_p1  : public T_AthenaPoolTPCnvBase<EventBookkeeper, EventBookkeeper_p1> {
 public:
  EventBookkeeperCnv_p1() {}
  virtual void   persToTrans(const EventBookkeeper_p1* persObj, EventBookkeeper* transObj, MsgStream &log);
  virtual void   transToPers(const EventBookkeeper* transObj, EventBookkeeper_p1* persObj, MsgStream &log);
};

template<>
class T_TPCnv<EventBookkeeper, EventBookkeeper_p1>
  : public EventBookkeeperCnv_p1
{
public:
};

#endif // EVENTBOOKKEEPERCNV_P1_H
