/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MUONTRACKMONITORING_RECOMUONTRACKPLOTS_H
#define MUONTRACKMONITORING_RECOMUONTRACKPLOTS_H

#include "xAODMuon/Muon.h"
#include "xAODMuon/MuonContainer.h"
#include "xAODTracking/TrackParticle.h"
#include "TrkEventPrimitives/ResidualPull.h"
#include "MuonIdHelpers/MuonStationIndex.h"

#include "TrkValHistUtils/PlotBase.h"
#include "TrkValHistUtils/ParamPlots.h"
#include "TrkValHistUtils/MSHitPlots.h"
#include "TrkValHistUtils/RecoInfoPlots.h"
#include "TrkValHistUtils/ImpactPlots.h"

#include "MuonHistUtils/MuonHitResidualPlots.h"
#include "TrkTrack/TrackStateOnSurface.h"
#include "TrkTrack/TrackCollection.h"
#include "TrkParameters/TrackParameters.h"
#include "MuonIdHelpers/MuonIdHelperTool.h"
#include "MuonRecHelperTools/MuonEDMHelperTool.h"
#include "TrkToolInterfaces/IResidualPullCalculator.h"


class RecoMuonTrackPlots:public PlotBase {
    public:
      RecoMuonTrackPlots(PlotBase* pParent, std::string sDir);
      
      // Reco only information
      Trk::ParamPlots                 m_oAllPlots;
      Trk::ImpactPlots                m_oImpactPlots;
      Trk::RecoInfoPlots              m_oTrkRecoInfoPlots;
      //Trk::MSHitPlots                 m_oMSHitPlots; 

      // Muon::MuonHitResidualPlots         m_oResidualPlots_Global;
      // Muon::MuonHitResidualPlots         m_oResidualPlots_MDT;
      // Muon::MuonHitResidualPlots         m_oResidualPlots_RPC_eta;
      // Muon::MuonHitResidualPlots         m_oResidualPlots_RPC_phi;
      // Muon::MuonHitResidualPlots         m_oResidualPlots_TGC_wire;
      // Muon::MuonHitResidualPlots         m_oResidualPlots_TGC_strip;
      // Muon::MuonHitResidualPlots         m_oResidualPlots_CSC_eta;
      // Muon::MuonHitResidualPlots         m_oResidualPlots_CSC_phi;
      
      TH1* m_pt_broad;
      TH2* m_eta_phi_broad;

      //ToolHandle<Trk::IResidualPullCalculator> m_pullCalculator;
      //ToolHandle<Muon::MuonIdHelperTool> m_idHelperTool;
      //ToolHandle<Muon::MuonEDMHelperTool> m_helperTool;

      //fill methods
      void fill(const xAOD::Muon& mu, int component);
      void fill(const xAOD::TrackParticle& muTP);
      void fill(const xAOD::TrackParticle& muTP, int LB_number, float LB_instant);
      //void fill(const Trk::ResidualPull& resPull, int stationPhi, Muon::MuonStationIndex::TechnologyIndex techid, bool measuresPhi);
    private:
      void initializePlots();

};

#endif
