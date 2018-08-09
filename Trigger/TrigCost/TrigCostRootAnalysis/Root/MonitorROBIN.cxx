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
#include "../TrigCostRootAnalysis/MonitorROBIN.h"
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
  MonitorROBIN::MonitorROBIN(const TrigCostData* costData) : MonitorBase(costData, "ROBIN") {
    m_dummyCounter = newCounter(Config::config().getStr(kDummyString), INT_MIN);
    allowSameIDCounters();
  }

  /**
   * Process new event for this monitor.
   * For the ROS, all ROS data is looped over and recorded.
   * @param weight The event weight.
   */
  void MonitorROBIN::newEvent(Float_t weight) {
    m_timer.start();
    if (Config::config().debug()) {
      Int_t N = 0;
      for (UInt_t rob = 0; rob < m_costData->getNROBs(); ++rob) {
        N += m_costData->getROBDataN(rob);
      }
      Info("MonitorROBIN::newEvent", "*** Processing ROBIN ***  Size %i ***", N);
    }

    //Now loop over the counter collections;
    for (CounterMapSetIt_t cmsIt = m_collectionsToProcess.begin(); cmsIt != m_collectionsToProcess.end(); ++cmsIt) {
      CounterMap_t* counterMap = *cmsIt;

      startEvent();

      // All looping over individual RODs to do their event monitoring can go here
      for (UInt_t robReq = 0; robReq < m_costData->getNROBs(); ++robReq) {
        //Info("TEMP", "*** ROBIN MON *** REQUEST %i / %i", rob,  m_costData->getNROBs()  );

        StringIntSetMap_t ROBINMapping = getROBINMapping(robReq);
        for (StringIntSetMapIt_t reqIt = ROBINMapping.begin(); reqIt != ROBINMapping.end(); ++reqIt) {
          CounterBase* counter = getCounter(counterMap, (*reqIt).first, 0 /*not used*/);

          if (counter->getCalls() == 0) {
            counter->decorate(kDecType, Config::config().getStr(kROBINString));
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
  Bool_t MonitorROBIN::getIfActive(ConfKey_t mode) {
    switch (mode) {
    case kDoAllSummary:       return kTRUE;

    case kDoKeySummary:       return kTRUE;

    case kDoLumiBlockSummary: return kFALSE;

    default: Error("MonitorROBIN::getIfActive", "An invalid summary mode was provided (key %s)",
                   Config::config().getName(mode).c_str());
    }
    return kFALSE;
  }

  /**
   * Save the results from this monitors counters as specified in the configuration.
   */
  void MonitorROBIN::saveOutput() {
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
  CounterBase* MonitorROBIN::newCounter(const std::string& name, Int_t ID) {
    return new CounterROB(m_costData, name, ID, m_detailLevel, (MonitorBase*) this);
  }
} // namespace TrigCostRootAnalysis
