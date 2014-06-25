/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGMON_TRIGMONITORINGEVENT_DICT_H
#define TRIGMON_TRIGMONITORINGEVENT_DICT_H

// Configuration
#include "TrigMonitoringEvent/TrigConfAlg.h"
#include "TrigMonitoringEvent/TrigConfChain.h"
#include "TrigMonitoringEvent/TrigConfSeq.h"
#include "TrigMonitoringEvent/TrigConfSig.h"
#include "TrigMonitoringEvent/TrigConfVar.h"

// Event data
#include "TrigMonitoringEvent/TrigMonAlg.h"
#include "TrigMonitoringEvent/TrigMonChain.h"
#include "TrigMonitoringEvent/TrigMonConfig.h"
#include "TrigMonitoringEvent/TrigMonEvent.h"
#include "TrigMonitoringEvent/TrigMonL1Item.h"
#include "TrigMonitoringEvent/TrigMonROB.h"
#include "TrigMonitoringEvent/TrigMonROBData.h"
#include "TrigMonitoringEvent/TrigMonROBSum.h"
#include "TrigMonitoringEvent/TrigMonRoi.h"
#include "TrigMonitoringEvent/TrigMonSeq.h"
#include "TrigMonitoringEvent/TrigMonTE.h"
#include "TrigMonitoringEvent/TrigMonTimer.h"
#include "TrigMonitoringEvent/TrigMonVar.h"

// DataVectors
#include "TrigMonitoringEvent/TrigMonConfigCollection.h"
#include "TrigMonitoringEvent/TrigMonEventCollection.h"

#ifdef __GCCXML__
template class std::vector<TrigConfAlg>;
template class std::vector<TrigConfChain>;
template class std::vector<TrigConfSeq>;
template class std::vector<TrigConfSig>;
template class std::vector<TrigConfVar>;

template class std::vector<TrigMonAlg>;
template class std::vector<TrigMonChain>;
template class std::vector<TrigMonConfig>;
template class std::vector<TrigMonEvent>;
template class std::vector<TrigMonL1Item>;
template class std::vector<TrigMonROB>;
template class std::vector<TrigMonROBData>;
template class std::vector<TrigMonROBSum>;
template class std::vector<TrigMonRoi>;
template class std::vector<TrigMonSeq>;
template class std::vector<TrigMonTE>;
template class std::vector<TrigMonTimer>;
template class std::vector<TrigMonVar>;
#endif
#endif
