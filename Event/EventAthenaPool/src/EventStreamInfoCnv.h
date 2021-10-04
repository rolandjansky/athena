/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#ifndef EVENTSTREAMINFOCNV_H
#define EVENTSTREAMINFOCNV_H

/** @file EventStreamInfoCnv.h
 *  @brief This file contains the class definition for the EventStreamInfoCnv class.
 *  @author Peter van Gemmeren <gemmeren@anl.gov>
 *  $Id: EventStreamInfoCnv.h,v 1.3 2009-03-18 17:48:04 gemmeren Exp $
 **/


#include "AthenaPoolCnvSvc/T_AthenaPoolTPCnvCnv.h"
#include "EventInfo/EventStreamInfo.h"
#include "EventTPCnv/EventStreamInfoCnv_p1.h"
#include "EventTPCnv/EventStreamInfoCnv_p2.h"
#include "EventTPCnv/EventStreamInfoCnv_p3.h"


typedef T_AthenaPoolTPCnvCnv<EventStreamInfo,
                             EventStreamInfoCnv_p3,
                             EventStreamInfoCnv_p2,
                             EventStreamInfoCnv_p1,
                             T_TPCnvNull<EventStreamInfo> >
  EventStreamInfoCnv;


#endif
