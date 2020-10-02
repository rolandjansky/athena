/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MUONTRACKMONITORING_RECOMUONSEGMENTPLOTS_H
#define MUONTRACKMONITORING_RECOMUONSEGMENTPLOTS_H

#include "TrkValHistUtils/PlotBase.h"
#include "MuonHistUtils/MuonSegmentPlots.h"
#include "MuonHistUtils/MuonSegmentSlimPlots.h"

#include "xAODMuon/Muon.h"
#include "xAODMuon/MuonContainer.h"
#include "xAODMuon/MuonSegment.h"
#include "xAODMuon/MuonSegmentContainer.h"
#include "xAODTracking/TrackParticleContainer.h"

class RecoMuonSegmentPlots:public PlotBase {
 public:
      RecoMuonSegmentPlots(PlotBase* pParent, std::string sDir, bool detail=false);

      //fill methods
      void fill(const xAOD::MuonContainer& muContainer);
      void fill(const xAOD::Muon& mu);
      void fill(const std::vector<ElementLink<DataVector<xAOD::MuonSegment_v1>>> Mu_Segments);
      void fill(const xAOD::MuonSegment& muonSeg);

      //generic plots from MuonHistUtils
      Muon::MuonSegmentPlots *m_oMuonSegmentPlots;
      Muon::MuonSegmentSlimPlots *m_oMuonSegmentSlimPlots;

      //Monitoring justification plots
      bool Detail;


 private:
      void initializePlots();
      

};

#endif
