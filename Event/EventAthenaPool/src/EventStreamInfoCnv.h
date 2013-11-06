/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef EVENTSTREAMINFOCNV_H
#define EVENTSTREAMINFOCNV_H

/** @file EventStreamInfoCnv.h
 *  @brief This file contains the class definition for the EventStreamInfoCnv class.
 *  @author Peter van Gemmeren <gemmeren@anl.gov>
 *  $Id: EventStreamInfoCnv.h,v 1.3 2009-03-18 17:48:04 gemmeren Exp $
 **/

#include "AthenaPoolCnvSvc/T_AthenaPoolCustomCnv.h"
#include "EventInfo/EventStreamInfo.h"
#include "EventTPCnv/EventStreamInfo_p3.h"

/// the latest persistent representation type of EventStreamInfo
typedef EventStreamInfo_p3 EventStreamInfo_PERS;
typedef T_AthenaPoolCustomCnv<EventStreamInfo, EventStreamInfo_PERS> EventStreamInfoCnvBase;

/** @class EventStreamInfoCnv
 *  @brief This class provides a converter for the EventStreamInfo class.
 **/
class EventStreamInfoCnv : public EventStreamInfoCnvBase {

friend class CnvFactory<EventStreamInfoCnv>;

protected:
   EventStreamInfoCnv(ISvcLocator* svcloc) : EventStreamInfoCnvBase(svcloc) {}

   virtual EventStreamInfo_PERS* createPersistent(EventStreamInfo* transObj);
   virtual EventStreamInfo* createTransient();
};

#endif
