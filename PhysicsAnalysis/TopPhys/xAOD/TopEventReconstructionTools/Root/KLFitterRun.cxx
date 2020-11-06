/*
   Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
 */

#include "TopEventReconstructionTools/KLFitterRun.h"

#include "TopEvent/Event.h"
#include "TopEvent/EventTools.h"
#include "TopConfiguration/TopConfig.h"

#include <iostream>
#include <vector>
#include <cmath>
#include <fstream>

namespace top {
  KLFitterRun::KLFitterRun(const std::string& kSelectionName, const std::string& kParameters,
                           std::shared_ptr<top::TopConfig> config) :
    m_name(""),
    m_myFitter(nullptr) {
    std::string kLeptonType = "";
    std::string kCustomParameters = "";
    if (kParameters.find(" ") != std::string::npos) {
      kLeptonType = kParameters.substr(0, kParameters.find(" "));
      kCustomParameters = kParameters.substr(kParameters.find(" ") + 1);
    } else kLeptonType = kParameters;
    m_name = "RECO::KLFitterRun_" + kLeptonType;
    m_myFitter = std::unique_ptr<top::KLFitterTool> (new top::KLFitterTool(m_name));
    top::check(m_myFitter->setProperty("config", config), "Failed to setProperty of KLFitterTool");
    top::check(m_myFitter->setProperty("LeptonType", kLeptonType), "Failed to setProperty of KLFitterTool");
    top::check(m_myFitter->setProperty("CustomParameters", kCustomParameters), "Failed to setProperty of KLFitterTool");
    top::check(m_myFitter->setProperty("SelectionName", kSelectionName), "Failed to setProperty of KLFitterTool");
    top::check(m_myFitter->initialize(), "Failed to initialize KLFitterTool");
  }

  bool KLFitterRun::apply(const top::Event& event) const {
    top::check(m_myFitter->execute(event), "Failed to run KLFitterTool");
    return true;
  }

  std::string KLFitterRun::name() const {
    return m_name;
  }
}
