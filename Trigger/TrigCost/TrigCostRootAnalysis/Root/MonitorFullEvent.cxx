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

//Local include(s):
#include "../TrigCostRootAnalysis/MonitorFullEvent.h"
#include "../TrigCostRootAnalysis/CounterAlgorithm.h"
#include "../TrigCostRootAnalysis/TrigConfInterface.h"
#include "../TrigCostRootAnalysis/TrigCostData.h"
#include "../TrigCostRootAnalysis/Utility.h"
#include "../TrigCostRootAnalysis/Config.h"

namespace TrigCostRootAnalysis {

  /**
   * Monitor constructor. Sets name and calls base constructor.
   */
  MonitorFullEvent::MonitorFullEvent(const TrigCostData* _costData) : MonitorBase(_costData, "Full_Evnt"), 
    m_probability(0.),
    m_rando(4) { // Seed chosen by fair dice role. 
    m_dummyCounter = newCounter(Config::config().getStr(kDummyString), INT_MIN);
    //allowSameNamedCounters(); //Allow 'degenerate' counter names - get a different counter for each alg execution.
    setDetailLevel(0); // Don't save any histograms for all counters spawned from here

    m_probability = 1. / (Float_t) Config::config().getInt(kFullEventSaveOnePer);
  }
  
  /**
   * Process new event for this monitor.
   * The full event execution will be summarised.
   * @param _weight The event weight.
   */
  void MonitorFullEvent::newEvent(Float_t _weight) {

    // Check if we are randomly going to accept this event
    if ( m_rando.Rndm() > m_probability ) return;
    
    // Check if we have already saved our max number of events.
    // In this monitor, we use a new counter collection for every event.
    if ( m_counterCollections.size() >= (UInt_t) Config::config().getInt(kFullEventMaxNumToSave) ) {
      if (Config::config().debug()) {
        Warning("MonitorFullEvent::newEvent", "Reached maximum number of Full-Events to save (%i). Skipping.", (Int_t) m_counterCollections.size());
      }
      return;
    }

    // For this monitor - we have a CounterCollection per event. 
    // The event number is used as the key. Pad with 6 0's
    const std::string _eventNumberStr = std::string("Event_") + intToString( m_costData->getEventNumber(), 6);
    CounterMap_t* _eventMap = getCounterCollection( _eventNumberStr );

    if (true || Config::config().debug()) {
      Info("MonitorFullEvent::newEvent","Saving full event for event number %s", _eventNumberStr.c_str());
    }

    startEvent();

    // Loop over all sequences.
    for (UInt_t _s = 0; _s < m_costData->getNSequences(); ++_s) {
    
      // Get the name of the chain this sequence instance is in (Supplying L2 or EF helps, but is not needed)
      Int_t _chainID = m_costData->getSequenceChannelCounter(_s);
      const std::string _chainName = TrigConfInterface::getHLTNameFromChainID( _chainID, m_costData->getSequenceLevel(_s) );
      
      // Loop over all algorithms in sequence
      for (UInt_t _a = 0; _a < m_costData->getNSeqAlgs(_s); ++_a) {
      
        Int_t _seqIndex = m_costData->getSequenceIndex(_s);
        const std::string _seqName = TrigConfInterface::getHLTSeqNameFromIndex( _seqIndex );
        Int_t _seqAlgPos = m_costData->getSeqAlgPosition(_s, _a);
        
        const std::string _algName = TrigConfInterface::getHLTAlgNameFromSeqIDAndAlgPos( _seqIndex, _seqAlgPos );
        const std::string _algType = TrigConfInterface::getHLTAlgClassNameFromSeqIDAndAlgPos( _seqIndex, _seqAlgPos );

        Int_t _seqAlgNameHash = TrigConfInterface::getHLTAlgClassNameIDFromSeqIDAndAlgPos( _seqIndex, _seqAlgPos );
        
        // Count how many we have already saved
        UInt_t _nCountersOfThisAlg = 0;
        for (CounterMapIt_t _it = _eventMap->begin(); _it != _eventMap->end(); ++_it) {
          if ( _it->first.find( _algName ) != std::string::npos ) ++_nCountersOfThisAlg;
        }
        
        // Change the name such that we always get a new counter
        const std::string _algNewName = _algName + std::string("[") + intToString(_nCountersOfThisAlg) + std::string("]");       
        CounterBase* _counter = getCounter( _eventMap, _algNewName, _seqAlgNameHash );
        _counter->decorate( kDecAlgClassName, _algType );
        _counter->decorate( kDecChainName, _chainName );
        _counter->decorate( kDecSeqName, _seqName);
        _counter->processEventCounter( _s, _a, _weight ); 
      }
    }

    endEvent();
  }
  
  /**
   * Construct new counter of correct derived type, pass back as base type.
   * This function must be implemented by all derived monitor types.
   * @see MonitorBase::addCounter( const std::string &_name, Int_t _ID )
   * @param _name Cost reference to name of counter.
   * @param _ID Reference to ID number of counter.
   * @returns Base class pointer to new counter object of correct serived type.
   */
  CounterBase* MonitorFullEvent::newCounter( const std::string &_name, Int_t _ID  ) {
    // Note - we use Algorithm counter objects in this monitor
    return new CounterAlgorithm( m_costData, _name,  _ID, m_detailLevel );
  }
  
  /**
   * Save the results from this monitors counters as specified in the configuration.
   */
  void MonitorFullEvent::saveOutput() {

    // No histogram output for MonitorFullEvent

    // Define output table
    std::vector<TableColumnFormatter> _toSave;
    _toSave.push_back( TableColumnFormatter("Start Time", 
      "Timestamp of the begining of this algorithms execution.",
      kDecStartTime, kSavePerCall, 4, kFormatOptionUseFloatDecoration ) );

    _toSave.push_back( TableColumnFormatter("Chain Name", 
      "Chain which requested this algorithm call.",
      kDecChainName, kSavePerCall, 0, kFormatOptionUseStringDecoration) );

    _toSave.push_back( TableColumnFormatter("Sequence Step", 
      "Sequence step of this algorothm.",
      kDecSeqName, kSavePerCall, 0, kFormatOptionUseStringDecoration) );

    _toSave.push_back( TableColumnFormatter("Algorithm Class", 
      "Class name of this algorithm instance.",
      kDecAlgClassName, kSavePerCall, 0, kFormatOptionUseStringDecoration) );

    _toSave.push_back( TableColumnFormatter("RoI ID(s)", 
      "Regions of interest to be processed by this algorithm.",
      kDecROIString, kSavePerCall, 0, kFormatOptionUseStringDecoration) );    

    _toSave.push_back( TableColumnFormatter("Algorithm Status", 
      "If the result of this algorithm call was cached.",
      kDecCallOrCache, kSavePerCall, 0, kFormatOptionUseStringDecoration) );  

    _toSave.push_back( TableColumnFormatter("Algorithm Total Time (ms)", 
      "Total (CPU + ROS) time for this algorithm execution",
      kVarTime, kSavePerCall, 2) );    

    _toSave.push_back( TableColumnFormatter("# Data Requests", 
      "Number of calls to the ROS made by this algorithm execution",
      kVarROSCalls, kSavePerCall, 0) ); 

    _toSave.push_back( TableColumnFormatter("ROB Time (ms)", 
      "Time spent retrieving data from ROBs for this algorithm execution.",
      kVarROSTime, kSavePerCall, 2) );   

    _toSave.push_back( TableColumnFormatter("# Cached ROBs", 
      "Number of cached ROBs fetched by this algorithm execution.",
      kVarROBReqs, kSavePerCall, 0) );   

    _toSave.push_back( TableColumnFormatter("# Retrieved ROBs", 
      "Number of ROBs retrieved by this algorithm execution",
      kVarROBRets, kSavePerCall, 0) );   

    _toSave.push_back( TableColumnFormatter("Cached ROBs Size (kB)", 
      "Size of cached ROBs fetched by this algorithm execution.",
      kVarROBReqSize, kSavePerCall, 2) );   

    _toSave.push_back( TableColumnFormatter("Retrieved ROBs Size (kB)", 
      "Size of ROBs fetched by this algorithm execution.",
      kVarROBRetSize, kSavePerCall, 2) );  

    _toSave.push_back( TableColumnFormatter("Data Requests", 
      "List of ROS calls by this algorithm and their multiplicity.",
      kDecROSString, kSavePerCall, 0, kFormatOptionUseStringDecoration) );   
    
    sharedTableOutputRoutine( _toSave );

  }
  
} // namespace TrigCostRootAnalysis

// Store some old code here - could come in handy again
  // void CounterFullEvent::debug(UInt_t _e) {
  //   UNUSED( _e );
  //   // Loop over TEs
  //   std::ofstream fout(std::string("te_test"+getName()+".dot").c_str());
  //   fout << "digraph G{" << std::endl;
  //   for (UInt_t i = 0; i < m_costData->getNTEs(); ++i) {
  //     fout << "\t" << m_costData->getTEIndex(i) << "[label=\"<f0>" <<
      
  //          (m_costData->getIsTERegularTe(i) ? "R|" : "" ) <<
  //          (m_costData->getIsTEInitial(i) ? " I|" : "" ) <<
  //          (m_costData->getIsTERoITe(i) ? "RoI|" : "" ) <<
  //          (m_costData->getIsTEL1Threshold(i) ? "L1|" : "") <<
           
  //          "<f1> " <<
           
  //          (m_costData->getIsTEActiveState(i) ? "A|" : "" ) <<
  //          (m_costData->getIsTEErrorState(i) ? "E|" : "" ) <<
  //          (m_costData->getIsTEOutputEFNode(i) ? "OEF|" : "" ) <<
  //          (m_costData->getIsTEOutputL2Node(i) ? "OL2|" : "" ) <<
  //          (m_costData->getIsTETerminalNode(i) ? "TRM|" : "" ) <<
  //          (m_costData->getIsTETopologicalTe(i) ? "TOPO|" : "" ) <<
           
  //          "<f2> ";
           
           
  //     for (UInt_t c = 0; c < m_costData->getTERoIIDSize(i); ++c) {
  //       Int_t r = m_costData->getRoIIndexFromId( m_costData->getTERoIIDIndex(i, c) );
  //       if (r != -1 && r != 255) {
  //         fout << "RoIThr:" << m_costData->getRoINL1Thresh(r) << "-";
  //         if (m_costData->getIsRoIEmTau(r)) fout << "EMTAU";
  //         if (m_costData->getIsRoIEnergy(r)) fout << "E";
  //         if (m_costData->getIsRoIJet(r))  fout << "JET";
  //         if (m_costData->getIsRoIJetEt(r))  fout << "JETET";
  //         if (m_costData->getIsRoIMuon(r))  fout << "MU";
  //         if (m_costData->getIsRoINone(r))  fout << "NONE";
  //         fout << " (" << m_costData->getRoIEta(r) << "," << m_costData->getRoIPhi(r) << ")|";
  //       }
  //     }
  //     fout << "\"";
  //     fout << std::endl << "\tshape = \"record\"];" << std::endl;
      
  //     std::cout << "TE Index:" << m_costData->getTEIndex(i) << " ID:" << m_costData->getTEID(i) << " Children:{";
  //     for (UInt_t c = 0; c < m_costData->getTEChildSize(i); ++c) {
  //       std::cout << m_costData->getTEChildIndex(i, c) << ",";
  //       fout << "\t" << m_costData->getTEIndex(i)  << ":f0 -> " << m_costData->getTEChildIndex(i, c) << ";" << std::endl;
  //     }
  //     std::cout << "} Parents:{";
  //     for (UInt_t c = 0; c < m_costData->getTEParentSize(i); ++c) {
  //       std::cout << m_costData->getTEParentIndex(i, c) << ",";
  //     }
  //     std::cout << "} CLID:{";
  //     for (UInt_t c = 0; c < m_costData->getTECLIDSize(i); ++c) {
  //       std::cout << m_costData->getTECLIDIndex(i, c) << ",";
  //     }
  //     std::cout << "} RoIID:{";
  //     for (UInt_t c = 0; c < m_costData->getTERoIIDSize(i); ++c) {
  //       std::cout << m_costData->getTERoIIDIndex(i, c) << "[";
  //       Int_t r = m_costData->getRoIIndexFromId( m_costData->getTERoIIDIndex(i, c) );
  //       if (r != -1) {
  //         std::cout << "NL1Thresh:" << m_costData->getRoINL1Thresh(r) << "|";
  //         if (m_costData->getIsRoIEmTau(r))  std::cout << "EMTAU|";
  //         if (m_costData->getIsRoIEnergy(r))  std::cout << "ENERGY|";
  //         if (m_costData->getIsRoIJet(r))  std::cout << "JET|";
  //         if (m_costData->getIsRoIJetEt(r))  std::cout << "JETET|";
  //         if (m_costData->getIsRoIMuon(r))  std::cout << "MU|";
  //         if (m_costData->getIsRoINone(r))  std::cout << "NONE|";
  //         std::cout << m_costData->getRoIEta(r) << "|" << m_costData->getRoIPhi(r);
  //       }
  //       std::cout << "],";
  //     }
  //     std::cout << "}";
  //     if (m_costData->getIsTERegularTe(i)) std::cout << " <isRegularTE>";
  //     if (m_costData->getIsTEInitial(i)) std::cout << " <isInitialTe>";
  //     if (m_costData->getIsTERoITe(i)) std::cout << " <isRoITE>";
  //     if (m_costData->getIsTEL1Threshold(i)) std::cout << " <isL1Thresh>";
  //     if (m_costData->getIsTEActiveState(i)) std::cout << " isActiveState";
  //     if (m_costData->getIsTEErrorState(i)) std::cout << " isErrorState";
  //     if (m_costData->getIsTEOutputEFNode(i)) std::cout << " isOutputEFNode";
  //     if (m_costData->getIsTEOutputL2Node(i)) std::cout << " isOutputL2Node";
  //     if (m_costData->getIsTETerminalNode(i)) std::cout << " isTerminalNode";
  //     if (m_costData->getIsTETopologicalTe(i)) std::cout << " isTopoTE";
  //     std::cout << std::endl;
  //   }
  //   fout << "}";
  //   fout.close();
  // }