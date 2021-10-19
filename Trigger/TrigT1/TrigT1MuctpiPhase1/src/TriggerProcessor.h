/*                                                                                                                      
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

// This file is really -*- C++ -*-.
#ifndef TRIGT1MUCTPIPHASE1_TRIGGERPROCESSOR_H
#define TRIGT1MUCTPIPHASE1_TRIGGERPROCESSOR_H

#include "TrigThresholdDecisionTool.h"
#include "MUCTPIResults.h"

#include "TrigT1Interfaces/Lvl1MuCTPIInputPhase1.h"

#include <vector>
#include <list>
#include <map>
#include <string>
#include <utility>

namespace TrigConf {
  class L1Menu;
}

namespace LVL1MUCTPIPHASE1 {
  class Configuration;

  class TriggerProcessor
  {
    
  public:
    
    TriggerProcessor() = default;

    //initialization
    void setMenu(const TrigConf::L1Menu* l1menu);
    void setTrigTool(LVL1::TrigThresholdDecisionTool& tool) {m_trigThresholdDecisionTool=&tool;}

    //processing
    LVL1MUONIF::Lvl1MuCTPIInputPhase1 mergeInputs(std::vector<const LVL1MUONIF::Lvl1MuCTPIInputPhase1*> inputs) const;
    std::string computeMultiplicities(LVL1MUONIF::Lvl1MuCTPIInputPhase1& mergedInputs, int bcid, MUCTPIResults& results) const;

  private:

    const TrigConf::L1Menu* m_l1menu{nullptr};
    LVL1::TrigThresholdDecisionTool* m_trigThresholdDecisionTool{nullptr};

  };
}

#endif // TRIGT1MUCTPIPHASE1_TRIGGERPROCESSOR_H
