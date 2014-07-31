/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef EVENTATHENAPOOL_EVENTSTREAMINFOCNV_P1_H
#define EVENTATHENAPOOL_EVENTSTREAMINFOCNV_P1_H

/** @file EventStreamInfoCnv_p1.h
 *  @brief This file contains the class definition for the EventStreamInfoCnv_p1 class.
 *  @author Peter van Gemmeren <gemmeren@anl.gov>
 *  $Id: EventStreamInfoCnv_p1.h,v 1.3 2009-02-09 22:13:27 gemmeren Exp $
 **/

#include "EventInfo/EventStreamInfo.h"
#include "EventTPCnv/EventStreamInfo_p1.h"
#include "AthenaPoolCnvSvc/T_AthenaPoolTPConverter.h"

class MsgStream;

/** @class EventStreamInfoCnv_p1
 *  @brief This class provides the converter to customize the saving of EventStreamInfo_p1.
 **/
class EventStreamInfoCnv_p1 : public T_AthenaPoolTPCnvBase<EventStreamInfo, EventStreamInfo_p1>  {
public:
   EventStreamInfoCnv_p1() {}

   virtual void persToTrans(const EventStreamInfo_p1* persObj, EventStreamInfo* transObj, MsgStream &log);
   virtual void transToPers(const EventStreamInfo* transObj, EventStreamInfo_p1* persObj, MsgStream &log);
};

template<>
class T_TPCnv<EventStreamInfo, EventStreamInfo_p1> : public EventStreamInfoCnv_p1 {
public:
};

#endif
