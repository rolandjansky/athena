/*
   Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
 */

#include "TopEventSelectionTools/TrigDecisionTightSelector.h"
#include "TopEvent/Event.h"
#include "TopConfiguration/TopConfig.h"

#include <sstream>
#include <iostream>

namespace top {
  TrigDecisionTightSelector::TrigDecisionTightSelector(const std::string& selectorName,
                                                       std::shared_ptr<top::TopConfig> config) {
    m_triggers = config->allTriggers_Tight(selectorName);

    std::cout << "Tight Triggers for selector = " << selectorName << std::endl;
    for (auto s : m_triggers) {
      std::cout << "--" << s << "--" << std::endl;
    }
  }

  bool TrigDecisionTightSelector::apply(const top::Event& event) const {
    // this selector does nothing for loose events
    bool loose = event.m_isLoose;

    if (loose) return true;

    bool orOfAllTriggers(false);
    for (const auto& trigger : m_triggers) {
      bool passThisTrigger(false);
      if (event.m_info->isAvailable<char>("TRIGDEC_" + trigger)) {
        if (event.m_info->auxdataConst<char>("TRIGDEC_" + trigger) == 1) {
          passThisTrigger = true;
        }
      }

      orOfAllTriggers |= passThisTrigger;
    }

    return orOfAllTriggers;
  }

  std::string TrigDecisionTightSelector::name() const {
    std::string name = "TRIGDEC_TIGHT ";
    for (auto trigger : m_triggers)
      name += " " + trigger;

    return name;
  }
}
