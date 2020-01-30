/*
   Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
 */

#include "TopEventSelectionTools/TrigDecisionLooseSelector.h"
#include "TopEvent/Event.h"
#include "TopConfiguration/TopConfig.h"

#include <sstream>
#include <iostream>

namespace top {
  TrigDecisionLooseSelector::TrigDecisionLooseSelector(const std::string& selectorName,
                                                       std::shared_ptr<top::TopConfig> config) {
    m_triggers = config->allTriggers_Loose(selectorName);

    std::cout << "Loose Triggers for selector = " << selectorName << std::endl;
    for (auto s : m_triggers) {
      std::cout << "--" << s << "--" << std::endl;
    }
  }

  bool TrigDecisionLooseSelector::apply(const top::Event& event) const {
    // this selector does nothing for non-loose events
    bool loose = event.m_isLoose;

    if (!loose) return true;

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

  std::string TrigDecisionLooseSelector::name() const {
    std::string name = "TRIGDEC_LOOSE ";
    for (auto trigger : m_triggers)
      name += " " + trigger;

    return name;
  }
}
