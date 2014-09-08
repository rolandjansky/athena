// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// -------------------------------------------------------------
//  author: Tim Martin <Tim.Martin@cern.ch>
// -------------------------------------------------------------
#ifndef TrigCostRootAnalysis_CounterL1ChainMap_H
#define TrigCostRootAnalysis_CounterL1ChainMap_H

// STL include(s):
#include <map>
#include <string>

// Local include(s):
#include "CounterBase.h"

// ROOT include(s):
#include <Rtypes.h>
#include <TH1.h>

namespace TrigCostRootAnalysis {

  //Forward declaration
  class TrigCostData;
  
  /**
   * @class CounterL1ChainMap
   * One CounterL1ChainMap is created for each L1 chain in the run. It iterates over all chains which
   * are seeded by this L1 chain and tabulates an estimated total HLT cost based on each L1 menu item.
   * @see CounterBase
   */
  class CounterL1ChainMap : public CounterBase {
  
   public:
   
    typedef std::map< std::string, Float_t >::iterator StringFloatMapIt;
    typedef std::map< std::string, UInt_t >::iterator StringUIntMapIt;
    typedef std::map< std::string, TH1F* >::iterator StringTH1FMapIt;
    
    CounterL1ChainMap( const TrigCostData* _costData, const std::string& _name, Int_t _ID, UInt_t _detailLevel = 10 );
    ~CounterL1ChainMap();
    void startEvent();
    void processEventCounter(UInt_t _e, UInt_t _f, Float_t _weight = 1.);
    void endEvent();
    UInt_t getTotalChainSeeds();
    std::string getChainName(UInt_t _c);
    UInt_t getChainCalls(UInt_t _c);
    TH1F* getChainsSeededHistogram();
    TH1F* getChainsTimeHistogram();
    UInt_t getTotalEvents();
    UInt_t getTotalChainCalls();
    Float_t getTotalChainTime();
    UInt_t getNChains();
    TH1F* getChainTimeHistogram(std::string _name);
    
   private:
   
    UInt_t m_totalEvents; //!< Store the total number of events processed.
    Float_t m_totalTime; //!< Store the integrated time for all chains in all events this L1 chain seeded.
    UInt_t m_totalCalls; //!< Count the total number of HLT chains called by this L1 chain.
    UInt_t m_tempChainCounter;  //!< Internal per-event temporary buffer of number of chains
    Float_t m_tempChainTimeCounter; //!< Internal per-event temporary buffer of chain time.
    TH1F* h_chainsSeededPerEvent; //!< Histogram to hold number of chains seeded per event.
    TH1F* h_chainTimePerEvent; //!< Histogram to hold total time of chains seeded per event.
    
    std::map< std::string, Float_t > m_chainTime; //!< Map of each seeded chain's name to its total time.
    std::map< std::string, UInt_t > m_chainCalls; //!< Map of each seeded chain's name to its total number of calls.
    std::map< std::string, TH1F* > h_chainTime; //!< Map of each seeded chain's name to its individual time histogram.
    
  }; //class CounterL1ChainMap
  
} // namespace TrigCostRootAnalysis

#endif //TrigCostRootAnalysis_CounterL1ChainMap_H
