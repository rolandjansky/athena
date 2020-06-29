/*
   Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
 */

#ifndef TOPCPTOOLS_TOPTRACKCPTOOLS_H_
#define TOPCPTOOLS_TOPTRACKCPTOOLS_H_

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
#include "InDetTrackSelectionTool/InDetTrackSelectionTool.h"

namespace top {
  class TopConfig;

  class TrackCPTools final: public asg::AsgTool {
  public:
    explicit TrackCPTools(const std::string& name);
    virtual ~TrackCPTools() {}

    StatusCode initialize();
  private:
    std::shared_ptr<top::TopConfig> m_config;

    std::vector<std::uint32_t> m_runPeriods;

    const std::string m_smearingToolName {
      "top::TrackCPTools::InDetTrackSmearingTool"
    };
    const std::string m_biasToolPrefix {
      "top::TrackCPTools::InDetTrackBiasingTool"
    };
    const std::string m_truthOriginToolName {
      "top::TrackCPTools::InDetTrackTruthOriginTool"
    };
    const std::string m_truthFilterToolName {
      "top::TrackCPTools::InDetTrackTruthFilterTool"
    };

    ToolHandle<InDet::InDetTrackSmearingTool>      m_smearingTool;
    ToolHandle<InDet::InDetTrackTruthOriginTool>   m_truthOriginTool;
    ToolHandle<InDet::InDetTrackTruthFilterTool>   m_truthFilterTool;

    ToolHandle<InDet::IInDetTrackSelectionTool> m_trkseltool;

    std::vector<ToolHandle<InDet::InDetTrackBiasingTool> > m_biasingTool;

    StatusCode setupSmearingTool();
    StatusCode setupBiasingTools();
    StatusCode setupTruthFilterTool();

  };
}  // namespace top

#endif  // TOPCPTOOLS_TOPTRACKCPTOOLS_H_
