/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// generate the T/P converter entries
#include "AthenaKernel/TPCnvFactory.h"

// Configuration
#include "TrigMonitoringEventTPCnv/TrigConfAlg_p1.h"
#include "TrigMonitoringEventTPCnv/TrigConfChain_p1.h"
#include "TrigMonitoringEventTPCnv/TrigConfSeq_p1.h"
#include "TrigMonitoringEventTPCnv/TrigConfSig_p1.h"

// Event data
#include "TrigMonitoringEventTPCnv/TrigMonAlg_p1.h"
#include "TrigMonitoringEventTPCnv/TrigMonROB_p1.h"
#include "TrigMonitoringEventTPCnv/TrigMonROBData_p1.h"
#include "TrigMonitoringEventTPCnv/TrigMonROBData_p2.h"
#include "TrigMonitoringEventTPCnv/TrigMonRoi_p1.h"
#include "TrigMonitoringEventTPCnv/TrigMonSeq_p1.h"
#include "TrigMonitoringEventTPCnv/TrigMonTE_p1.h"

// TrigMonConfig headers
#include "TrigMonitoringEventTPCnv/TrigMonConfig_p1.h"
#include "TrigMonitoringEventTPCnv/TrigMonConfigCnv_p1.h"
#include "TrigMonitoringEventTPCnv/TrigMonConfigCollection_p1.h"
#include "TrigMonitoringEventTPCnv/TrigMonConfigCollection_tlp1.h"
#include "TrigMonitoringEventTPCnv/TrigMonConfigCollectionCnv_tlp1.h"

// TrigMonEvent headers
#include "TrigMonitoringEventTPCnv/TrigMonEvent_p1.h"
#include "TrigMonitoringEventTPCnv/TrigMonEventCnv_p1.h"
#include "TrigMonitoringEventTPCnv/TrigMonEventCollection_p1.h"
#include "TrigMonitoringEventTPCnv/TrigMonEventCollection_tlp1.h"
#include "TrigMonitoringEventTPCnv/TrigMonEventCollectionCnv_tlp1.h"


DECLARE_TPCNV_FACTORY(TrigMonConfigCollectionCnv_tlp1,
                      TrigMonConfigCollection,
                      TrigMonConfigCollection_tlp1,
                      Athena::TPCnvVers::Current)
                      
DECLARE_TPCNV_FACTORY(TrigMonEventCollectionCnv_tlp1,
                      TrigMonEventCollection,
                      TrigMonEventCollection_tlp1,
                      Athena::TPCnvVers::Current)
