/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// generate the T/P converter entries
#include "AthenaKernel/TPCnvFactory.h"

#include "EventTPCnv/EventID_p1.h"
#include "EventTPCnv/EventType_p1.h"
#include "EventTPCnv/TriggerInfo_p1.h"
#include "EventTPCnv/TriggerInfo_p2.h"
#include "EventTPCnv/EventInfo_p1.h"
#include "EventTPCnv/EventInfo_p2.h"
#include "EventTPCnv/EventInfo_p3.h"
#include "EventTPCnv/EventInfo_p4.h"
#include "EventTPCnv/EventStreamInfo_p1.h"
#include "EventTPCnv/EventStreamInfo_p2.h"
#include "EventTPCnv/EventStreamInfo_p3.h"
#include "EventTPCnv/MergedEventInfo_p1.h"
#include "EventTPCnv/MergedEventInfo_p2.h"
#include "EventTPCnv/PileUpEventInfo_p1.h"
#include "EventTPCnv/PileUpEventInfo_p2.h"
#include "EventTPCnv/PileUpEventInfo_p3.h"
#include "EventTPCnv/PileUpEventInfo_p4.h"
#include "EventTPCnv/PileUpEventInfo_p5.h"
#include "EventTPCnv/EventInfoCnv_p1.h"
#include "EventTPCnv/EventInfoCnv_p2.h"
#include "EventTPCnv/EventInfoCnv_p3.h"
#include "EventTPCnv/EventInfoCnv_p4.h"
#include "EventTPCnv/EventStreamInfoCnv_p1.h"
#include "EventTPCnv/EventStreamInfoCnv_p2.h"
#include "EventTPCnv/EventStreamInfoCnv_p3.h"
#include "EventTPCnv/MergedEventInfoCnv_p1.h"
#include "EventTPCnv/MergedEventInfoCnv_p2.h"
#include "EventTPCnv/PileUpEventInfoCnv_p1.h"
#include "EventTPCnv/PileUpEventInfoCnv_p2.h"
#include "EventTPCnv/PileUpEventInfoCnv_p3.h"
#include "EventTPCnv/PileUpEventInfoCnv_p4.h"
#include "EventTPCnv/PileUpEventInfoCnv_p5.h"

DECLARE_TPCNV_FACTORY(EventInfoCnv_p1,
                      EventInfo,
                      EventInfo_p1,
                      Athena::TPCnvVers::Old)
                      
DECLARE_TPCNV_FACTORY(EventInfoCnv_p2,
                      EventInfo,
                      EventInfo_p2,
                      Athena::TPCnvVers::Old)
                      
DECLARE_TPCNV_FACTORY(EventInfoCnv_p3,
                      EventInfo,
                      EventInfo_p3,
                      Athena::TPCnvVers::Old)
                      
DECLARE_TPCNV_FACTORY(EventInfoCnv_p4,
                      EventInfo,
                      EventInfo_p4,
                      Athena::TPCnvVers::Current)
                      
DECLARE_TPCNV_FACTORY(EventStreamInfoCnv_p1,
                      EventStreamInfo,
                      EventStreamInfo_p1,
                      Athena::TPCnvVers::Old)
                      
DECLARE_TPCNV_FACTORY(EventStreamInfoCnv_p2,
                      EventStreamInfo,
                      EventStreamInfo_p2,
                      Athena::TPCnvVers::Old)
                      
DECLARE_TPCNV_FACTORY(EventStreamInfoCnv_p3,
                      EventStreamInfo,
                      EventStreamInfo_p3,
                      Athena::TPCnvVers::Current)
                      
DECLARE_TPCNV_FACTORY(MergedEventInfoCnv_p1,
                      MergedEventInfo,
                      MergedEventInfo_p1,
                      Athena::TPCnvVers::Old)
                      
DECLARE_TPCNV_FACTORY(MergedEventInfoCnv_p2,
                      MergedEventInfo,
                      MergedEventInfo_p2,
                      Athena::TPCnvVers::Current)
                      
DECLARE_TPCNV_FACTORY(PileUpEventInfoCnv_p1,
                      PileUpEventInfo,
                      PileUpEventInfo_p1,
                      Athena::TPCnvVers::Old)
                      
DECLARE_TPCNV_FACTORY(PileUpEventInfoCnv_p2,
                      PileUpEventInfo,
                      PileUpEventInfo_p2,
                      Athena::TPCnvVers::Old)
                      
DECLARE_TPCNV_FACTORY(PileUpEventInfoCnv_p3,
                      PileUpEventInfo,
                      PileUpEventInfo_p3,
                      Athena::TPCnvVers::Old)
                      
DECLARE_TPCNV_FACTORY(PileUpEventInfoCnv_p4,
                      PileUpEventInfo,
                      PileUpEventInfo_p4,
                      Athena::TPCnvVers::Old)
                      
DECLARE_TPCNV_FACTORY(PileUpEventInfoCnv_p5,
                      PileUpEventInfo,
                      PileUpEventInfo_p5,
                      Athena::TPCnvVers::Current)
