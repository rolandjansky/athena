/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TOPCPTOOLS_TOPTRIGGERCPTOOLS_H_
#define TOPCPTOOLS_TOPTRIGGERCPTOOLS_H_

// Include what you use
#include <vector>
#include <string>

// Framework include(s):
#include "AsgTools/AsgTool.h"
#include "AsgTools/ToolHandle.h"
#include "AsgTools/ToolHandleArray.h"
#include "AsgTools/AnaToolHandle.h"

// Trigger include(s):
#include "TrigConfInterfaces/ITrigConfigTool.h"
#include "TrigDecisionTool/TrigDecisionTool.h"
#include "TriggerMatchingTool/IMatchingTool.h"
#include "TrigTauMatching/ITrigTauMatching.h"
#include "TriggerAnalysisInterfaces/ITrigGlobalEfficiencyCorrectionTool.h"
// Need to have these tools held in the class
#include "EgammaAnalysisInterfaces/IAsgElectronEfficiencyCorrectionTool.h"
#include "MuonAnalysisInterfaces/IMuonTriggerScaleFactors.h"

namespace top {

class TopConfig;

class TriggerCPTools final : public asg::AsgTool {
 public:
  explicit TriggerCPTools(const std::string& name);
  virtual ~TriggerCPTools() {}

  StatusCode initialize();

 private:
  std::shared_ptr<top::TopConfig> m_config;

  ToolHandle<TrigConf::ITrigConfigTool> m_trigConfTool;
  ToolHandle<Trig::TrigDecisionTool> m_trigDecisionTool;
  ToolHandle<Trig::IMatchingTool> m_trigMatchTool;
  ToolHandle<Trig::ITrigTauMatchingTool> m_trigMatchTauTool;
  ToolHandle<ITrigGlobalEfficiencyCorrectionTool> m_globalTriggerEffTool;
  ToolHandle<ITrigGlobalEfficiencyCorrectionTool> m_globalTriggerEffToolLoose;
  StatusCode initialiseGlobalTriggerEff();
  std::string mapWorkingPoints(const std::string& type);

  // Tool handles for the CP tools, need to be members here, or inaccessible to global trigger tool
  asg::AnaToolHandle<CP::IMuonTriggerScaleFactors> m_muonTool;
  asg::AnaToolHandle<CP::IMuonTriggerScaleFactors> m_muonToolLoose;
  std::vector<asg::AnaToolHandle<IAsgElectronEfficiencyCorrectionTool>> m_electronToolsFactory;
  std::vector<asg::AnaToolHandle<IAsgElectronEfficiencyCorrectionTool>> m_electronToolsFactoryLoose;

};
}  // namespace top

#endif  // TOPCPTOOLS_TOPTRIGGERCPTOOLS_H_
