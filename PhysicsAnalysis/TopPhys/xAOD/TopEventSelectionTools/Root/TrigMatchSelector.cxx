/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TopEventSelectionTools/TrigMatchSelector.h"
#include "TopEvent/Event.h"
#include "TopConfiguration/TopConfig.h"

#include <sstream>
#include <iostream>

namespace top {

  TrigMatchSelector::TrigMatchSelector(const std::string& selectorName,std::shared_ptr<top::TopConfig> config)
  {
    m_electronTriggers = config->electronTriggers(selectorName);
    m_muonTriggers = config->muonTriggers(selectorName);
    m_tauTriggers = config->tauTriggers(selectorName);
    
    std::cout<<"Triggers Matching for selector = "<<selectorName<<std::endl;
    for (auto s : m_electronTriggers) {
      std::cout << "--Electron Trigger = " << s << "--" << std::endl;      
    }
    for (auto s : m_muonTriggers) {
      std::cout << "--Muon Trigger = " << s << "--" << std::endl;      
    }
    for (auto s : m_tauTriggers) {
      std::cout << "--Tau Trigger = " << s << "--" << std::endl;      
    }    
  }

  bool TrigMatchSelector::apply(const top::Event& event) const 
  {
    // if no trigger menu us associated to this selection, return true
    // no effect of TRIGMATCH if TRIGDEC wasn't used
    if (m_electronTriggers.size()+m_muonTriggers.size()+m_tauTriggers.size() == 0) return true;
    
    bool trigMatch(false);
    
    // Loop over electrons
    for (const auto* const elPtr : event.m_electrons) {
      // Loop over triggers 
      for (const auto& trigger : m_electronTriggers) {
        std::string trig = "TRIGMATCH_" + trigger;
        if (elPtr->isAvailable<char>(trig)) {
          if (elPtr->auxdataConst<char>(trig) == 1) {
            trigMatch = true;
            return trigMatch;
          }
        } // decoration isAvailable
      } // Loop over triggers
    } // Loop over electrons 
    
    // Loop over muons
    for (const auto* const muPtr : event.m_muons) {
      // Loop over triggers 
      for (const auto& trigger : m_muonTriggers) {  
        std::string trig = "TRIGMATCH_" + trigger;
        if (muPtr->isAvailable<char>(trig)) {
          if (muPtr->auxdataConst<char>(trig) == 1) {
            trigMatch = true;
            return trigMatch;
          }
        } // decoration isAvailable
      } // Loop over triggers
    } // Loop over muons


    // Loop over taus
    for (const auto* const tauPtr : event.m_tauJets) {
      // Loop over triggers 
      for (const auto& trigger : m_tauTriggers) {
        std::string trig = "TRIGMATCH_" + trigger;
        if (tauPtr->isAvailable<char>(trig)) {
          if (tauPtr->auxdataConst<char>(trig) == 1) {
            trigMatch = true;
            return trigMatch;
          }
        } // decoration isAvailable
      } // Loop over triggers
    } // Loop over taus
    
    return trigMatch;
  }
  
  std::string TrigMatchSelector::name() const {
    std::string name = "TRIGMATCH";
    return name;
  }

}
