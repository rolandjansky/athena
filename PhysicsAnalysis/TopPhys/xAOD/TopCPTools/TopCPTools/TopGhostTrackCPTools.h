/*
   Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
 */

#ifndef TOPCPTOOLS_TOPGHOSTTRACKCPTOOLS_H_
#define TOPCPTOOLS_TOPGHOSTTRACKCPTOOLS_H_

// Include what you use
#include <vector>
#include <string>

// Framework include(s):
#include "AsgTools/AsgTool.h"
#include "AsgTools/ToolHandle.h"
#include "AsgTools/ToolHandleArray.h"
#include "AsgTools/AnaToolHandle.h"

// Tracking include(s):
#include "InDetTrackSystematicsTools/InDetTrackSystematics.h"
#include "InDetTrackSystematicsTools/InDetTrackSmearingTool.h"
#include "InDetTrackSystematicsTools/InDetTrackBiasingTool.h"
#include "InDetTrackSystematicsTools/InDetTrackTruthFilterTool.h"
#include "InDetTrackSystematicsTools/InDetTrackTruthOriginTool.h"
#include "InDetTrackSystematicsTools/JetTrackFilterTool.h"
#include "InDetTrackSystematicsTools/JetTrackFilterTool.h"
#include "InDetTrackSelectionTool/InDetTrackSelectionTool.h"



namespace top {
  class TopConfig;

  class GhostTrackCPTools final: public asg::AsgTool {
  public:
    explicit GhostTrackCPTools(const std::string& name);
    virtual ~GhostTrackCPTools() {}

    StatusCode initialize();
  private:
    std::shared_ptr<top::TopConfig> m_config;

    std::vector<std::uint32_t> m_runPeriods;

    const std::string m_smearingToolName {
      "top::GhostTrackCPTools::InDetTrackSmearingTool"
    };
    const std::string m_biasToolPrefix {
      "top::GhostTrackCPTools::InDetTrackBiasingTool"
    };
    const std::string m_truthOriginToolName {
      "top::GhostTrackCPTools::InDetTrackTruthOriginTool"
    };
    const std::string m_truthFilterToolName {
      "top::GhostTrackCPTools::InDetTrackTruthFilterTool"
    };
    const std::string m_jetTrackFilterToolName {
      "top::GhostTrackCPTools::JetTrackFilterTool"
    };
    
    const std::string m_TrkSelName {
      "top::GhostTrackCPTools::TrkSelTool"
    };

    ToolHandle<InDet::InDetTrackSmearingTool>      m_smearingTool;
    ToolHandle<InDet::InDetTrackTruthOriginTool>   m_truthOriginTool;
    ToolHandle<InDet::InDetTrackTruthFilterTool>   m_truthFilterTool;
    ToolHandle<InDet::JetTrackFilterTool>          m_jetTrackFilterTool;
    ToolHandle<InDet::InDetTrackSelectionTool>     m_trackseltool;
    

    std::vector<ToolHandle<InDet::InDetTrackBiasingTool> > m_biasingTool;

    StatusCode setupSmearingTool();
    StatusCode setupBiasingTools();
    StatusCode setupTruthFilterTool();
    StatusCode setupJetTrackFilterTool();
    StatusCode setupSelectionTool();
  };
}  // namespace top

#endif  // TOPCPTOOLS_TOPGHOSTTRACKCPTOOLS_H_
