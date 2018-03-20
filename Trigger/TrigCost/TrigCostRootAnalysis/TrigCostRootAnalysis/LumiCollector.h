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

    void recordEventLumi(Int_t lumiBlock, Float_t length);
    Float_t getLumiBlockTime(Int_t lumiBlock);
    Float_t getTotalLumiBlockTime();
    UInt_t getNLumiBlocks();
    void saveOutput();
    void print();
  private:
    IntFloatMap_t m_lumiLength; //!< Map lumi block number to length in seconds.
    IntIntMap_t m_eventsProcessedPerLB; //!< Map of how many events have been run over per LB
    Float_t m_totalLumiLength; //!< Total length (in s) of all lumi blocks encountered during processing
    Bool_t m_useDefault; //!< Flag  to store default length value if this service is not in use.
    DataStore m_dataStore; //!< Data store to make lumi histogram(s)
  }; //class LumiCollector
} // namespace TrigCostRootAnalysis

#endif //TrigCostRootAnalysis_LumiCollector_H
