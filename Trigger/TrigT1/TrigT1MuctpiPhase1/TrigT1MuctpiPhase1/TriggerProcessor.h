/*                                                                                                                      
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration                                               
*/

// This file is really -*- C++ -*-.
#ifndef TRIGT1MUCTPIPHASE1_TRIGGERPROCESSOR_H
#define TRIGT1MUCTPIPHASE1_TRIGGERPROCESSOR_H

#include <vector>
#include <list>

namespace TrigConf {
  class TriggerThreshold;
}

namespace LVL1MUONIF {
  class Lvl1MuCTPIInputPhase1;
}

namespace LVL1MUCTPIPHASE1 {
  class Configuration;
  class TriggerProcessor
  {
    
  public:
    
    TriggerProcessor();
    ~TriggerProcessor();

    void setThresholds(const std::vector<TrigConf::TriggerThreshold*>& thresholds);
    void mergeInputs(std::vector<LVL1MUONIF::Lvl1MuCTPIInputPhase1*> inputs);
    void computeMultiplicities(int bcid);
    void makeTopoSelections();
    const std::vector<unsigned int>& getCTPData();
    const std::vector<unsigned int>& getDAQData();

  private:

    Configuration* m_conf;
    std::vector<unsigned int> m_ctp_words;
    std::vector<unsigned int> m_daq_data;

    LVL1MUONIF::Lvl1MuCTPIInputPhase1* m_mergedInputs;
    std::vector<TrigConf::TriggerThreshold*> m_thresholds;
  };
}

#endif // TRIGT1MUCTPIPHASE1_TRIGGERPROCESSOR_H
