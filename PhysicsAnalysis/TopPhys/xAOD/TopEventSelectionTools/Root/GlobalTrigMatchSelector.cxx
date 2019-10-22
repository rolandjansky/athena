/*
   Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
 */

#include "TopEventSelectionTools/GlobalTrigMatchSelector.h"
#include "TopEvent/Event.h"
#include "TopEvent/EventTools.h"

#include "xAODBase/IParticle.h"
#include "xAODEgamma/ElectronContainer.h"
#include "xAODMuon/MuonContainer.h"

#include <iostream>
#include <vector>


namespace top {
  GlobalTrigMatchSelector::GlobalTrigMatchSelector()
    : m_globalTriggerSF("TrigGlobalEfficiencyCorrectionTool::TrigGlobal")
    , m_globalTriggerSFLoose("TrigGlobalEfficiencyCorrectionTool::TrigGlobalLoose") {
  }

  bool GlobalTrigMatchSelector::apply(top::Event const& event) const {
    auto&& tool = (event.m_isLoose ? m_globalTriggerSFLoose : m_globalTriggerSF);
    bool result = false;

    std::vector<xAOD::IParticle const*> particles;
    particles.insert(particles.end(), event.m_electrons.begin(), event.m_electrons.end());
    particles.insert(particles.end(), event.m_muons.begin(), event.m_muons.end());
    top::check(tool->checkTriggerMatching(result,
                                          particles),
               "TrigGlobalEfficiencyCorrectionTool::checkTriggerMatching failed");
    return result;
  }

  std::string GlobalTrigMatchSelector::name() const {
    return "GTRIGMATCH";
  }
}
