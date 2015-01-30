/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGMONITORINGEVENT_TCPCNV_DICT_H
#define TRIGMONITORINGEVENT_TCPCNV_DICT_H

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
#include "TrigMonitoringEventTPCnv/TrigMonConfigCollection_p1.h"
#include "TrigMonitoringEventTPCnv/TrigMonConfigCollection_tlp1.h"

// TrigMonEvent headers
#include "TrigMonitoringEventTPCnv/TrigMonEvent_p1.h"
#include "TrigMonitoringEventTPCnv/TrigMonEventCollection_p1.h"
#include "TrigMonitoringEventTPCnv/TrigMonEventCollection_tlp1.h"

#ifdef __GCCXML__
template class std::vector<TrigConfAlg_p1>;
template class std::vector<TrigConfChain_p1>;
template class std::vector<TrigConfSeq_p1>;
template class std::vector<TrigConfSig_p1>;

template class std::vector<TrigMonAlg_p1>;
template class std::vector<TrigMonEvent_p1>;
template class std::vector<TrigMonConfig_p1>;
template class std::vector<TrigMonROB_p1>;
template class std::vector<TrigMonROBData_p1>;
template class std::vector<TrigMonROBData_p2>;
template class std::vector<TrigMonRoi_p1>;
template class std::vector<TrigMonSeq_p1>;
template class std::vector<TrigMonTE_p1>;

#endif
#endif
