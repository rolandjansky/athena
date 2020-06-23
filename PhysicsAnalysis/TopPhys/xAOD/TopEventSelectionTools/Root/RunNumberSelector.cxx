/*
   Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
 */

#include "TopEventSelectionTools/RunNumberSelector.h"
#include "TopEvent/EventTools.h"
#include "TopConfiguration/TopConfig.h"

namespace top {
  RunNumberSelector::RunNumberSelector(const std::string& params, std::shared_ptr<top::TopConfig> config) :
    SignValueSelector("RUN_NUMBER", params) {
    m_config = config;
  }

  bool RunNumberSelector::apply(const top::Event& event) const {
    unsigned int number = 0;

    if (!m_config->isMC()) number = event.m_info->runNumber();
    else {
      if (event.m_info->isAvailable<unsigned int>("RandomRunNumber")) number = event.m_info->auxdataConst<unsigned int>(
          "RandomRunNumber");
      else {
        throw std::runtime_error("RunNumberSelector: RandomRunNumber EventInfo decoration not available. Indicates issue with PRW tool.");
      }
    }

    return checkInt(number, value());
  }
}
