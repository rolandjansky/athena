/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TAURECTOOLS_MUONTRACKREMOVAL_H
#define TAURECTOOLS_MUONTRACKREMOVAL_H

#include "AsgTools/AnaToolHandle.h"
#include "tauRecTools/TauRecToolBase.h"
#include "xAODTau/TauJet.h"

namespace CP{
  class IMuonSelectionTool;
  class IMuonCalibrationAndSmearingTool;
}

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
    asg::AnaToolHandle<CP::IMuonSelectionTool>* m_thMuonSelectionTool;//!
    asg::AnaToolHandle<CP::IMuonCalibrationAndSmearingTool>* m_thMuonCalibrationTool;//!
  };
}

#endif// TAURECTOOLS_MUONTRACKREMOVAL_H

