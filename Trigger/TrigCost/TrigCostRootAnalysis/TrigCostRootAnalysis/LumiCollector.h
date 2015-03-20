// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// -------------------------------------------------------------
//  author: Tim Martin <Tim.Martin@cern.ch>
// -------------------------------------------------------------
#ifndef TrigCostRootAnalysis_LumiCollector_H
#define TrigCostRootAnalysis_LumiCollector_H

// STL include(s):
#include <map>

// Local include(s):
#include "DataStore.h"

// ROOT include(s):
#include <Rtypes.h>

namespace TrigCostRootAnalysis {

  /**
   * @class LumiCollector
   * Service class which tabulates the length of lumi blocks that are processed during execution.
   */
  class LumiCollector {
  
   public:

    LumiCollector(); 
    ~LumiCollector();

    void    recordEventLumi(Int_t _lumiBlock, Float_t _length);
    Float_t getLumiBlockTime(Int_t _lumiBlock);
    Float_t getTotalLumiBlockTime();
    UInt_t  getNLumiBlocks(); 
    void    saveOutput();
    void    print();
    
   private:

    void tryXMLLoad();
   
    IntFloatMap_t m_lumiLength; //!< Map lumi block number to length in seconds.
    IntIntMap_t   m_eventsProcessedPerLB; //!< Map of how many events have been run over per LB
    IntIntMap_t   m_totalEventsPerLB; //!< Map of how many events there were online per LB (from COOL, stored in XML)
    Float_t       m_totalLumiLength; //!< Total length (in s) of all lumi blocks encountered during processing
    Bool_t        m_useDefault; //!< Flag  to store default length value if this service is not in use.
    DataStore     m_dataStore; //!< Data store to make lumi histogram(s)

    Bool_t        m_triedXMLLoad; //!< Tried to load number of events per LB from XML

  }; //class LumiCollector
  
} // namespace TrigCostRootAnalysis

#endif //TrigCostRootAnalysis_LumiCollector_H
