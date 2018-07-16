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
#include <vector>

// Local include(s):
#include "../TrigCostRootAnalysis/MonitorROI.h"
#include "../TrigCostRootAnalysis/CounterROI.h"
#include "../TrigCostRootAnalysis/TrigConfInterface.h"
#include "../TrigCostRootAnalysis/TrigCostData.h"
#include "../TrigCostRootAnalysis/Config.h"

namespace TrigCostRootAnalysis {
  /**
   * Monitor constructor. Sets name and calls base constructor.
   */
  MonitorROI::MonitorROI(const TrigCostData* costData) : MonitorBase(costData, "ROI") {
    m_dummyCounter = newCounter(Config::config().getStr(kDummyString), INT_MIN);
  }

  /**
   * Process new event for this monitor.
   * We have one counter per ROI type and a global one
   * @param weight The event weight.
   */
  void MonitorROI::newEvent(Float_t weight) {
    m_timer.start();
    if (Config::config().debug()) Info("MonitorROI::newEvent", "*** Processing ROI ***");

    //Now loop over the counter collections;

    for (CounterMapSetIt_t cmsIt = m_collectionsToProcess.begin(); cmsIt != m_collectionsToProcess.end(); ++cmsIt) {
      CounterMap_t* counterMap = *cmsIt;

      startEvent();

      // Loop over all chains.
      for (UInt_t r = 0; r < m_costData->getNRoIs(); ++r) {
        CounterBase* counter = getCounter(counterMap, m_costData->getRoITypeString(r), 0);

        if (counter != 0) {
          counter->processEventCounter(r, 0, weight);
        }

        // Call another counter on all RoIs to get the global picture
        counter = getCounter(counterMap, Config::config().getStr(kAllROIString), 6);
        counter->processEventCounter(r, 0, weight);
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
  Bool_t MonitorROI::getIfActive(ConfKey_t mode) {
    switch (mode) {
    case kDoAllSummary:       return kTRUE;

    case kDoKeySummary:       return kTRUE;

    case kDoLumiBlockSummary: return kTRUE;

    default: Error("MonitorROI::getIfActive", "An invalid summary mode was provided (key %s)",
                   Config::config().getName(mode).c_str());
    }
    return kFALSE;
  }

  /**
   * Save the results from this monitors counters as specified in the configuration.
   */
  void MonitorROI::saveOutput() {
    m_filterOutput = kFALSE; // Apply any user-specified name filter to output

    // Specify what plots we wish to save from the counters
    VariableOptionVector_t toSavePlots = m_dummyCounter->getAllHistograms();
    sharedHistogramOutputRoutine(toSavePlots);

    std::vector<TableColumnFormatter> toSaveTable;
    toSaveTable.push_back(TableColumnFormatter("ROIs",
                                                "Total number of ROIs of this type.",
                                                kVarCalls, kSavePerEvent, 0));

    toSaveTable.push_back(TableColumnFormatter("ROIs/Event",
                                                "Average number of ROIs of this type per event.",
                                                kVarCalls, kSavePerEvent, kVarEventsActive, kSavePerEvent, 2));

    toSaveTable.push_back(TableColumnFormatter("L1Thresholds/ROI",
                                                "Average number of L1 Thresholds per ROI of this type.",
                                                kVarL1Thresh, kSavePerCall, kVarCalls, kSavePerEvent, 2));

    sharedTableOutputRoutine(toSaveTable);
  }

  /**
   * Construct new counter of correct derived type, pass back as base type.
   * This function must be implemented by all derived monitor types.
   * @see MonitorBase::addCounter( const std::string &_name, Int_t ID )
   * @param name Cost reference to name of counter.
   * @param ID Reference to ID number of counter.
   * @returns Base class pointer to new counter object of correct serived type.
   */
  CounterBase* MonitorROI::newCounter(const std::string& name, Int_t ID) {
    return new CounterROI(m_costData, name, ID, m_detailLevel, (MonitorBase*) this);
  }
} // namespace TrigCostRootAnalysis
