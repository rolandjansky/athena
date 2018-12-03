/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TAURECTOOLS_MUONTRACKREMOVAL_H
#define TAURECTOOLS_MUONTRACKREMOVAL_H

#include "tauRecTools/TauRecToolBase.h"
#include "xAODTau/TauJet.h"
#include "MuonAnalysisInterfaces/IMuonCalibrationAndSmearingTool.h"
#include "MuonAnalysisInterfaces/IMuonSelectionTool.h"

namespace tauRecTools
{
  class MuonTrackRemoval : public TauRecToolBase {
  public:

    ASG_TOOL_CLASS2( MuonTrackRemoval, TauRecToolBase, ITauToolBase )

    MuonTrackRemoval(const std::string& type);
    ~MuonTrackRemoval();

    virtual StatusCode initialize() override;
    virtual StatusCode finalize() override;
    virtual StatusCode execute(xAOD::TauJet& xTau) override;
    virtual StatusCode eventInitialize() override { return StatusCode::SUCCESS; }
    virtual StatusCode eventFinalize() override { return StatusCode::SUCCESS; }

  private:
    int m_iMaxMuonIDWP; //!
    ToolHandle<CP::IMuonCalibrationAndSmearingTool> m_thMuonCalibrationTool;//!
    ToolHandle<CP::IMuonSelectionTool> m_thMuonSelectionTool;//!
  };
}

#endif// TAURECTOOLS_MUONTRACKREMOVAL_H

