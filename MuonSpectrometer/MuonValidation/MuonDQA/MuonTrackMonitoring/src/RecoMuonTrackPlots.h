/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MUONTRACKMONITORING_RECOMUONTRACKPLOTS_H
#define MUONTRACKMONITORING_RECOMUONTRACKPLOTS_H

#include "xAODTracking/TrackParticle.h"
#include "TrkEventPrimitives/ResidualPull.h"
#include "MuonIdHelpers/MuonStationIndex.h"

#include "TrkValHistUtils/PlotBase.h"
#include "TrkValHistUtils/ParamPlots.h"
#include "TrkValHistUtils/MSHitPlots.h"
#include "TrkValHistUtils/RecoInfoPlots.h"
#include "TrkValHistUtils/ImpactPlots.h"

#include "ResidualPlots.h"

class RecoMuonTrackPlots:public PlotBase {
    public:
      RecoMuonTrackPlots(PlotBase* pParent, std::string sDir);
      
      // Reco only information
      Trk::ParamPlots                 m_oAllPlots;
      Trk::ImpactPlots                m_oImpactPlots;
      Trk::RecoInfoPlots              m_oTrkRecoInfoPlots;
      Trk::MSHitPlots                 m_oMSHitPlots; 

      Muon::ResidualPlots             m_oResidualPlots;

      //fill methods
      void fill(const xAOD::TrackParticle& muTP);
      void fill(const Trk::ResidualPull& resPull, int stationPhi, Muon::MuonStationIndex::TechnologyIndex techid, bool measuresPhi);

};

#endif
