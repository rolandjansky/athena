/*
Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
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
  class MuonTrackRemoval : virtual public TauRecToolBase {
  public:

    ASG_TOOL_CLASS2( MuonTrackRemoval, TauRecToolBase, ITauToolBase )

    MuonTrackRemoval(const std::string& type);
    ~MuonTrackRemoval();


    virtual StatusCode initialize();
    virtual StatusCode finalize();
    virtual StatusCode execute(xAOD::TauJet& xTau);
    virtual StatusCode eventInitialize() { return StatusCode::SUCCESS; }
    virtual StatusCode eventFinalize() { return StatusCode::SUCCESS; }

  private:
    int m_iMaxMuonIDWP; //!
    asg::AnaToolHandle<CP::IMuonSelectionTool>* m_thMuonSelectionTool;//!
    asg::AnaToolHandle<CP::IMuonCalibrationAndSmearingTool>* m_thMuonCalibrationTool;//!
  };
}

#endif// TAURECTOOLS_MUONTRACKREMOVAL_H

