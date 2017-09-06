/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TopEventSelectionTools/TrigDecisionSelector.h"
#include "TopEvent/Event.h"
#include "TopConfiguration/TopConfig.h"

#include <sstream>
#include <iostream>

namespace top {

  TrigDecisionSelector::TrigDecisionSelector(const std::string& selectorName,std::shared_ptr<top::TopConfig> config)
  {
    m_triggers = config->allTriggers(selectorName);

    std::cout<<"Triggers for selector = "<<selectorName<<std::endl;
    for (auto s : m_triggers) {
      std::cout << "--" << s << "--" << std::endl;
    }
       
  }

  bool TrigDecisionSelector::apply(const top::Event& event) const 
  {

    bool orOfAllTriggers(false);
    for (const auto& trigger : m_triggers){
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

  std::string TrigDecisionSelector::name() const {
    std::string name = "TRIGDEC ";
    for (auto trigger : m_triggers)
        name += " " + trigger;

    return name;
  }

}
