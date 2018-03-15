// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// -------------------------------------------------------------
//  author: Tim Martin <Tim.Martin@cern.ch>
// -------------------------------------------------------------
#ifndef TrigCostRootAnalysis_MonitorEventProfile_H
#define TrigCostRootAnalysis_MonitorEventProfile_H

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
   * @class MonitorEventProfile
   * Profile different classes of events executed at HLT
   */
  class MonitorEventProfile: public MonitorBase {
  public:
    MonitorEventProfile(const TrigCostData* costData);
    void newEvent(Float_t weight = 1.);
    CounterBase* newCounter(const std::string& name, Int_t ID);
    Bool_t getIfActive(ConfKey_t mode);
    void saveOutput();
  private:
    StringStringMap_t m_AlgToEventProfile; //!< Which algorithm names should be linked to monitored event types.
                                           // Currently only configured at design time.
  }; //class MonitorEventProfile
} // namespace TrigCostRootAnalysis

#endif //TrigCostRootAnalysis_MonitorEventProfile_H
