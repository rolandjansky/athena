/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef EVENTATHENAPOOL_EVENTSTREAMINFOCNV_P3_H
#define EVENTATHENAPOOL_EVENTSTREAMINFOCNV_P3_H

/** @file EventStreamInfoCnv_p3.h
 *  @brief This file contains the class definition for the EventStreamInfoCnv_p3 class.
 *  @author Peter van Gemmeren <gemmeren@anl.gov>
 *  $Id: EventStreamInfoCnv_p3.h,v 1.1 2009-03-18 17:42:44 gemmeren Exp $
 **/

#include "EventInfo/EventStreamInfo.h"
#include "EventTPCnv/EventStreamInfo_p3.h"
#include "AthenaPoolCnvSvc/T_AthenaPoolTPConverter.h"

class MsgStream;

/** @class EventStreamInfoCnv_p3
 *  @brief This class provides the converter to customize the saving of EventStreamInfo_p3.
 **/
class EventStreamInfoCnv_p3 : public T_AthenaPoolTPCnvBase<EventStreamInfo, EventStreamInfo_p3>  {
public:
   EventStreamInfoCnv_p3() {}

   virtual void persToTrans(const EventStreamInfo_p3* persObj, EventStreamInfo* transObj, MsgStream &log);
   virtual void transToPers(const EventStreamInfo* transObj, EventStreamInfo_p3* persObj, MsgStream &log);
};

template<>
class T_TPCnv<EventStreamInfo, EventStreamInfo_p3> : public EventStreamInfoCnv_p3 {
public:
};

#endif
