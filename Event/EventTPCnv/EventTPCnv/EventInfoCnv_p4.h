/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef EVENTATHENAPOOL_EVENTINFOCNV_P4_H
#define EVENTATHENAPOOL_EVENTINFOCNV_P4_H

#include "EventTPCnv/EventInfo_p4.h"
#include "AthenaPoolCnvSvc/T_AthenaPoolTPConverter.h"

class MsgStream;
class EventInfoCnv_p4  : public T_AthenaPoolTPCnvBase<EventInfo, EventInfo_p4>  {
public:
  EventInfoCnv_p4() {}
  virtual void   persToTrans(const EventInfo_p4* persObj, EventInfo* transObj, MsgStream &log) ;
  virtual void   transToPers(const EventInfo* transObj, EventInfo_p4* persObj, MsgStream &log) ;

  // needed to handle specific default constructor of EventInfo
  virtual EventInfo *createTransient( const EventInfo_p4* persObj, MsgStream &log);
};

template<>
class T_TPCnv<EventInfo, EventInfo_p4>
  : public EventInfoCnv_p4
{
public:
};


#endif
