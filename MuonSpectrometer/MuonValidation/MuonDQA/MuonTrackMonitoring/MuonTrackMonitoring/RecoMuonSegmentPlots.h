/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MUONTRACKMONITORING_RECOMUONSEGMENTPLOTS_H
#define MUONTRACKMONITORING_RECOMUONSEGMENTPLOTS_H

#include "TrkValHistUtils/PlotBase.h"
#include "MuonHistUtils/MuonSegmentPlots.h"

#include "xAODMuon/Muon.h"
#include "xAODMuon/MuonContainer.h"
#include "xAODMuon/MuonSegment.h"
#include "xAODMuon/MuonSegmentContainer.h"
#include "xAODTracking/TrackParticleContainer.h"
//#include "AthenaMonitoring/ManagedMonitorToolBase.h"


namespace Muon {
  class IMuonSegmentTrackBuilder;
}

class RecoMuonSegmentPlots:public PlotBase {
 public:
      RecoMuonSegmentPlots(PlotBase* pParent, std::string sDir);

      //fill methods
      void fill(const xAOD::MuonContainer& muContainer);
      void fill(const xAOD::Muon& mu);
      void fill(const xAOD::MuonSegment& muonSeg);

      //generic plots from MuonHistUtils
      Muon::MuonSegmentPlots m_oMuonSegmentPlots;

      //Monitoring justification plots



 private:
      void initializePlots();
      

};

#endif
