/*                                                                                                                      
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration                                               
*/

// This file is really -*- C++ -*-.
#ifndef TRIGT1MUCTPIPHASE1_TRIGGERPROCESSOR_H
#define TRIGT1MUCTPIPHASE1_TRIGGERPROCESSOR_H

#include <vector>
#include <list>
#include <map>
#include <string>
#include <utility>

namespace TrigConf {
  class L1Menu;
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

    void setMenu(const TrigConf::L1Menu* l1menu);
    void mergeInputs(std::vector<LVL1MUONIF::Lvl1MuCTPIInputPhase1*> inputs);
    void computeMultiplicities(int bcid);
    void makeTopoSelections();
    const std::vector<unsigned int>& getCTPData();

    //subsystem - daq word pairs
    const std::vector<std::pair<int, unsigned int> >& getDAQData();

  private:

    std::vector<std::string> parseString(std::string str, std::string sep);

    std::vector<unsigned int> m_ctp_words;
    std::vector<std::pair<int, unsigned int> > m_daq_data;

    LVL1MUONIF::Lvl1MuCTPIInputPhase1* m_mergedInputs;
    const TrigConf::L1Menu* m_l1menu;

    std::map<std::string, std::vector<std::vector<std::string> > > m_parsed_tgcFlags;
  };
}

#endif // TRIGT1MUCTPIPHASE1_TRIGGERPROCESSOR_H
