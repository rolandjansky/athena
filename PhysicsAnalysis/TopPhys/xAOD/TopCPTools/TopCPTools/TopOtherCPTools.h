/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TOPCPTOOLS_TOPOTHERCPTOOLS_H_
#define TOPCPTOOLS_TOPOTHERCPTOOLS_H_

// Include what you use
#include <vector>
#include <string>

// Framework include(s):
#include "AsgTools/AsgTool.h"
#include "AsgTools/ToolHandle.h"
#include "AsgTools/ToolHandleArray.h"
#include "AsgTools/AnaToolHandle.h"

// GRL include(s):
#include "AsgAnalysisInterfaces/IGoodRunsListSelectionTool.h"

// Pileup Reweighting include(s):
#include "AsgAnalysisInterfaces/IPileupReweightingTool.h"

// PMG Tools
#include "PMGTools/PMGSherpa22VJetsWeightTool.h"

namespace top {

class TopConfig;

class OtherCPTools final : public asg::AsgTool {
 public:
  explicit OtherCPTools(const std::string& name);
  virtual ~OtherCPTools() {}

  StatusCode initialize();

 private:
  std::shared_ptr<top::TopConfig> m_config;

  int m_release_series = 24;  // Default to 2.4

  ToolHandle<IGoodRunsListSelectionTool> m_grlTool;
  ToolHandle<CP::IPileupReweightingTool> m_pileupReweightingTool;
  ToolHandle<PMGTools::PMGSherpa22VJetsWeightTool> m_pmg_sherpa22_vjets_tool;

  StatusCode setupGRL();
  StatusCode setupPileupReweighting();
  StatusCode setupPMGTools();
};
}  // namespace top

#endif  // TOPCPTOOLS_TOPOTHERCPTOOLS_H_
