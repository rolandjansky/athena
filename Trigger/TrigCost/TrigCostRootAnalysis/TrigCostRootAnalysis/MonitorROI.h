// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// -------------------------------------------------------------
//  author: Tim Martin <Tim.Martin@cern.ch>
// -------------------------------------------------------------
#ifndef TrigCostRootAnalysis_MonitorROI_H
#define TrigCostRootAnalysis_MonitorROI_H

// STL include(s):
#include <map>
#include <string>
#include <vector>

// Local include(s):
#include "MonitorBase.h"

// ROOT include(s):
#include <TH1.h>
#include <TCanvas.h>

namespace TrigCostRootAnalysis {

  
  /**
   * @class MonitorROI
   * Collect statistics on the number and type of ROI
   */
  class MonitorROI : public MonitorBase {
  
   public:
   
    MonitorROI(const TrigCostData* _costData);
    void newEvent(Float_t _weight = 1.);
    CounterBase* newCounter( const std::string &_name, Int_t _ID );
    void saveOutput();
    
  }; //class MonitorROI
  
} // namespace TrigCostRootAnalysis

#endif //TrigCostRootAnalysis_MonitorROI_H
