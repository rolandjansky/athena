// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// -------------------------------------------------------------
//  author: Tim Martin <Tim.Martin@cern.ch>
// -------------------------------------------------------------

// STL include(s):
#include <map>
#include <utility> //std::pair
#include <iostream>
#include <assert.h>

//Local include(s):
#include "../TrigCostRootAnalysis/TrigConfInterface.h"
#include "../TrigCostRootAnalysis/MonitorROS.h"
#include "../TrigCostRootAnalysis/CounterROB.h"
#include "../TrigCostRootAnalysis/TrigCostData.h"
#include "../TrigCostRootAnalysis/ROSConfService.h"
#include "../TrigCostRootAnalysis/Config.h"

// ROOT includes
#include <TError.h>

namespace TrigCostRootAnalysis {
  /**
   * Monitor constructor. Sets name and calls base constructor.
   */
  MonitorROS::MonitorROS(const TrigCostData* costData) : MonitorBase(costData, "ROS") {
    m_dummyCounter = newCounter(Config::config().getStr(kDummyString), INT_MIN);
    allowSameIDCounters();
  }

  /**
   * Process new event for this monitor.
   * For the ROS, all ROS data is looped over and recorded.
   * @param weight The event weight.
   */
  void MonitorROS::newEvent(Float_t weight) {
    m_timer.start();
    if (Config::config().debug()) {
      Int_t N = 0;
      for (UInt_t rob = 0; rob < m_costData->getNROBs(); ++rob) {
        N += m_costData->getROBDataN(rob);
      }
      Info("MonitorROS::newEvent", "*** Processing ROS ***  Size %i ***", N);
    }

    //Now loop over the counter collections;
    for (CounterMapSetIt_t cmsIt = m_collectionsToProcess.begin(); cmsIt != m_collectionsToProcess.end(); ++cmsIt) {
      CounterMap_t* counterMap = *cmsIt;

      startEvent();

      for (UInt_t robReq = 0; robReq < m_costData->getNROBs(); ++robReq) {
        // We want to call each ROS once, but the inner loop is over ROBINs
        // We now do this mapping in advance
        StringIntSetMap_t ROSMapping = getROSMapping(robReq);
        for (StringIntSetMapIt_t reqIt = ROSMapping.begin(); reqIt != ROSMapping.end(); ++reqIt) {
          // Get the counter - note we do not store any ID here
          CounterBase* counter = getCounter(counterMap, (*reqIt).first, 0 /*not used*/);
          // This lets the counter know it should ask its parent for the full set of ROBINs to collate
          if (counter->getCalls() == 0) {
            counter->decorate(kDecType, Config::config().getStr(kROSString));
            counter->decorate(kDecMyROS, (*reqIt).first);
          }
          counter->processEventCounter(robReq, UINT_MAX /*not used*/, weight);
        }
      }

      endEvent(weight);
    }
    m_timer.stop();
  }

  /**
   * Do we use this monitor for this particular mode? Try and keep things managable in terms of output created!
   * Note these are currently hard-coded. We may want to make them configurable
   * @return If this monitor should be active for a given mode.
   */
  Bool_t MonitorROS::getIfActive(ConfKey_t mode) {
    switch (mode) {
    case kDoAllSummary:       return kTRUE;

    case kDoKeySummary:       return kTRUE;

    case kDoLumiBlockSummary: return kTRUE;

    default: Error("MonitorROS::getIfActive", "An invalid summary mode was provided (key %s)",
                   Config::config().getName(mode).c_str());
    }
    return kFALSE;
  }

  /**
   * Save the results from this monitors counters as specified in the configuration.
   */
  void MonitorROS::saveOutput() {
    m_filterOutput = kTRUE; // Apply any user-specified name filter to output

    VariableOptionVector_t toSave = m_dummyCounter->getAllHistograms();
    sharedHistogramOutputRoutine(toSave);

    std::vector<TableColumnFormatter> toSaveTable;
    addCommonTableEntries(toSaveTable);
    sharedTableOutputRoutine(toSaveTable);
  }

  /**
   * Construct new counter of correct derived type, pass back as base type.
   * This function must be implemented by all derived monitor types.
   * @see MonitorBase::addCounter( const std::string &_name, Int_t ID )
   * @param name Cost reference to name of counter.
   * @param ID Reference to ID number of counter.
   * @returns Base class pointer to new counter object of correct derived type.
   */
  CounterBase* MonitorROS::newCounter(const std::string& name, Int_t ID) {
    return new CounterROB(m_costData, name, ID, m_detailLevel, (MonitorBase*) this);
  }
} // namespace TrigCostRootAnalysis
