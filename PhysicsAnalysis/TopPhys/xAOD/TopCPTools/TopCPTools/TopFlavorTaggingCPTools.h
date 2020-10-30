/*
   Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
 */

#ifndef TOPCPTOOLS_TOPFLAVORTAGGINGCPTOOLS_H_
#define TOPCPTOOLS_TOPFLAVORTAGGINGCPTOOLS_H_

// Include what you use
#include <vector>
#include <string>
#include <map>

// Framework include(s):
#include "AsgTools/AsgTool.h"
#include "AsgTools/ToolHandle.h"
#include "AsgTools/ToolHandleArray.h"
#include "AsgTools/AnaToolHandle.h"

#include "TopConfiguration/TopConfig.h"

// Flavor tagging include(s):
#include "FTagAnalysisInterfaces/IBTaggingEfficiencyTool.h"
#include "FTagAnalysisInterfaces/IBTaggingSelectionTool.h"
// Need a pointer for excluded systematic functions
#include "xAODBTaggingEfficiency/BTaggingEfficiencyTool.h"

namespace top {
  class TopConfig;

  class FlavorTaggingCPTools final: public asg::AsgTool {
  public:
    explicit FlavorTaggingCPTools(const std::string& name);
    virtual ~FlavorTaggingCPTools() {}

    StatusCode initialize();
  private:
    std::shared_ptr<top::TopConfig> m_config;

    std::string m_tagger = "";
    std::string m_cdi_file = "";
    std::string m_efficiency_maps;
    const std::vector<std::string> m_jet_flavors = {
      "B", "C", "T", "Light"
    };
    std::vector<std::string> m_calo_WPs_calib;
    std::vector<std::string> m_calo_WPs;
    std::vector<std::string> m_trackAntiKtVR_WPs_calib;
    std::vector<std::string> m_trackAntiKtVR_WPs;
    std::vector<std::string> m_trackAntiKt2_WPs_calib;
    std::vector<std::string> m_trackAntiKt2_WPs;
    std::vector<std::string> m_trackAntiKt4_WPs_calib;
    std::vector<std::string> m_trackAntiKt4_WPs;
    std::vector<std::string> m_pflow_WPs_calib;
    std::vector<std::string> m_pflow_WPs;
    // Some tools here
    ToolHandleArray<IBTaggingEfficiencyTool> m_btagging_efficiency_tools;
    ToolHandleArray<IBTaggingSelectionTool> m_btagging_selection_tools;
    // EV decomposition functions
    StatusCode checkExcludedSysts(BTaggingEfficiencyTool*, std::string);
    void createExcludedSystMapping(std::vector<std::string>);
    std::map<std::string, std::string> m_mapped_excluded_systs;
    // Helper function for tracking tagger/WP/Calibration
    StatusCode setTaggerWorkingPoints(std::string, bool, std::string, std::vector<std::string>);
    void printConfigurations();
  };
}  // namespace top

#endif  // TOPCPTOOLS_TOPFLAVORTAGGINGCPTOOLS_H_
