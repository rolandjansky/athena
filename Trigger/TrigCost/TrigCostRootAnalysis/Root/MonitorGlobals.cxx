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
#include "../TrigCostRootAnalysis/MonitorGlobals.h"
#include "../TrigCostRootAnalysis/CounterGlobals.h"
#include "../TrigCostRootAnalysis/TrigConfInterface.h"
#include "../TrigCostRootAnalysis/TrigCostData.h"
#include "../TrigCostRootAnalysis/Utility.h"
#include "../TrigCostRootAnalysis/Config.h"
#include "../TrigCostRootAnalysis/TableValueFunctions.h"

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
  
    if ( Config::config().debug() ) Info("MonitorGlobals::newEvent", "*** Processing Global Quantites ***");
    
    // Populate the counter collections we'll be using for this event.
    collateCounterCollectionsForEvent( m_costData->getLumi(), TrigConfInterface::getCurrentDBKey() );
    
    //Now loop over the counter collections;
    for (CounterMapSetIt_t _cmsIt = m_collectionsToProcess.begin(); _cmsIt != m_collectionsToProcess.end(); ++_cmsIt) {
      CounterMap_t* _counterMap = *_cmsIt;
      
      startEvent();

      //Get the counter for this lumiblock
      const std::string _lumiBlockString = std::string( "LumiBlock_") + intToString( m_costData->getLumi() );
      CounterBase* _counter = getCounter( _counterMap, _lumiBlockString, m_costData->getLumi() );
      if (_counter->getCalls() == 0) {
        _counter->decorate(kDecLbLength, m_costData->getLumiLength());
      }
      _counter->processEventCounter( 0, 0, _weight );

      endEvent();

    }
  }
  
  /**
   * Save the results from this monitors counters as specified in the configuration.
   */
  void MonitorGlobals::saveOutput() {
  
    m_filterOutput = kFALSE; // Apply any user-specified name filter to output
    
    // Specify what plots we wish to save from the counters
    VariableOptionVector_t _toSavePlots = m_dummyCounter->getAllHistograms();
    sharedHistogramOutputRoutine( _toSavePlots );

    std::vector<TableColumnFormatter> _toSave;

    _toSave.push_back( TableColumnFormatter("Lumi Block Length (s)", 
      "Length of this luminosity block.",
      kDecLbLength, kSavePerCall, 2, kFormatOptionUseFloatDecoration) ); // kSavePerCall is ignored here

    _toSave.push_back( TableColumnFormatter("HLT PUs", 
      "Number of HLT Processing Units in the HLT farm which were active for this lumi block.",
      kVarHLTPUs, kSavePerCall, 0) );

    _toSave.push_back( TableColumnFormatter("Events Passing L1", 
      "Total number of events where at least one L1 chain is passed after veto.",
      kVarL1PassEvents, kSavePerCall, 0) );

    _toSave.push_back( TableColumnFormatter(std::string("#") + getLevelStr() + std::string(" Events"), 
      "Total number of events seen at this level.",
      kVarHLTEvents, kSavePerCall, 0) );

    _toSave.push_back( TableColumnFormatter(std::string("#") + getLevelStr() + std::string(" Passes"), 
      "Total number of events seen at this which have at least one chain passing raw.",
      kVarHLTPassEvents, kSavePerCall, 0) );

    _toSave.push_back( TableColumnFormatter("Steering Time [s]", 
      "Total difference between the start of the first and the end of the last algorithm execution summed over all events.",
      kVarSteeringTime, kSavePerEvent, 2, kFormatOptionMiliSecToSec) );

    _toSave.push_back( TableColumnFormatter("Steering Time/Event [ms]", 
      "Average time difference between the start of the first and the end of the last algorithm execution per event.",
      kVarSteeringTime, kSavePerEvent, 2, kFormatOptionNormaliseEntries) );

    _toSave.push_back( TableColumnFormatter("Alg Walltime Time/Event [ms]", 
      "Average per event of the sum over all algorithms walltimes.",
      kVarAlgTime, kSavePerEvent, 2, kFormatOptionNormaliseEntries) );

    _toSave.push_back( TableColumnFormatter("Alg Walltime Time/Call [ms]", 
      "Average per algorithm call of the sum over all algorithms walltimes.",
      kVarAlgTime, kSavePerCall, 2, kFormatOptionNormaliseEntries) );

    _toSave.push_back( TableColumnFormatter("ROS Walltime Time/Event [ms]", 
      "Average per event of the sum over all algorithms ROS request times.",
      kVarROSTime, kSavePerEvent, 2, kFormatOptionNormaliseEntries) );

    _toSave.push_back( TableColumnFormatter("Data Requests/Event", 
      "Average per event number of calls made to the Readout System by executed algorithms.",
      kVarROSCalls, kSavePerEvent, 2, kFormatOptionNormaliseEntries) );

    _toSave.push_back( TableColumnFormatter("RoIs/Events", 
      "Average per event number of Regions of Interest supplied from the lower trigger level.",
      kVarROI, kSavePerEvent, 2, kFormatOptionNormaliseEntries) );

    _toSave.push_back( TableColumnFormatter("Farm Usage from Steering (%)", 
      "Approximated by Total HLT Steering Time / Lumi Block Length * N HLT PUs)",
      &tableFnGlobalGetSteeringFarmUse, 2) );

    // TODO - add INPUT rate using L1 info (?)
    // Fin LB start time and LB length
    // Fin HLT farm usage estimates
    // TODO Lumi block scaling corrections
    
    sharedTableOutputRoutine( _toSave );

  }


  /**
   * Construct new counter of correct derived type, pass back as base type.
   * This function must be implemented by all derived monitor types.
   * @see MonitorBase::addCounter( const std::string &_name, Int_t _ID )
   * @param _name Cost reference to name of counter.
   * @param _ID Reference to ID number of counter.
   * @returns Base class pointer to new counter object of correct serived type.
   */
  CounterBase* MonitorGlobals::newCounter(  const std::string &_name, Int_t _ID  ) {
    return new CounterGlobals( m_costData, _name,  _ID, m_detailLevel );
  }
  
  
} // namespace TrigCostRootAnalysis
