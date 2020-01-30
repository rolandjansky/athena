/*
   Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
 */

#include "TopEventSelectionTools/TrigMatchSelector.h"
#include "TopEvent/Event.h"
#include "TopConfiguration/TopConfig.h"

#include <sstream>
#include <iostream>

namespace top {
  TrigMatchSelector::TrigMatchSelector(const std::string& selectorName, std::shared_ptr<top::TopConfig> config) {
    m_electronTriggers_Tight = config->electronTriggers_Tight(selectorName);
    m_muonTriggers_Tight = config->muonTriggers_Tight(selectorName);
    m_tauTriggers_Tight = config->tauTriggers_Tight(selectorName);
    m_electronTriggers_Loose = config->electronTriggers_Loose(selectorName);
    m_muonTriggers_Loose = config->muonTriggers_Loose(selectorName);
    m_tauTriggers_Loose = config->tauTriggers_Loose(selectorName);

    std::cout << "Triggers Matching for selector = " << selectorName << std::endl;
    for (auto s : m_electronTriggers_Tight)
      std::cout << "--Electron Trigger (Tight) = " << s << "--" << std::endl;
    for (auto s : m_muonTriggers_Tight)
      std::cout << "--Muon Trigger (Tight) = " << s << "--" << std::endl;
    for (auto s : m_tauTriggers_Tight)
      std::cout << "--Tau Trigger (Tight) = " << s << "--" << std::endl;
    for (auto s : m_electronTriggers_Loose)
      std::cout << "--Electron Trigger (Loose) = " << s << "--" << std::endl;
    for (auto s : m_muonTriggers_Loose)
      std::cout << "--Muon Trigger (Loose) = " << s << "--" << std::endl;
    for (auto s : m_tauTriggers_Loose)
      std::cout << "--Tau Trigger (Loose) = " << s << "--" << std::endl;
  }

  bool TrigMatchSelector::apply(const top::Event& event) const {
    // different treatment depending if it's a loose or tight top::Event
    bool loose = event.m_isLoose;

    // if no trigger menu us associated to this selection, return true
    // no effect of TRIGMATCH if TRIGDEC wasn't used
    if (!loose) {
      if (m_electronTriggers_Tight.size() + m_muonTriggers_Tight.size() + m_tauTriggers_Tight.size() == 0) return true;
    } else {
      if (m_electronTriggers_Loose.size() + m_muonTriggers_Loose.size() + m_tauTriggers_Loose.size() == 0) return true;
    }

    bool trigMatch(false);

    // Loop over electrons
    for (const auto* const elPtr : event.m_electrons) {
      // Loop over triggers; loose ones for loose events, tight ones for tight events
      for (const auto& trigger : loose ? m_electronTriggers_Loose : m_electronTriggers_Tight) {
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
      // Loop over triggers; loose ones for loose events, tight ones for tight events
      for (const auto& trigger : loose ? m_muonTriggers_Loose : m_muonTriggers_Tight) {
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
      // Loop over triggers; loose ones for loose events, tight ones for tight events
      for (const auto& trigger : loose ? m_tauTriggers_Loose : m_tauTriggers_Tight) {
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
