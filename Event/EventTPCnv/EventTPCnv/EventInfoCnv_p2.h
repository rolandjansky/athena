/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef EVENTATHENAPOOL_EVENTINFOCNV_P2_H
#define EVENTATHENAPOOL_EVENTINFOCNV_P2_H

#include "EventTPCnv/EventInfo_p2.h"
#include "AthenaPoolCnvSvc/T_AthenaPoolTPConverter.h"

class MsgStream;
class EventInfoCnv_p2  : public T_AthenaPoolTPCnvBase<EventInfo, EventInfo_p2>  {
public:
  EventInfoCnv_p2() {}
  virtual void   persToTrans(const EventInfo_p2* persObj, EventInfo* transObj, MsgStream &log) ;
  virtual void   transToPers(const EventInfo* transObj, EventInfo_p2* persObj, MsgStream &log) ;

  // needed to handle specific default constructor of EventInfo
  virtual EventInfo *createTransient( const EventInfo_p2* persObj, MsgStream &log);
};

template<>
class T_TPCnv<EventInfo, EventInfo_p2>
  : public EventInfoCnv_p2
{
public:
};


#endif
