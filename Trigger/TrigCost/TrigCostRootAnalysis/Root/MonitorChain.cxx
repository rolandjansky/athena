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
#include "../TrigCostRootAnalysis/MonitorChain.h"
#include "../TrigCostRootAnalysis/CounterChain.h"
#include "../TrigCostRootAnalysis/TrigConfInterface.h"
#include "../TrigCostRootAnalysis/TrigCostData.h"
#include "../TrigCostRootAnalysis/Config.h"

namespace TrigCostRootAnalysis {
  /**
   * Monitor constructor. Sets name and calls base constructor.
   * Chain monitor looks at the level of activated HLT chains and monitors how many algorithms are executed and the time
   */
  MonitorChain::MonitorChain(const TrigCostData* costData) : MonitorBase(costData, "Chain") {
    // Dummy counter still knows of the histograms defined for counters of this type
    m_dummyCounter = newCounter(Config::config().getStr(kDummyString), INT_MIN);
  }

  /**
   * Process new event for this monitor.
   * For the chain monitor, all chains present in this event are processed and recorded.
   * @param weight The event weight.
   */
  void MonitorChain::newEvent(Float_t weight) {
    m_timer.start();
    if (Config::config().debug()) Info("MonitorChain::newEvent", "*** Processing Chains ***");

    //Now loop over the counter collections;

    const Bool_t hltPass = (Bool_t) Config::config().getInt(kHLTPass);

    for (CounterMapSetIt_t cmsIt = m_collectionsToProcess.begin(); cmsIt != m_collectionsToProcess.end(); ++cmsIt) {
      CounterMap_t* counterMap = *cmsIt;

      startEvent();

      // Loop over all chains.
      for (UInt_t c = 0; c < m_costData->getNChains(); ++c) {
        if ((UInt_t) m_costData->getChainLevel(c) != getLevel()) {
          continue;
        }

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

        if (m_costData->getIsChainResurrected(c)) { // Am I scheduled for 2nd pass?
          if (!hltPass) continue; // If so then the event needs to pass
        } else if (m_costData->getIsChainPrescaled(c)) {
          continue; // Don't include prescaled out chains from the 1st pass
        }

        CounterBase* counter = getCounter(counterMap, chainName, chainID);
        if (counter->getCalls() ==
            0) counter->decorate(kDecGroupName, TrigConfInterface::getHLTGroupNameFromChainID(chainID, 0)); // i'm new
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
  Bool_t MonitorChain::getIfActive(ConfKey_t mode) {
    switch (mode) {
    case kDoAllSummary:       return kTRUE;

    case kDoKeySummary:       return kTRUE;

    case kDoLumiBlockSummary: return kTRUE;

    default: Error("MonitorChain::getIfActive", "An invalid summary mode was provided (key %s)",
                   Config::config().getName(mode).c_str());
    }
    return kFALSE;
  }

  /**
   * Save the results from this monitors counters as specified in the configuration.
   */
  void MonitorChain::saveOutput() {
    m_filterOutput = kTRUE; // Apply any user-specified name filter to output

    // Specify what plots we wish to save from the counters
    VariableOptionVector_t toSavePlots = m_dummyCounter->getAllHistograms();
    sharedHistogramOutputRoutine(toSavePlots);

    std::vector<TableColumnFormatter> toSaveTable;
    const std::string slowText = "Calls > " + intToString(Config::config().getInt(kSlowThreshold)) + " ms";

    toSaveTable.push_back(TableColumnFormatter("Group",
                                                "Bandwidth group this chain is associated to.",
                                                kDecGroupName, kSavePerCall, 0, kFormatOptionUseStringDecoration));

    toSaveTable.push_back(TableColumnFormatter("Raw Active Events",
                                                "Raw (unweighted) statistics on the number of events in which this chain was executed.",
                                                kVarEventsActive, kSavePerEvent, 0, kFormatOptionUseEntries));

    toSaveTable.push_back(TableColumnFormatter("Active Events",
                                                "Number of events in which this chain was executed.",
                                                kVarEventsActive, kSavePerEvent, 0));

    toSaveTable.push_back(TableColumnFormatter("Time Per Event [ms]",
                                                "Average execution time per event of this chain.",
                                                kVarTime, kSavePerEvent, kVarEventsActive, kSavePerEvent, 2));

    // toSaveTable.push_back( TableColumnFormatter("Effective PS Weight",
    //   "Weight applied to all events to simulate prescale.",
    //   kEffectivePrescale, kSavePerEvent, 4, kFormatOptionUseFloatDecoration ) );

    toSaveTable.push_back(TableColumnFormatter("Execute Rate [Hz]",
                                                "Number of chain executions normalised to the wall time for this run range.",
                                                kVarEventsActive, kSavePerEvent, 4, kFormatOptionNormaliseWallTime));

    toSaveTable.push_back(TableColumnFormatter(getLevelStr() + std::string(" Pass Fraction [%]"),
                                                "What percentage of events pass events are kept",
                                                kVarEventsPassed, kSavePerEvent, kVarEventsActive, kSavePerEvent, 6,
                                                kFormatOptionToPercentage));

    toSaveTable.push_back(TableColumnFormatter(slowText,
                                                "Number of algorithm executions which were particularly slow",
                                                kVarEventsSlow, kSavePerEvent, 0));

    toSaveTable.push_back(TableColumnFormatter("Total Chain Time [s]",
                                                "Total time used by this chain for this run range.",
                                                kVarTime, kSavePerEvent, 2, kFormatOptionMiliSecToSec));

    toSaveTable.push_back(TableColumnFormatter("Total Chain Time Frac. Err. [%]",
                                                "Fractional statistical uncertainty on total chain time.",
                                                &tableFnChainGetTotalTimeErr, 3));

    toSaveTable.push_back(TableColumnFormatter("Total Chain Time [%]",
                                                "Total chain time as a percentage of the total time of all chains in this run range.",
                                                &tableFnChainGetTotalFracTime, 3));

    toSaveTable.push_back(TableColumnFormatter("Time Use In Rerun [%]",
                                                "Percentage of this chains CPU usage which comes from resurrection.",
                                                kVarRerunTime, kSavePerEvent, kVarTime, kSavePerEvent, 3,
                                                kFormatOptionToPercentage));

    toSaveTable.push_back(TableColumnFormatter("Run Agls/Event",
                                                "Total number of algorithms executed by this chain.",
                                                kVarAlgCalls, kSavePerEvent, kVarEventsActive, kSavePerEvent, 2));

    toSaveTable.push_back(TableColumnFormatter("Cached Algs/Event",
                                                "Total number of algorithms which supplied a cached result to this chain.",
                                                kVarAlgCaches, kSavePerEvent, kVarEventsActive, kSavePerEvent, 2));

    toSaveTable.push_back(TableColumnFormatter("Cached ROB Rate [Hz]",
                                                "Number of cached ROBs requested by this chain normalised to the run range.",
                                                kVarROBReqs, kSavePerEvent, 2, kFormatOptionNormaliseWallTime));

    toSaveTable.push_back(TableColumnFormatter("Cached ROB Rate [kB/s]",
                                                "Size of cached ROBs requested by this chain normalised to the run range.",
                                                kVarROBReqSize, kSavePerEvent, 2, kFormatOptionNormaliseWallTime));

    toSaveTable.push_back(TableColumnFormatter("Retrieved ROB Rate [Hz]",
                                                "Number of ROBs retrieved by this chain normalised to the run range.",
                                                kVarROBRets, kSavePerEvent, 2, kFormatOptionNormaliseWallTime));

    toSaveTable.push_back(TableColumnFormatter("Retrieved ROB Rate [kB/s]",
                                                "Size of the ROBs retrieved by this chain normalised to the run range.",
                                                kVarROBRetSize, kSavePerEvent, 2, kFormatOptionNormaliseWallTime));

    sharedTableOutputRoutine(toSaveTable);
  }

  /**
   * Construct new counter of correct derived type, pass back as base type.
   * This function must be implemented by all derived monitor types.
   * @see MonitorBase::addCounter( const std::string &name, Int_t ID )
   * @param name Cost reference to name of counter.
   * @param ID Reference to ID number of counter.
   * @returns Base class pointer to new counter object of correct serived type.
   */
  CounterBase* MonitorChain::newCounter(const std::string& name, Int_t ID) {
    return new CounterChain(m_costData, name, ID, m_detailLevel, (MonitorBase*) this);
  }
} // namespace TrigCostRootAnalysis
