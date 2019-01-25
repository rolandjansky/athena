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

//Local include(s):
#include "../TrigCostRootAnalysis/MonitorSliceCPU.h"
#include "../TrigCostRootAnalysis/CounterSliceCPU.h"
#include "../TrigCostRootAnalysis/TrigConfInterface.h"
#include "../TrigCostRootAnalysis/TrigCostData.h"
#include "../TrigCostRootAnalysis/Config.h"
#include "../TrigCostRootAnalysis/Utility.h"

namespace TrigCostRootAnalysis {
  /**
   * Monitor constructor. Sets name and calls base constructor.
   */
  MonitorSliceCPU::MonitorSliceCPU(const TrigCostData* costData) : MonitorBase(costData, "SliceCPU") {
    m_dummyCounter = newCounter(Config::config().getStr(kDummyString), INT_MIN);
    allowSameIDCounters();
  }

  /**
   * Process new event for this monitor.
   * For each alg, get its chain and the chain's first group. Use this as the counter type
   * @param weight The event weight.
   */
  void MonitorSliceCPU::newEvent(Float_t weight) {
    m_timer.start();
    if (Config::config().debug()) Info("MonitorSliceCPU::newEvent", "*** Processing Algorithms ***");

    for (CounterMapSetIt_t cmsIt = m_collectionsToProcess.begin(); cmsIt != m_collectionsToProcess.end(); ++cmsIt) {
      CounterMap_t* counterMap = *cmsIt;

      startEvent();

      // Loop over all chains.
      for (UInt_t c = 0; c < m_costData->getNChains(); ++c) {
        // Get the name of the chain (Supplying L2 or EF helps, but is not needed)
        Int_t chainID = m_costData->getChainID(c);
        const std::string chainName =
          TrigConfInterface::getHLTNameFromChainID(chainID, m_costData->getChainLevel(c));

        // Did we fail?
        if (chainName == Config::config().getStr(kBlankString)) {
          Warning("MonitorChain::newEvent", "Skipping Chain ID %i. Cannot get name from current configuration.",
                  chainID);
          continue;
        }

        // Are we running over this chain?
        if (checkPatternNameMonitor(chainName, m_invertFilter,
                                    m_costData->getIsChainResurrected(c)) == kFALSE) continue;

        std::string chainGroup;
        if (TrigConfInterface::getNHLTGroupNamesFromChainID(chainID) > 0) {
          chainGroup = TrigConfInterface::getHLTGroupNameFromChainID(chainID, 0);
        } else {
          chainGroup = Config::config().getStr(kUnknownString);
          if (Config::config().getDisplayMsg(kMsgNoGroup) == kTRUE) Warning("MonitorSliceCPU::newEvent",
                                                                            "Chain %s (%i) has no group",
                                                                            chainName.c_str(), chainID);
        }

        CounterBase* counter = getCounter(counterMap, chainGroup, 0);
        counter->processEventCounter(c, 0, weight);
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
  Bool_t MonitorSliceCPU::getIfActive(ConfKey_t mode) {
    switch (mode) {
    case kDoAllSummary:       return kTRUE;

    case kDoKeySummary:       return kTRUE;

    case kDoLumiBlockSummary: return kTRUE;

    default: Error("MonitorSliceCPU::getIfActive", "An invalid summary mode was provided (key %s)",
                   Config::config().getName(mode).c_str());
    }
    return kFALSE;
  }

  /**
   * Save the results from this monitors counters as specified in the configuration.
   */
  void MonitorSliceCPU::saveOutput() {
    m_filterOutput = kTRUE; // Apply any user-specified name filter to output

    VariableOptionVector_t toSavePlots = m_dummyCounter->getAllHistograms();
    std::vector<TableColumnFormatter> toSaveTable;

    toSaveTable.push_back(MonitorBase::TableColumnFormatter("Event Rate [Hz]",
                                                             "Rate in this run range of events with at least one execution of this algorithm.",
                                                             kVarEventsActive, kSavePerEvent, 2,
                                                             kFormatOptionNormaliseWallTime));

    toSaveTable.push_back(MonitorBase::TableColumnFormatter("Group Total Time [s]",
                                                             "Total time for a chain execution in this group.",
                                                             kVarTime, kSavePerCall, 2, kFormatOptionMiliSecToSec));

    toSaveTable.push_back(MonitorBase::TableColumnFormatter("Group Total Time Frac. Err. [%]",
                                                             "Fractional statistical uncertainty on total chain time.",
                                                             &tableFnChainGetTotalTimeErr, 3));

    toSaveTable.push_back(MonitorBase::TableColumnFormatter("Group Total Time [%]",
                                                             "Total time for chain execution in this group as a percentage of all algorithm executions in this run range.",
                                                             &tableFnChainGetTotalFracTime, 2)); // We can re-use the
                                                                                                 // Chain fn here as
                                                                                                 // vars in both called
                                                                                                 // "time"

    toSaveTable.push_back(MonitorBase::TableColumnFormatter("Group Total Time/Event [ms]",
                                                             "Average execution time for chains in this group per event for events with at least one executed chain in this run range.",
                                                             kVarTime, kSavePerEvent, kVarEventsActive, kSavePerEvent,
                                                             2));

    sharedTableOutputRoutine(toSaveTable);
    sharedHistogramOutputRoutine(toSavePlots);
  }

  /**
   * Construct new counter of correct derived type, pass back as base type.
   * This function must be implemented by all derived monitor types.
   * @see MonitorBase::addCounter( const std::string &_name, Int_t ID )
   * @param name Cost reference to name of counter.
   * @param ID Reference to ID number of counter.
   * @returns Base class pointer to new counter object of correct serived type.
   */
  CounterBase* MonitorSliceCPU::newCounter(const std::string& name, Int_t ID) {
    return new CounterSliceCPU(m_costData, name, ID, m_detailLevel, (MonitorBase*) this);
  }
} // namespace TrigCostRootAnalysis
