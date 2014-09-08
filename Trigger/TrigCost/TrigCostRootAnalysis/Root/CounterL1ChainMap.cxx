// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// -------------------------------------------------------------
//  author: Tim Martin <Tim.Martin@cern.ch>
// -------------------------------------------------------------

// STL include(s):
#include <iostream>

// ROOT include(s):
#include <TChain.h>
#include <TTree.h>
#include <TFile.h>

// Local include(s):
#include "../TrigCostRootAnalysis/CounterL1ChainMap.h"
#include "../TrigCostRootAnalysis/TrigConfInterface.h"
#include "../TrigCostRootAnalysis/TrigCostData.h"
#include "../TrigCostRootAnalysis/Utility.h"

namespace TrigCostRootAnalysis {

  /**
   * L1ChainMap counter constructor. Sets values of internal variables and sets up data store.
   * @param _name Const ref to L1 chain's name
   * @param _ID CTPID number of L1 chain.
   */
  CounterL1ChainMap::CounterL1ChainMap( const TrigCostData* m_costData, const std::string& _name, Int_t _ID, UInt_t _detailLevel ) : CounterBase(m_costData, _name, _ID, _detailLevel), 
    m_totalEvents(0),
    m_totalTime(0),
    m_totalCalls(0),
    m_tempChainCounter(0),
    m_tempChainTimeCounter(0) {
    
    h_chainsSeededPerEvent = new TH1F( "",
                                       std::string( m_name + ";HLT Chains Seeded per Event;Events" ).c_str(),
                                       20, 0.5, 20.5);
                                       
    h_chainTimePerEvent = new TH1F( "",
                                    std::string( m_name + ";HLT Algorithm Time of Seeded Chains [ms];Events" ).c_str(),
                                    TrigCostStyle::xTimeBinsN,
                                    TrigCostStyle::xTimeBins);
  }
  
  /**
   * Counter destructor. Remove internal histograms.
   */
  CounterL1ChainMap::~CounterL1ChainMap() {
    delete h_chainsSeededPerEvent;
    delete h_chainTimePerEvent;
    std::map< std::string, TH1F* >::iterator _it = h_chainTime.begin();
    for (; _it != h_chainTime.end(); ++_it) {
      delete _it->second;
    }
    m_chainTime.clear();
  }
  
  /**
   * Reset per-event counter(s). Not used currently for L1ChainMap counter
   */
  void CounterL1ChainMap::startEvent() {
  }
  
  /**
   * Perform monitoring of a L1 chain. this counter is passed the D3PD location of a HLT chain which was seeded by
   * its L1 chain.
   * @param _e Chain index in D3PD.
   * @param _f Unused by this counter.
   * @param _weight Event weight.
   */
  void CounterL1ChainMap::processEventCounter(UInt_t _e, UInt_t _f, Float_t _weight) {
    ++m_calls;
    UNUSED( _f );
    // I am L1 item m_name. I have just called by chain _e
    // monitor my resource usage for this chain
    
    std::string _chainName = TrigConfInterface::getHLTNameFromChainID( m_costData->getChainID(_e), m_costData->getChainLevel(_e) );
    Float_t _chainTime = m_costData->getChainTimerFromSequences(_e);
    
    // Count totals
    m_totalCalls += _weight;
    m_totalTime += ( _chainTime * _weight);
    
    // Count how many chains are seeded per event and time cost
    m_tempChainCounter += _weight;
    m_tempChainTimeCounter += (_chainTime * _weight);
    
    // Count totals per sub-chain
    m_chainCalls[_chainName] += 1;
    m_chainTime[_chainName] += _chainTime;
    if (h_chainTime.count(_chainName) == 0) {
      h_chainTime.insert( std::pair< std::string, TH1F* >( _chainName, new TH1F( "", "",
                          TrigCostStyle::xTimeBinsN,
                          TrigCostStyle::xTimeBins) ) );
    }
    h_chainTime[_chainName]->Fill(_chainTime, _weight);
    
  }
  
  /**
   * Perform end-of-event monitoring. Reset temp counters
   */
  void CounterL1ChainMap::endEvent() {
    if ( isZero(m_tempChainCounter) == kTRUE ) {
      // No chains were seeded by this L1 item this event
      return;
    }
    m_totalEvents += 1;
    h_chainsSeededPerEvent->Fill( m_tempChainCounter );
    h_chainTimePerEvent->Fill( m_tempChainTimeCounter );
    m_tempChainCounter = 0;
    m_tempChainTimeCounter = 0;
  }
  
  /**
   * Helper function to calculate the total number of chains which were seeded by calculating
   * the sum(bin content * bin value) over the corresponding histogram.
   * @result Total number of HLT chain executions which were seeded by this L1 chain.
   */
  UInt_t CounterL1ChainMap::getTotalChainSeeds() {
    // Get  to get how many chains in total were seeded
    UInt_t _sum = 0;
    for (Int_t i = 1; i <= h_chainsSeededPerEvent->GetNbinsX(); ++i) {
      _sum += h_chainsSeededPerEvent->GetBinContent(i) * h_chainsSeededPerEvent->GetBinCenter(i);
    }
    return _sum;
  }
  
  /**
   * Helper function to retrieve the name of a HLT chain which has been seeded by this L1 item.
   * @param _c Internal chain ID.
   * @result The name of the HLT chain.
   */
  std::string CounterL1ChainMap::getChainName(UInt_t _c) {
    if (_c >= m_chainCalls.size()) return "OUT_OF_RANGE";
    StringUIntMapIt It = m_chainCalls.begin();
    for (UInt_t _i = 0; _i < _c; ++_i) {
      ++It;
    }
    return It->first;
  }
  
  /**
   * Helper function to retrieve the number of calls of a HLT chain which has been seeded by this L1 item.
   * @param _c Internal chain ID.
   * @result The number of calls for this chain.
   */
  UInt_t CounterL1ChainMap::getChainCalls(UInt_t _c) {
    if (_c >= m_chainCalls.size()) return 0;
    StringUIntMapIt It = m_chainCalls.begin();
    for (UInt_t _i = 0; _i < _c; ++_i) {
      ++It;
    }
    return It->second;
  }
  
  /**
   * Return pointer to histogram of number of HLT chains seeded per event from this L1 chain.
   * @return TH1F pointer to histogram.
   */
  TH1F* CounterL1ChainMap::getChainsSeededHistogram() {
    return h_chainsSeededPerEvent;
  }
  
  /**
   * Return pointer to histogram total HLT chain time per event for this L1 chain.
   * @return TH1F pointer to histogram.
   */
  TH1F* CounterL1ChainMap::getChainsTimeHistogram() {
    return h_chainTimePerEvent;
  }
  
  /**
   * Get the total number of events processed.
   * @return The total number of events.
   */
  UInt_t CounterL1ChainMap::getTotalEvents() {
    return m_totalEvents;
  }
  
  /**
   * Get the total number of chains this counter has been called to run over.
   * @return The total number of chains run over.
   */
  UInt_t CounterL1ChainMap::getTotalChainCalls() {
    return m_totalCalls;
  }
  
  /**
   * Get the total time of all chains this counter has been called to run over.
   * @return The total time of all chains run over.
   */
  Float_t CounterL1ChainMap::getTotalChainTime() {
    return m_totalTime;
  }
  
  /**
   * Get the number of unique HLT chains which have been seeded by this L1 chain.
   * @return The number of unique HLT chains seeded.
   */
  UInt_t CounterL1ChainMap::getNChains() {
    return m_chainCalls.size();
  }
  
  /**
   * Return pointer to histogram of time for a single seeded HLT chain, chosen by name.
   * @param _name Name of the HLT chain to fetch the histogram for.
   * @return TH1F pointer to histogram.
   */
  TH1F* CounterL1ChainMap::getChainTimeHistogram(std::string _name) {
    return h_chainTime[_name];
  }
  
} // namespace TrigCostRootAnalysis
