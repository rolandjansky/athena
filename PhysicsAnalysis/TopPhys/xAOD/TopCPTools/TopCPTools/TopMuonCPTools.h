/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TOPCPTOOLS_TOPMUONCPTOOLS_H_
#define TOPCPTOOLS_TOPMUONCPTOOLS_H_

// Include what you use
#include <vector>
#include <string>

// Framework include(s):
#include "AsgTools/AsgTool.h"
#include "AsgTools/ToolHandle.h"
#include "AsgTools/ToolHandleArray.h"
#include "AsgTools/AnaToolHandle.h"

// Muon include(s):
#include "MuonAnalysisInterfaces/IMuonCalibrationAndSmearingTool.h"
#include "MuonAnalysisInterfaces/IMuonSelectionTool.h"
#include "MuonAnalysisInterfaces/IMuonTriggerScaleFactors.h"
#include "MuonAnalysisInterfaces/IMuonEfficiencyScaleFactors.h"

namespace top {

class TopConfig;

class MuonCPTools final : public asg::AsgTool {
 public:
  explicit MuonCPTools(const std::string& name);
  virtual ~MuonCPTools() {}

  StatusCode initialize();

 private:
  std::shared_ptr<top::TopConfig> m_config;

  int m_release_series = 24;  // Default to 2.4

  ToolHandle<CP::IMuonCalibrationAndSmearingTool> m_muonCalibrationAndSmearingTool;
  ToolHandle<CP::IMuonSelectionTool> m_muonSelectionTool;
  ToolHandle<CP::IMuonSelectionTool> m_muonSelectionToolLoose;
  // the following is needed to make sure all muons for which d0sig is calculated are at least Loose
  ToolHandle<CP::IMuonSelectionTool> m_muonSelectionToolVeryLooseVeto;

  ToolHandle<CP::IMuonTriggerScaleFactors> m_muonTriggerScaleFactors_2015;
  ToolHandle<CP::IMuonTriggerScaleFactors> m_muonTriggerScaleFactorsLoose_2015;
  ToolHandle<CP::IMuonTriggerScaleFactors> m_muonTriggerScaleFactors_2016;
  ToolHandle<CP::IMuonTriggerScaleFactors> m_muonTriggerScaleFactorsLoose_2016;
  ToolHandle<CP::IMuonTriggerScaleFactors> m_muonTriggerScaleFactors_R21;
  ToolHandle<CP::IMuonTriggerScaleFactors> m_muonTriggerScaleFactorsLoose_R21;

  ToolHandle<CP::IMuonEfficiencyScaleFactors> m_muonEfficiencyCorrectionsTool;
  ToolHandle<CP::IMuonEfficiencyScaleFactors> m_muonEfficiencyCorrectionsToolLoose;
  ToolHandle<CP::IMuonEfficiencyScaleFactors> m_muonEfficiencyCorrectionsToolIso;
  ToolHandle<CP::IMuonEfficiencyScaleFactors> m_muonEfficiencyCorrectionsToolLooseIso;
  ToolHandle<CP::IMuonEfficiencyScaleFactors> m_muonEfficiencyCorrectionsToolPromptLeptonIso;
  ToolHandle<CP::IMuonEfficiencyScaleFactors> m_muonEfficiencyCorrectionsToolTTVA;



  StatusCode setupCalibration();
  StatusCode setupScaleFactors();

  CP::IMuonSelectionTool*
    setupMuonSelectionTool(const std::string& name, const std::string& quality, double max_eta);

  CP::IMuonTriggerScaleFactors*
    setupMuonTrigSFTool(const std::string& name, const std::string& quality, const std::string& year);

  CP::IMuonEfficiencyScaleFactors*
    setupMuonSFTool(const std::string& name, const std::string& WP);
};
}  // namespace top

#endif  // TOPCPTOOLS_TOPMUONCPTOOLS_H_
