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
  MonitorEventProfile::MonitorEventProfile(const TrigCostData* _costData) : MonitorBase(_costData, "EventProfile") {
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
   * @param _weight The event weight.
   */
  void MonitorEventProfile::newEvent(Float_t _weight) {
    m_timer.start();
    if (Config::config().debug()) Info("MonitorEventProfile::newEvent", "*** Processing Global Quantites ***");

    // Get categories
    std::set< std::string > _eventClassifications;

    for (UInt_t _s = 0; _s < m_costData->getNSequences(); ++_s) {
      for (UInt_t _a = 0; _a < m_costData->getNSeqAlgs(_s); ++_a) {
        Int_t _seqIndex = m_costData->getSequenceIndex(_s);
        Int_t _seqAlgPos = m_costData->getSeqAlgPosition(_s, _a);
        const std::string _algName = TrigConfInterface::getHLTAlgNameFromSeqIDAndAlgPos(_seqIndex, _seqAlgPos);
        if (m_AlgToEventProfile.count(_algName) == 1) {
          _eventClassifications.insert(m_AlgToEventProfile[_algName]);
        }
      }
    }

    _eventClassifications.insert("AllEvents"); // TEsting

    //Now loop over the counter collections;
    for (CounterMapSetIt_t _cmsIt = m_collectionsToProcess.begin(); _cmsIt != m_collectionsToProcess.end(); ++_cmsIt) {
      CounterMap_t* _counterMap = *_cmsIt;

      startEvent();

      // Loop over each classification
      for (std::set<std::string>::iterator _ecIt = _eventClassifications.begin(); _ecIt != _eventClassifications.end();
           ++_ecIt) {
        Int_t _algN = 0;
        Float_t _timeOffset = 0;

        for (UInt_t _s = 0; _s < m_costData->getNSequences(); ++_s) {
          for (UInt_t _a = 0; _a < m_costData->getNSeqAlgs(_s); ++_a) {
            Float_t _start = m_costData->getSeqAlgTimeStartSec(_s, _a);
            _start += m_costData->getSeqAlgTimeStartMicroSec(_s, _a) / 1e6;

            if (Config::config().debug()) {
              Info("MonitorEventProfile::newEvent", "(%i,%i) - %f cache:%i time:%f",
                   _s, _a, _start, m_costData->getSeqAlgIsCached(_s, _a), m_costData->getSeqAlgTimer(_s, _a));
            }

            if (isZero(_start) == kTRUE) continue;                                                        // Ignore for
                                                                                                          // now algs

            if (isZero(_timeOffset) == kTRUE) {
              _timeOffset = _start;
              Config::config().setFloat(kEventTimeOffset, _timeOffset, "TimeOffset", kUnlocked);
            }

            std::ostringstream _ss;
            _ss << std::setfill('0') << std::setw(5) << _algN;
            std::string _counterStr = (*_ecIt) + "_Alg" + _ss.str();

            CounterBase* _counter = getCounter(_counterMap, _counterStr, _algN);
            if (_counter->getCalls() == 0) { // Save the event classification as a decoration so we can pick it up easy
                                             // when doing the output
              _counter->decorate(kDecCounterClassification, (*_ecIt));
            }

            _counter->processEventCounter(_s, _a, _weight);
            ++_algN;
          }
        }
      }
      endEvent(_weight);
    }
    m_timer.stop();
  }

  /**
   * Do we use this monitor for this particular mode? Try and keep things managable in terms of output created!
   * Note these are currently hard-coded. We may want to make them configurable
   * @return If this monitor should be active for a given mode.
   */
  Bool_t MonitorEventProfile::getIfActive(ConfKey_t _mode) {
    switch (_mode) {
    case kDoAllSummary:       return kTRUE;

    case kDoKeySummary:       return kTRUE;

    case kDoLumiBlockSummary: return kFALSE;

    default: Error("MonitorEventProfile::getIfActive", "An invalid summary mode was provided (key %s)",
                   Config::config().getName(_mode).c_str());
    }
    return kFALSE;
  }

  /**
   * Save the results from this monitors counters as specified in the configuration.
   */
  void MonitorEventProfile::saveOutput() {
    std::vector<TableColumnFormatter> _toSave;

    _toSave.push_back(TableColumnFormatter("Events",
                                           "Number of events which made it this deep into alg execution",
                                           kVarCalls, kSavePerCall, 0));

    _toSave.push_back(TableColumnFormatter("Av. Time Elapsed [s]",
                                           "Average CPU time spent at this step",
                                           kVarTimeElapsed, kSavePerCall, 6, kFormatOptionNormaliseEntries));

    _toSave.push_back(TableColumnFormatter("Av. CPU Time [ms]",
                                           "Average CPU time spent at this step",
                                           kVarCPUTime, kSavePerCall, 6, kFormatOptionNormaliseEntries));

    _toSave.push_back(TableColumnFormatter("Av. ROS Time [ms]",
                                           "Average ROS time spent at this step",
                                           kVarROSTime, kSavePerCall, 6, kFormatOptionNormaliseEntries));

    //_toSave.push_back( TableColumnFormatter("Alg Cache Fraction",
    //  "Fraction of algorithm executions which were cached",
    //  kVarAlgCaches, kSavePerCall, 4, kFormatOptionNormaliseEntries) );

    _toSave.push_back(TableColumnFormatter("Av. ROS Calls",
                                           "Average number of calls to the ROS both cached and retrieved",
                                           kVarROSCalls, kSavePerCall, 6, kFormatOptionNormaliseEntries));

    _toSave.push_back(TableColumnFormatter("Av. ROS Retrieve [kB]",
                                           "Average size of ROB fragment retrieved from the ROS",
                                           kVarROBRetSize, kSavePerCall, 6, kFormatOptionNormaliseEntries));

    _toSave.push_back(TableColumnFormatter("Av. ROS Cached [kB]",
                                           "Average size of cached ROB fragment requested from the ROS",
                                           kVarROBReqSize, kSavePerCall, 6, kFormatOptionNormaliseEntries));


    // Loop over each classification and output a monitor for each
    m_filterOutput = kTRUE;
    std::set< std::string > _eventClassificationsOutput;
    for (StringStringMapIt_t _it = m_AlgToEventProfile.begin(); _it != m_AlgToEventProfile.end(); ++_it) {
      if (_eventClassificationsOutput.count(_it->second) == 1) continue; // Prevent duplicates
      _eventClassificationsOutput.insert(_it->second);
      setName(std::string("EventProfile_" + _it->second));
      filterOutputOnStrDecoration(kDecCounterClassification, _it->second);
      sharedTableOutputRoutine(_toSave);
    }
  }

  /**
   * Construct new counter of correct derived type, pass back as base type.
   * This function must be implemented by all derived monitor types.
   * @see MonitorBase::addCounter( const std::string &_name, Int_t _ID )
   * @param _name Cost reference to name of counter.
   * @param _ID Reference to ID number of counter.
   * @returns Base class pointer to new counter object of correct serived type.
   */
  CounterBase* MonitorEventProfile::newCounter(const std::string& _name, Int_t _ID) {
    return new CounterEventProfile(m_costData, _name, _ID, m_detailLevel, (MonitorBase*) this);
  }
} // namespace TrigCostRootAnalysis
