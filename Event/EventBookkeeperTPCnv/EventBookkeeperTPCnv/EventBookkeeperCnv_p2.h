/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// Author: David Cote <david.cote@cern.ch>
#ifndef EVENTBOOKKEEPERCNV_p2_H
#define EVENTBOOKKEEPERCNV_p2_H

#include "EventBookkeeperMetaData/EventBookkeeper.h"
#include "EventBookkeeperTPCnv/EventBookkeeper_p2.h"
#include "AthenaPoolCnvSvc/T_AthenaPoolTPConverter.h"

class MsgStream;
class EventBookkeeperCnv_p2  : public T_AthenaPoolTPCnvBase<EventBookkeeper, EventBookkeeper_p2> {
 public:
  EventBookkeeperCnv_p2() {}
  virtual void   persToTrans(const EventBookkeeper_p2* persObj, EventBookkeeper* transObj, MsgStream &log);
  virtual void   transToPers(const EventBookkeeper* transObj, EventBookkeeper_p2* persObj, MsgStream &log);
};

template<>
class T_TPCnv<EventBookkeeper, EventBookkeeper_p2>
  : public EventBookkeeperCnv_p2
{
public:
};

#endif // EVENTBOOKKEEPERCNV_p2_H
