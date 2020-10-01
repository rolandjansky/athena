/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MUONTRACKMONITORING_RECOMUONIDTRACKPLOTS_H
#define MUONTRACKMONITORING_RECOMUONIDTRACKPLOTS_H

#include "xAODMuon/Muon.h"
#include "xAODMuon/MuonContainer.h"
#include "xAODTracking/TrackParticle.h"
//#include "TrkEventPrimitives/ResidualPull.h"

#include "TrkValHistUtils/PlotBase.h"
#include "TrkValHistUtils/ParamPlots.h"
#include "TrkValHistUtils/RecoInfoPlots.h"
#include "TrkValHistUtils/ImpactPlots.h"
#include "TrkValHistUtils/IDHitPlots.h"


class RecoMuonIDTrackPlots:public PlotBase {
    public:
      RecoMuonIDTrackPlots(PlotBase* pParent, std::string sDir);
      
      // Reco only information
      Trk::ParamPlots                 m_oAllPlots;
      Trk::ImpactPlots                m_oImpactPlots;
      Trk::RecoInfoPlots              m_oTrkRecoInfoPlots;
      Trk::IDHitPlots                 m_oIDHitPlots;

      TH1* m_pt_broad;
      TH2* m_eta_phi_broad;

      //fill methods
      void fill(const xAOD::Muon& mu, int component);
      void fill(const xAOD::TrackParticle& muTP);
      //void fill(const xAOD::TrackParticle& muTP, int LB_number, float LB_instant);
      //void fill(const Trk::ResidualPull& resPull, int stationPhi, Muon::MuonStationIndex::TechnologyIndex techid, bool measuresPhi);
    private:
      void initializePlots();

};

#endif
