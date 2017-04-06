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
#include "../TrigCostRootAnalysis/MonitorAlgorithm.h"
#include "../TrigCostRootAnalysis/CounterAlgorithm.h"
#include "../TrigCostRootAnalysis/TrigConfInterface.h"
#include "../TrigCostRootAnalysis/TrigCostData.h"
#include "../TrigCostRootAnalysis/Config.h"
#include "../TrigCostRootAnalysis/Utility.h"

namespace TrigCostRootAnalysis {
  /**
   * Monitor constructor. Sets name and calls base constructor.
   */
  MonitorAlgorithm::MonitorAlgorithm(const TrigCostData* _costData) : MonitorBase(_costData, "Algorithm") {
    m_dummyCounter = newCounter(Config::config().getStr(kDummyString), INT_MIN);
  }

  /**
   * Process new event for this monitor.
   * For algorithms, all sequences are looped over, all individual algorithms in the sequences are looped over.
   * Each algorithm call has low-level information recorded.
   * @param _weight The event weight.
   */
  void MonitorAlgorithm::newEvent(Float_t _weight) {
    m_timer.start();
    if (Config::config().debug()) Info("MonitorAlgorithm::newEvent", "*** Processing Algorithms ***");

    //Now loop over the counter collections;
    for (CounterMapSetIt_t _cmsIt = m_collectionsToProcess.begin(); _cmsIt != m_collectionsToProcess.end(); ++_cmsIt) {
      CounterMap_t* _counterMap = *_cmsIt;

      startEvent();

      std::vector<AlgsInEvent>::iterator _itAlgs = m_algsInEvent.begin();
      for (; _itAlgs != m_algsInEvent.end(); ++_itAlgs) {
        CounterBase* _counter = getCounter(_counterMap, _itAlgs->m_algName, _itAlgs->m_algNameID);
        // If this counter is new, we can set it's secondary name which in this case is its class
        if (_counter->getCalls() == 0) {
          _counter->decorate(kDecAlgClassName, _itAlgs->m_algClassName);
        }
        _counter->processEventCounter(_itAlgs->m_seqD3PDIndex, _itAlgs->m_algD3PDIndex, _weight);
      }

      // Do end of event
      endEvent(_weight);
    }
    m_timer.stop();
  }

  /**
   * Do we use this monitor for this particular mode? Try and keep things managable in terms of output created!
   * Note these are currently hard-coded. We may want to make them configurable
   * @return If this monitor should be active for a given mode.
   */
  Bool_t MonitorAlgorithm::getIfActive(ConfKey_t _mode) {
    switch (_mode) {
    case kDoAllSummary:       return kTRUE;

    case kDoKeySummary:       return kTRUE;

    case kDoLumiBlockSummary: return kTRUE;

    default: Error("MonitorAlgorithm::getIfActive", "An invalid summary mode was provided (key %s)",
                   Config::config().getName(_mode).c_str());
    }
    return kFALSE;
  }

  /**
   * Save the results from this monitors counters as specified in the configuration.
   */
  void MonitorAlgorithm::saveOutput() {
    m_filterOutput = kTRUE; // Apply any user-specified name filter to output

    VariableOptionVector_t _toSavePlots; //Leave blank = each counter gets asked what hists it contains
    sharedHistogramOutputRoutine(_toSavePlots);

    std::vector<TableColumnFormatter> _toSaveTable;
    addCommonTableEntries(_toSaveTable);
    sharedTableOutputRoutine(_toSaveTable);
  }

  /**
   * Construct new counter of correct derived type, pass back as base type.
   * This function must be implemented by all derived monitor types.
   * @see MonitorBase::addCounter( const std::string &_name, Int_t _ID )
   * @param _name Cost reference to name of counter.
   * @param _ID Reference to ID number of counter.
   * @returns Base class pointer to new counter object of correct serived type.
   */
  CounterBase* MonitorAlgorithm::newCounter(const std::string& _name, Int_t _ID) {
    return new CounterAlgorithm(m_costData, _name, _ID, m_detailLevel, (MonitorBase*) this);
  }
} // namespace TrigCostRootAnalysis
