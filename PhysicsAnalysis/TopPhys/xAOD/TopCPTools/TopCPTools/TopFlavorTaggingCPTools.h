/*
   Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
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
    std::string m_cdi_file;
    std::string m_calib_file_path;
    std::string m_excluded_systs;
    std::string m_efficiency_maps;
    const std::vector<std::string> m_jet_flavors = {
      "B", "C", "T", "Light"
    };
    // Some tools here
    ToolHandleArray<IBTaggingEfficiencyTool> m_btagging_efficiency_tools;
    ToolHandleArray<IBTaggingSelectionTool> m_btagging_selection_tools;

    /**
     * @brief Setup BTaggingSelectionTool for a given WP
     *
     * @param btag_algo_WP pair of tagger (e.g. DL1r) and WP name (e.g. FixedCutBEff77)
     * @param jetCollection name of the jet collection for the btagging tool
     * @param jetPtCut minimum pT cut used for jets
     * @param jetEtaCut maximum |eta| cut used for jets
     * @param trackJets true, if btagging for track jets is to be initialized, otherwise false
    */
    StatusCode setupBtagSelectionTool(const std::pair<std::string, std::string>& btag_algo_WP,
                                      const std::string& jetCollection,
                                      double jetPtCut, double jetEtaCut,
                                      bool trackJets=false);
    // setup btagging efficiency tool, see documentation of setupBtagSelectionTool above
    StatusCode setupBtagEfficiencyTool(const std::pair<std::string, std::string>& btag_algo_WP,
                                       const std::string& jetCollection,
                                       double jetPtCut,
                                       bool trackJets=false);

    // workaround method to erase PV0 from VR track jet collection name
    // needed for the currently-bugged CDI file
    std::string erasePV0fromJetsName(std::string jetCollectionName);

    // EV decomposition functions
    StatusCode checkExcludedSysts(BTaggingEfficiencyTool*, std::string);
  };
}  // namespace top

#endif  // TOPCPTOOLS_TOPFLAVORTAGGINGCPTOOLS_H_
