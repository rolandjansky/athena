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
#include "../TrigCostRootAnalysis/MonitorGlobals.h"
#include "../TrigCostRootAnalysis/CounterGlobals.h"
#include "../TrigCostRootAnalysis/TrigConfInterface.h"
#include "../TrigCostRootAnalysis/TrigCostData.h"
#include "../TrigCostRootAnalysis/Utility.h"
#include "../TrigCostRootAnalysis/Config.h"
#include "../TrigCostRootAnalysis/TableValueFunctions.h"
#include "../TrigCostRootAnalysis/TrigXMLService.h"

namespace TrigCostRootAnalysis {
  /**
   * Monitor constructor. Sets name and calls base constructor.
   */
  MonitorGlobals::MonitorGlobals(const TrigCostData* _costData) : MonitorBase(_costData, "Global") {
    m_dummyCounter = newCounter(Config::config().getStr(kDummyString), INT_MIN);
  }

  /**
   * The global monitor records overviews. Each counter corresponds to a whole lumi block.
   * It is sensible to only enable this for the "All" or "Per HLT keyset" ranges.
   * But can be used with "Per lumi block" as well, it will just only have one entry.
   * @param _weight The event weight.
   */
  void MonitorGlobals::newEvent(Float_t _weight) {
    m_timer.start();
    if (Config::config().debug()) Info("MonitorGlobals::newEvent", "*** Processing Global Quantites ***");


    //Now loop over the counter collections;
    for (CounterMapSetIt_t _cmsIt = m_collectionsToProcess.begin(); _cmsIt != m_collectionsToProcess.end(); ++_cmsIt) {
      CounterMap_t* _counterMap = *_cmsIt;

      startEvent();

      //Get the counter for this lumiblock
      std::stringstream _ss;
      _ss << std::setfill('0') << std::setw(5) << m_costData->getLumi();
      const std::string _lumiBlockString = std::string("LumiBlock_") + _ss.str();
      CounterBase* _counter = getCounter(_counterMap, _lumiBlockString, m_costData->getLumi());
      _counter->processEventCounter(0, 0, _weight);

      // Also process the All LB counter, we give this the ID -1
      // Don't want to do this if we're iterating over a single-LB collection
      if (m_counterMapType[_counterMap] != kDoLumiBlockSummary) {
        getCounter(_counterMap, Config::config().getStr(kAllString), -1)->processEventCounter(0, 0, _weight);
      }

      endEvent(_weight);
    }
    m_timer.stop();
  }

  /**
   * Do we use this monitor for this particular mode? Try and keep things manageable in terms of output created!
   * Note these are currently hard-coded. We may want to make them configurable
   * @return If this monitor should be active for a given mode.
   */
  Bool_t MonitorGlobals::getIfActive(ConfKey_t _mode) {
    switch (_mode) {
    case kDoAllSummary:       return kTRUE;

    case kDoKeySummary:       return kTRUE;

    case kDoLumiBlockSummary: return kTRUE;

    default: Error("MonitorGlobals::getIfActive", "An invalid summary mode was provided (key %s)",
                   Config::config().getName(_mode).c_str());
    }
    return kFALSE;
  }

  /**
   * Save the results from this monitors counters as specified in the configuration.
   */
  void MonitorGlobals::saveOutput() {
    // Now we're done, we want to additionally decorate these counters with their effective LB length, given how many
    // events we saw vs. how many were were expecting to see
    // Loop over all my counter collections
    // TODO make a finalise loop and put me in it
    for (CounterCollectionIt_t _ccIt = m_counterCollections.begin(); _ccIt != m_counterCollections.end(); ++_ccIt) {
      std::string _counterCollectionName = (*_ccIt).first; // This collection name
      CounterMap_t _counterMap = (*_ccIt).second; // This collection's counter map
      LumiCollector* _collectionLumiCollector = m_collectionLumiCollector[ _counterCollectionName ]; // This
                                                                                                     // collection's
                                                                                                     // lumi counter
      // Loop over my counters
      for (CounterMapIt_t _cmIt = _counterMap.begin(); _cmIt != _counterMap.end(); ++_cmIt) {
        // Get the effective lumi for this LB. The ID of the counter is the LB number
        CounterBase* _counter = (*_cmIt).second;
        Int_t _lbNumber = _counter->getID();
        Float_t _lbTime = 0.;
        if (_lbNumber == -1) {
          // I'm the 'ALL' counter
          _lbTime = _collectionLumiCollector->getTotalLumiBlockTime();
        } else {
          // I'm a normal counter
          _lbTime = _collectionLumiCollector->getLumiBlockTime(_lbNumber);
        }
        _counter->decorate(kDecLbLength, _lbTime);
      }
    }

    m_filterOutput = kFALSE; // Apply any user-specified name filter to output

    // Specify what plots we wish to save from the counters
    VariableOptionVector_t _toSavePlots = m_dummyCounter->getAllHistograms();
    sharedHistogramOutputRoutine(_toSavePlots);

    std::vector<TableColumnFormatter> _toSave;

    const Bool_t _isAPrediction = (Bool_t) Config::config().getInt(kIsCPUPrediction);

    if (_isAPrediction == kTRUE) {
      _toSave.push_back(TableColumnFormatter("Effective LB Length (s)",
                                             "Effective length of the lumi block after correcting for how many events we have seen.",
                                             kDecLbLength, kSavePerCall, 2, kFormatOptionUseFloatDecoration)); // kSavePerCall
                                                                                                               // is
                                                                                                               // ignored
                                                                                                               // here
    } else {
      _toSave.push_back(TableColumnFormatter("Lumi Block Length (s)",
                                             "Length of this luminosity block.",
                                             kDecLbLength, kSavePerCall, 2, kFormatOptionUseFloatDecoration)); // kSavePerCall
                                                                                                               // is
                                                                                                               // ignored
                                                                                                               // here

      _toSave.push_back(TableColumnFormatter("HLT PUs",
                                             "Number of HLT Processing Units in the HLT farm which were active for this lumi block.",
                                             kVarHLTPUs, kSavePerCall, 0));
    }

    if (_isAPrediction == kTRUE) {
      _toSave.push_back(TableColumnFormatter("Predicted HLT Cores From Algs",
                                             "Approximated by Total HLT Algorithm Time / Effective Lumi Block Length",
                                             &tableFnGlobalGetHLTNodePrediction, 2));

      _toSave.push_back(TableColumnFormatter("Predicted Cores (Algs) Err",
                                             "sqrt(sumW2 AlgTime) / Effective Lumi Block Length",
                                             &tableFnGlobalGetHLTNodePredictionErr, 2));

      _toSave.push_back(TableColumnFormatter("Predicted HLT Cores From Steering",
                                             "Approximated by Total HLT Algorithm Time / Effective Lumi Block Length",
                                             &tableFnGlobalGetHLTNodePredictionSteering, 2));

      _toSave.push_back(TableColumnFormatter("Predicted Cores (Steer) Err",
                                             "sqrt(sumW2 AlgTime) / Effective Lumi Block Length",
                                             &tableFnGlobalGetHLTNodePredictionErrSteering, 2));
    } else {
      _toSave.push_back(TableColumnFormatter("Farm Usage from Steering (%)",
                                             "Approximated by Total HLT Steering Time / (Lumi Block Length * N HLT PUs)",
                                             &tableFnGlobalGetSteeringFarmUse, 2));
    }

    _toSave.push_back(TableColumnFormatter("Raw Events",
                                           "Raw nummber of events for which we have collected statistics for before weighting",
                                           kVarEventsActive, kSavePerCall, 0, kFormatOptionUseEntries));

    if (_isAPrediction == kFALSE) {
      _toSave.push_back(TableColumnFormatter("Events Passing L1",
                                             "Total number of events where at least one L1 chain is passed after veto.",
                                             kVarL1PassEvents, kSavePerCall, 0));

      _toSave.push_back(TableColumnFormatter("Input Rate from L1",
                                             "Total number of events where at least one L1 chain is passed after veto.",
                                             kVarL1PassEvents, kSavePerCall, 2, kFormatOptionNormaliseLBTimeDec));
    }

    if (_isAPrediction == kTRUE) {
      // If prediction - then the number of 'pass' events is compilicated and needs the RatesMonitor.
      // Here we can give the effective total number of events however
      _toSave.push_back(TableColumnFormatter(std::string("# ") + getLevelStr() + std::string(" Events"),
                                             "Total number of events seen at this level.",
                                             kVarHLTEvents, kSavePerCall, 0));
    } else {
      // If not weighting, then just count events where a chain passes
      _toSave.push_back(TableColumnFormatter(std::string("# ") + getLevelStr() + std::string(" Passes"),
                                             "Total number of events seen at this which have at least one chain passing raw.",
                                             kVarHLTPassEvents, kSavePerCall, 0));

      _toSave.push_back(TableColumnFormatter(getLevelStr() + std::string(" Pass Fraction [%]"),
                                             "What percentage of L1 events are kept",
                                             kVarHLTPassEvents, kSavePerCall, kVarL1PassEvents, kSavePerCall, 2,
                                             kFormatOptionToPercentage));

      // These two are of no help if we're throwing weights and prescales all over the place
      _toSave.push_back(TableColumnFormatter("Steering Time [s]",
                                             "Total difference between the start of the first and the end of the last algorithm execution summed over all events.",
                                             kVarSteeringTime, kSavePerEvent, 2, kFormatOptionMiliSecToSec));

      _toSave.push_back(TableColumnFormatter("Steering Time/Event [ms]",
                                             "Average time difference between the start of the first and the end of the last algorithm execution per event.",
                                             kVarSteeringTime, kSavePerEvent, kVarEventsActive, kSavePerCall, 2));
    }

    _toSave.push_back(TableColumnFormatter("Alg Walltime Time [s]",
                                           "The sum over all algorithms walltimes.",
                                           kVarAlgTime, kSavePerCall, 2, kFormatOptionMiliSecToSec));

    _toSave.push_back(TableColumnFormatter("Alg Walltime Time/Event [ms]",
                                           "Average per event of the sum over all algorithms walltimes.",
                                           kVarAlgTime, kSavePerEvent, kVarEventsActive, kSavePerCall, 2));

    _toSave.push_back(TableColumnFormatter("Alg Walltime Time/Call [ms]",
                                           "Average per algorithm call of the sum over all algorithms walltimes.",
                                           kVarAlgTime, kSavePerCall, kVarAlgCalls, kSavePerCall, 2));

    _toSave.push_back(TableColumnFormatter("Time Use In Rerun [%]",
                                           "Percentage of this total CPU usage which comes from resurrection.",
                                           kVarRerunTime, kSavePerEvent, kVarAlgTime, kSavePerEvent, 2,
                                           kFormatOptionToPercentage));

    _toSave.push_back(TableColumnFormatter("Time Use In Accepted Events [%]",
                                           "Percentage of this total CPU usage which comes from resurrection.",
                                           kVarPassTime, kSavePerEvent, kVarAlgTime, kSavePerEvent, 2,
                                           kFormatOptionToPercentage));

    _toSave.push_back(TableColumnFormatter("ROS Walltime Time/Event [ms]",
                                           "Average per event of the sum over all algorithms ROS request times.",
                                           kVarROSTime, kSavePerEvent, kVarEventsActive, kSavePerCall, 2));

    _toSave.push_back(TableColumnFormatter("Data Requests/Event",
                                           "Average per event number of calls made to the Readout System by executed algorithms.",
                                           kVarROSCalls, kSavePerEvent, kVarEventsActive, kSavePerCall, 2));

    _toSave.push_back(TableColumnFormatter("RoIs/Event",
                                           "Average per event number of Regions of Interest supplied from the lower trigger level.",
                                           kVarROI, kSavePerEvent, kVarEventsActive, kSavePerCall, 2));

    _toSave.push_back(TableColumnFormatter("CostMon Time/Event [ms]",
                                           "Average time per event to execute cost monitoring.",
                                           kVarTrigCostTime, kSavePerEvent, kVarEventsActive, kSavePerCall, 2));


    // _toSave.push_back( TableColumnFormatter("Texec Time/Event [ms]",
    //   "Average time per event for the Texec timer.",
    //   kVarTexecTime, kSavePerEvent, kVarEventsActive, kSavePerCall, 2) );

    // _toSave.push_back( TableColumnFormatter("Chain Exec Time/Event [ms]",
    //   "Average per event to process all chains.",
    //   kVarChainExecTime, kSavePerEvent, kVarEventsActive, kSavePerCall, 2) );

    // _toSave.push_back( TableColumnFormatter("Result Builder Time/Event [ms]",
    //   "Average per event number to run the Result Builder.",
    //   kVarResultBuildingTime, kSavePerEvent, kVarEventsActive, kSavePerCall, 2) );

    // _toSave.push_back( TableColumnFormatter("Monitoring Time/Event [ms]",
    //   "Average per event to run all monitoring tools. Excluding CostMon/",
    //   kVarMonitoringTime, kSavePerEvent, kVarEventsActive, kSavePerCall, 2) );


    // TODO - add INPUT rate using L1 info (?)
    // Fin LB start time and LB length
    // Fin HLT farm usage estimates
    // TODO Lumi block scaling corrections

    sharedTableOutputRoutine(_toSave);
  }

  /**
   * Construct new counter of correct derived type, pass back as base type.
   * This function must be implemented by all derived monitor types.
   * @see MonitorBase::addCounter( const std::string &_name, Int_t _ID )
   * @param _name Cost reference to name of counter.
   * @param _ID Reference to ID number of counter.
   * @returns Base class pointer to new counter object of correct serived type.
   */
  CounterBase* MonitorGlobals::newCounter(const std::string& _name, Int_t _ID) {
    return new CounterGlobals(m_costData, _name, _ID, m_detailLevel, (MonitorBase*) this);
  }
} // namespace TrigCostRootAnalysis
