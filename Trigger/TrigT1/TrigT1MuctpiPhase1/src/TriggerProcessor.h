/*                                                                                                                      
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

// This file is really -*- C++ -*-.
#ifndef TRIGT1MUCTPIPHASE1_TRIGGERPROCESSOR_H
#define TRIGT1MUCTPIPHASE1_TRIGGERPROCESSOR_H

#include "TrigThresholdDecisionTool.h"
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

  struct DAQData {

    DAQData(unsigned word, 
	    const std::vector<std::pair<std::shared_ptr<TrigConf::L1Threshold>, bool> >& decisions)
    {
      dataWord=word;
      thresholdDecisions=decisions;
    }

    unsigned dataWord;
    std::vector<std::pair<std::shared_ptr<TrigConf::L1Threshold>, bool> > thresholdDecisions;
  };

  class TriggerProcessor
  {
    
  public:
    
    TriggerProcessor() = default;

    void setMenu(const TrigConf::L1Menu* l1menu);
    void setTrigTool(LVL1::TrigThresholdDecisionTool& tool) {m_trigThresholdDecisionTool=&tool;}
    void mergeInputs(std::vector<const LVL1MUONIF::Lvl1MuCTPIInputPhase1*> inputs);
    std::string computeMultiplicities(int bcid);
    void makeTopoSelections();
    const std::vector<unsigned int>& getCTPData() const;

    //subsystem - daq word pairs
    const std::vector<DAQData>& getDAQData() const;

  private:

    std::vector<unsigned int> m_ctp_words;
    std::vector<DAQData> m_daq_data;

    LVL1MUONIF::Lvl1MuCTPIInputPhase1 m_mergedInputs;
    const TrigConf::L1Menu* m_l1menu{nullptr};
    LVL1::TrigThresholdDecisionTool* m_trigThresholdDecisionTool{nullptr};

  };
}

#endif // TRIGT1MUCTPIPHASE1_TRIGGERPROCESSOR_H
