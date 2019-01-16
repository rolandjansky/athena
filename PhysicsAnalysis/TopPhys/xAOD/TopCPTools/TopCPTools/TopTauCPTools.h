/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TOPCPTOOLS_TOPTAUCPTOOLS_H_
#define TOPCPTOOLS_TOPTAUCPTOOLS_H_

// Include what you use
#include <vector>
#include <string>

// Framework include(s):
#include "AsgTools/AsgTool.h"
#include "AsgTools/ToolHandle.h"
#include "AsgTools/ToolHandleArray.h"
#include "AsgTools/AnaToolHandle.h"

//PileupReweighting include:
#include "AsgAnalysisInterfaces/IPileupReweightingTool.h"

// Tau include(s):
#include "TauAnalysisTools/ITauSelectionTool.h"
#include "TauAnalysisTools/ITauSmearingTool.h"
#include "TauAnalysisTools/ITauEfficiencyCorrectionsTool.h"

namespace top {

class TopConfig;

class TauCPTools final : public asg::AsgTool {
 public:
  explicit TauCPTools(const std::string& name);
  virtual ~TauCPTools() {}

  StatusCode initialize();

 private:
  std::shared_ptr<top::TopConfig> m_config;

  int m_release_series = 24;  // Default to 2.4

  ToolHandle<TauAnalysisTools::ITauSelectionTool> m_tauSelectionTool;
  ToolHandle<TauAnalysisTools::ITauEfficiencyCorrectionsTool> m_tauEffCorrTool;
  ToolHandle<CP::IPileupReweightingTool> m_pileupReweightingTool;

  ToolHandle<TauAnalysisTools::ITauSelectionTool> m_tauSelectionToolLoose;
  ToolHandle<TauAnalysisTools::ITauEfficiencyCorrectionsTool> m_tauEffCorrToolLoose;

  ToolHandle<TauAnalysisTools::ITauSmearingTool> m_tauSmearingTool;

  StatusCode setupCalibration();
  StatusCode setupScaleFactors();
};
}  // namespace top

#endif  // TOPCPTOOLS_TOPTAUCPTOOLS_H_
