/*
   Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
 */

#include "TopEventSelectionTools/GlobalTrigDecisionSelector.h"
#include "TopEvent/Event.h"
#include "TopEvent/EventTools.h"

#include <vector>
#include <iostream>


namespace top {
  GlobalTrigDecisionSelector::GlobalTrigDecisionSelector()
    : m_globalTriggerSF("TrigGlobalEfficiencyCorrectionTool::TrigGlobal")
    , m_globalTriggerSFLoose("TrigGlobalEfficiencyCorrectionTool::TrigGlobalLoose") {
  }

  bool GlobalTrigDecisionSelector::apply(top::Event const& event) const {
    auto&& tool = (event.m_isLoose ? m_globalTriggerSFLoose : m_globalTriggerSF);

    std::vector<std::string> triggers;
    top::check(tool->getRelevantTriggers(triggers), "TrigGlobalEfficiencyCorrectionTool::getRelevantTriggers failed");
    for (std::string const& trigger : triggers) {
      if (event.m_info->auxdataConst<char>("TRIGDEC_HLT_" + trigger) > 0) return true;
    }
    return false;
  }

  std::string GlobalTrigDecisionSelector::name() const {
    return "GTRIGDEC";
  }
}
