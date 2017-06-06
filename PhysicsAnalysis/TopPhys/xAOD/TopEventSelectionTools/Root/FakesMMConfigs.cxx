/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TopEventSelectionTools/FakesMMConfigs.h"

namespace top {

  FakesMMConfigs::FakesMMConfigs(const std::string& configs) {
    std::istringstream iss(configs);
    std::string config;
    while (iss>>config) {
      m_configurations.push_back(config);
    }
  }

  bool FakesMMConfigs::apply(const top::Event&) const {
    return true;
  }

  std::string FakesMMConfigs::name() const {
    return "FAKESMMCONFIGS";
  }

}
