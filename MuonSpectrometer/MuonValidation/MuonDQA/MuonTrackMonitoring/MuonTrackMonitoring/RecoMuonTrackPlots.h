/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MUONTRACKMONITORING_RECOMUONTRACKPLOTS_H
#define MUONTRACKMONITORING_RECOMUONTRACKPLOTS_H

#include "xAODMuon/Muon.h"
#include "xAODMuon/MuonContainer.h"
#include "xAODTracking/TrackParticle.h"
#include "TrkEventPrimitives/ResidualPull.h"

#include "TrkValHistUtils/PlotBase.h"
#include "TrkValHistUtils/ParamPlots.h"
#include "TrkValHistUtils/MSHitPlots.h"
#include "TrkValHistUtils/RecoInfoPlots.h"
#include "TrkValHistUtils/ImpactPlots.h"

#include "MuonHistUtils/MuonHitResidualPlots.h"
#include "TrkTrack/TrackStateOnSurface.h"
#include "TrkTrack/TrackCollection.h"
#include "TrkParameters/TrackParameters.h"


class RecoMuonTrackPlots:public PlotBase {
    public:
      RecoMuonTrackPlots(PlotBase* pParent, std::string sDir);

      // Reco only information
      Trk::ParamPlots                 m_oAllPlots;
      Trk::ImpactPlots                m_oImpactPlots;
      Trk::RecoInfoPlots              m_oTrkRecoInfoPlots;

      TH1* m_pt_broad;
      TH2* m_eta_phi_broad;

      //fill methods
      void fill(const xAOD::Muon& mu, int component);
      void fill(const xAOD::TrackParticle& muTP);
      void fill(const xAOD::TrackParticle& muTP, int LB_number, float LB_instant);
    private:
      void initializePlots();

};

#endif
