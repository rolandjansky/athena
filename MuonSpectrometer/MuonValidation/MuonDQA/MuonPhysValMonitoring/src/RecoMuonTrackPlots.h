/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MUONPHYSVALMONITORING_RECOMUONTRACKPLOTS_H
#define MUONPHYSVALMONITORING_RECOMUONTRACKPLOTS_H

#include "xAODTracking/TrackParticle.h"
#include "xAODTruth/TruthParticle.h"

#include "TrkValHistUtils/PlotBase.h"
#include "TrkValHistUtils/ParamPlots.h"
#include "TrkValHistUtils/MSHitPlots.h"
#include "TrkValHistUtils/RecoInfoPlots.h"
#include "TrkValHistUtils/ImpactPlots.h"

class RecoMuonTrackPlots:public PlotBase {
    public:
      RecoMuonTrackPlots(PlotBase* pParent, std::string sDir);
      
      // Reco only information
      Trk::ParamPlots                 m_oAllPlots;
      Trk::ImpactPlots                m_oImpactPlots;
      Trk::RecoInfoPlots              m_oTrkRecoInfoPlots;
      Trk::MSHitPlots                 m_oMSHitPlots; 
      
      //fill methods
      void fill(const xAOD::TrackParticle& muTP);

};

#endif
