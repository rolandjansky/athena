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
#include <sstream>
#include <iomanip>

// Local include(s):
#include "../TrigCostRootAnalysis/MonitorEventProfile.h"
#include "../TrigCostRootAnalysis/CounterEventProfile.h"
#include "../TrigCostRootAnalysis/TrigConfInterface.h"
#include "../TrigCostRootAnalysis/TrigCostData.h"
#include "../TrigCostRootAnalysis/Utility.h"
#include "../TrigCostRootAnalysis/Config.h"
#include "../TrigCostRootAnalysis/TableValueFunctions.h"

namespace TrigCostRootAnalysis {
  /**
   * Monitor constructor. Sets name and calls base constructor.
   */
  MonitorEventProfile::MonitorEventProfile(const TrigCostData* costData) : MonitorBase(costData, "EventProfile") {
    m_dummyCounter = newCounter(Config::config().getStr(kDummyString), INT_MIN);
    allowSameIDCounters();

    m_AlgToEventProfile[""] = "AllEvents";
    m_AlgToEventProfile["TrigMuSuperEF"] = "Muon";
    m_AlgToEventProfile["TrigCaloClusterMaker_topo_tcemFS"] = "JetFullScan";
    m_AlgToEventProfile["TrigCaloClusterMaker_topo_tclcwFS"] = "JetFullScan";
    m_AlgToEventProfile["TrigFastTrackFinder_BeamSpot_IDTrig"] = "IDFullScan";
  }

  /**
   * The global monitor records overviews. Each counter corresponds to a whole lumi block.
   * It is sensible to only enable this for the "All" or "Per HLT keyset" ranges.
   * But can be used with "Per lumi block" as well, it will just only have one entry.
   * @param weight The event weight.
   */
  void MonitorEventProfile::newEvent(Float_t weight) {
    m_timer.start();
    if (Config::config().debug()) Info("MonitorEventProfile::newEvent", "*** Processing Global Quantites ***");

    // Get categories
    std::set< std::string > eventClassifications;

    for (UInt_t s = 0; s < m_costData->getNSequences(); ++s) {
      for (UInt_t a = 0; a < m_costData->getNSeqAlgs(s); ++a) {
        Int_t seqIndex = m_costData->getSequenceIndex(s);
        Int_t seqAlgPos = m_costData->getSeqAlgPosition(s, a);
        const std::string algName = TrigConfInterface::getHLTAlgNameFromSeqIDAndAlgPos(seqIndex, seqAlgPos);
        if (m_AlgToEventProfile.count(algName) == 1) {
          eventClassifications.insert(m_AlgToEventProfile[algName]);
        }
      }
    }

    eventClassifications.insert("AllEvents"); // TEsting

    //Now loop over the counter collections;
    for (CounterMapSetIt_t cmsIt = m_collectionsToProcess.begin(); cmsIt != m_collectionsToProcess.end(); ++cmsIt) {
      CounterMap_t* counterMap = *cmsIt;

      startEvent();

      // Loop over each classification
      for (std::set<std::string>::iterator ecIt = eventClassifications.begin(); ecIt != eventClassifications.end();
           ++ecIt) {
        Int_t algN = 0;
        Float_t timeOffset = 0;

        for (UInt_t s = 0; s < m_costData->getNSequences(); ++s) {
          for (UInt_t a = 0; a < m_costData->getNSeqAlgs(s); ++a) {
            Float_t start = m_costData->getSeqAlgTimeStartSec(s, a);
            start += m_costData->getSeqAlgTimeStartMicroSec(s, a) / 1e6;

            if (Config::config().debug()) {
              Info("MonitorEventProfile::newEvent", "(%i,%i) - %f cache:%i time:%f",
                   s, a, start, m_costData->getSeqAlgIsCached(s, a), m_costData->getSeqAlgTimer(s, a));
            }

            if (isZero(start) == kTRUE) continue;                                                        // Ignore for
                                                                                                          // now algs

            if (isZero(timeOffset) == kTRUE) {
              timeOffset = start;
              Config::config().setFloat(kEventTimeOffset, timeOffset, "TimeOffset", kUnlocked);
            }

            std::ostringstream ss;
            ss << std::setfill('0') << std::setw(5) << algN;
            std::string counterStr = (*ecIt) + "_Alg" + ss.str();

            CounterBase* counter = getCounter(counterMap, counterStr, algN);
            if (counter->getCalls() == 0) { // Save the event classification as a decoration so we can pick it up easy
                                             // when doing the output
              counter->decorate(kDecCounterClassification, (*ecIt));
            }

            counter->processEventCounter(s, a, weight);
            ++algN;
          }
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
  Bool_t MonitorEventProfile::getIfActive(ConfKey_t mode) {
    switch (mode) {
    case kDoAllSummary:       return kTRUE;

    case kDoKeySummary:       return kTRUE;

    case kDoLumiBlockSummary: return kFALSE;

    default: Error("MonitorEventProfile::getIfActive", "An invalid summary mode was provided (key %s)",
                   Config::config().getName(mode).c_str());
    }
    return kFALSE;
  }

  /**
   * Save the results from this monitors counters as specified in the configuration.
   */
  void MonitorEventProfile::saveOutput() {
    std::vector<TableColumnFormatter> toSave;

    toSave.push_back(TableColumnFormatter("Events",
                                           "Number of events which made it this deep into alg execution",
                                           kVarCalls, kSavePerCall, 0));

    toSave.push_back(TableColumnFormatter("Av. Time Elapsed [s]",
                                           "Average CPU time spent at this step",
                                           kVarTimeElapsed, kSavePerCall, 6, kFormatOptionNormaliseEntries));

    toSave.push_back(TableColumnFormatter("Av. CPU Time [ms]",
                                           "Average CPU time spent at this step",
                                           kVarCPUTime, kSavePerCall, 6, kFormatOptionNormaliseEntries));

    toSave.push_back(TableColumnFormatter("Av. ROS Time [ms]",
                                           "Average ROS time spent at this step",
                                           kVarROSTime, kSavePerCall, 6, kFormatOptionNormaliseEntries));

    //toSave.push_back( TableColumnFormatter("Alg Cache Fraction",
    //  "Fraction of algorithm executions which were cached",
    //  kVarAlgCaches, kSavePerCall, 4, kFormatOptionNormaliseEntries) );

    toSave.push_back(TableColumnFormatter("Av. ROS Calls",
                                           "Average number of calls to the ROS both cached and retrieved",
                                           kVarROSCalls, kSavePerCall, 6, kFormatOptionNormaliseEntries));

    toSave.push_back(TableColumnFormatter("Av. ROS Retrieve [kB]",
                                           "Average size of ROB fragment retrieved from the ROS",
                                           kVarROBRetSize, kSavePerCall, 6, kFormatOptionNormaliseEntries));

    toSave.push_back(TableColumnFormatter("Av. ROS Cached [kB]",
                                           "Average size of cached ROB fragment requested from the ROS",
                                           kVarROBReqSize, kSavePerCall, 6, kFormatOptionNormaliseEntries));


    // Loop over each classification and output a monitor for each
    m_filterOutput = kTRUE;
    std::set< std::string > eventClassificationsOutput;
    for (StringStringMapIt_t it = m_AlgToEventProfile.begin(); it != m_AlgToEventProfile.end(); ++it) {
      if (eventClassificationsOutput.count(it->second) == 1) continue; // Prevent duplicates
      eventClassificationsOutput.insert(it->second);
      setName(std::string("EventProfile_" + it->second));
      filterOutputOnStrDecoration(kDecCounterClassification, it->second);
      sharedTableOutputRoutine(toSave);
    }
  }

  /**
   * Construct new counter of correct derived type, pass back as base type.
   * This function must be implemented by all derived monitor types.
   * @see MonitorBase::addCounter( const std::string &name, Int_t ID )
   * @param name Cost reference to name of counter.
   * @param ID Reference to ID number of counter.
   * @returns Base class pointer to new counter object of correct serived type.
   */
  CounterBase* MonitorEventProfile::newCounter(const std::string& name, Int_t ID) {
    return new CounterEventProfile(m_costData, name, ID, m_detailLevel, (MonitorBase*) this);
  }
} // namespace TrigCostRootAnalysis
