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
#include "../TrigCostRootAnalysis/MonitorAlgorithmClass.h"
#include "../TrigCostRootAnalysis/CounterAlgorithm.h"
#include "../TrigCostRootAnalysis/TrigConfInterface.h"
#include "../TrigCostRootAnalysis/TrigCostData.h"
#include "../TrigCostRootAnalysis/Config.h"
#include "../TrigCostRootAnalysis/Utility.h"

namespace TrigCostRootAnalysis {
  /**
   * Monitor constructor. Sets name and calls base constructor.
   */
  MonitorAlgorithmClass::MonitorAlgorithmClass(const TrigCostData* costData) : MonitorBase(costData,
                                                                                            "Algorithm_Class") {
    allowSameIDCounters();
  }

  /**
   * Process new event for this monitor.
   * For each algorithm class - record details of the execution
   * @param weight The event weight.
   */
  void MonitorAlgorithmClass::newEvent(Float_t weight) {
    m_timer.start();
    if (Config::config().debug()) Info("MonitorAlgorithmClass::newEvent", "*** Processing Algorithms [Class] ***");

    //Now loop over the counter collections;
    for (CounterMapSetIt_t cmsIt = m_collectionsToProcess.begin(); cmsIt != m_collectionsToProcess.end(); ++cmsIt) {
      CounterMap_t* counterMap = *cmsIt;

      startEvent();

      // Loop over all algs in event
      std::vector<AlgsInEvent>::iterator itAlgs = m_algsInEvent.begin();
      for (; itAlgs != m_algsInEvent.end(); ++itAlgs) {
        // Here we want a counter which comprises both the chain name and the alg name
        const std::string className = itAlgs->m_algClassName;
        CounterBase* counter = getCounter(counterMap, className, itAlgs->m_algNameID);
        counter->processEventCounter(itAlgs->m_seqD3PDIndex, itAlgs->m_algD3PDIndex, weight);
      }

      // Do end of event
      endEvent(weight);
    }
    m_timer.stop();
  }

  /**
   * Do we use this monitor for this particular mode? Try and keep things managable in terms of output created!
   * Note these are currently hard-coded. We may want to make them configurable
   * @return If this monitor should be active for a given mode.
   */
  Bool_t MonitorAlgorithmClass::getIfActive(ConfKey_t mode) {
    switch (mode) {
    case kDoAllSummary:       return kTRUE;

    case kDoKeySummary:       return kTRUE;

    case kDoLumiBlockSummary: return kTRUE;

    default: Error("MonitorAlgorithmClass::getIfActive", "An invalid summary mode was provided (key %s)",
                   Config::config().getName(mode).c_str());
    }
    return kFALSE;
  }

  /**
   * Save the results from this monitors counters as specified in the configuration.
   */
  void MonitorAlgorithmClass::saveOutput() {
    m_filterOutput = kTRUE; // Apply any user-specified name filter to output

    VariableOptionVector_t toSavePlots; //Leave blank = each counter gets asked what hists it contains
    sharedHistogramOutputRoutine(toSavePlots);

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
   * @returns Base class pointer to new counter object of correct serived type.
   */
  CounterBase* MonitorAlgorithmClass::newCounter(const std::string& name, Int_t ID) {
    return new CounterAlgorithm(m_costData, name, ID, m_detailLevel, (MonitorBase*) this);
  }
} // namespace TrigCostRootAnalysis
