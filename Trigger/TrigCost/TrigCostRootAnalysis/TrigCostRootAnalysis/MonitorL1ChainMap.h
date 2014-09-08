// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// -------------------------------------------------------------
//  author: Tim Martin <Tim.Martin@cern.ch>
// -------------------------------------------------------------
#ifndef TrigCostRootAnalysis_MonitorL1ChainMap_H
#define TrigCostRootAnalysis_MonitorL1ChainMap_H

// STL include(s):
#include <map>
#include <string>
#include <vector>
#include <string>

// Local include(s):
#include "MonitorBase.h"

namespace TrigCostRootAnalysis {

  //Forward declaration
  class TrigCostData;
  
  /**
   * @class MonitorL1ChainMap
   * Look at chains seeding from a L1 item - assess change in costing with L1 prescale
   */
  class MonitorL1ChainMap : public MonitorBase {
  
   public:
   
    MonitorL1ChainMap(const TrigCostData* _costData);
    void newEvent(Float_t _weight = 1.);
    CounterBase* newCounter( const std::string &_name, Int_t _ID );
    void saveOutput();
    
    
  }; //class MonitorL1ChainMap
  
} // namespace TrigCostRootAnalysis

#endif //TrigCostRootAnalysis_MonitorL1ChainMap_H
