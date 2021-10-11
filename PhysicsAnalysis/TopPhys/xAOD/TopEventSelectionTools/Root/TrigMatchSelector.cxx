/*
   Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
 */

#include "TopEventSelectionTools/TrigMatchSelector.h"
#include "TopEvent/Event.h"
#include "TopConfiguration/TopConfig.h"

#include <sstream>
#include <iostream>

namespace top {
  TrigMatchSelector::TrigMatchSelector(const std::string& selectorName, std::shared_ptr<top::TopConfig> config) {
    m_allTriggers_Tight = config->allTriggers_Tight(selectorName);
    m_allTriggers_Loose = config->allTriggers_Loose(selectorName);
  }

  bool TrigMatchSelector::apply(const top::Event& event) const {
    // different treatment depending if it's a loose or tight top::Event
    bool loose = event.m_isLoose;

    // if no trigger menu us associated to this selection, return true
    // no effect of TRIGMATCH if TRIGDEC wasn't used
    if (!loose) {
      if (m_allTriggers_Tight.size() == 0) return true;
    } else {
      if (m_allTriggers_Loose.size() == 0) return true;
    }

    bool trigMatch(false);

    

    // Loop over triggers; loose ones for loose events, tight ones for tight events
    for (const auto& trigger : loose ? m_allTriggers_Loose : m_allTriggers_Tight) {
      int nObjects(0);
      // Loop over electrons
      for (const auto* const elPtr : event.m_electrons) {
        std::string trig = "TRIGMATCH_" + trigger.first;
        if (elPtr->isAvailable<char>(trig)) {
          if (elPtr->auxdataConst<char>(trig) == 1) {
            nObjects++;
          }
        } // decoration isAvailable
      } // Loop over electron

      // Loop over muons
      for (const auto* const muPtr : event.m_muons) {
        std::string trig = "TRIGMATCH_" + trigger.first;
        if (muPtr->isAvailable<char>(trig)) {
          if (muPtr->auxdataConst<char>(trig) == 1) {
            nObjects++;
          }
        } // decoration isAvailable
      } // Loop over muons

      // Loop over taus
      for (const auto* const tauPtr : event.m_tauJets) {
        std::string trig = "TRIGMATCH_" + trigger.first;
        if (tauPtr->isAvailable<char>(trig)) {
          if (tauPtr->auxdataConst<char>(trig) == 1) {
            nObjects++;
          }
        } // decoration isAvailable
      } // Loop over taus

      // Loop over photons
      for (const auto* const photonPtr : event.m_photons) {
        std::string trig = "TRIGMATCH_" + trigger.first;
        if (photonPtr->isAvailable<char>(trig)) {
          if (photonPtr->auxdataConst<char>(trig) == 1) {
            nObjects++;
          }
        } // decoration isAvailable
      } // Loop over photons
      if (nObjects >= trigger.second) trigMatch = true;
    } // Loop over triggers

    return trigMatch;
  }

  std::string TrigMatchSelector::name() const {
    std::string name = "TRIGMATCH";
    return name;
  }
}
