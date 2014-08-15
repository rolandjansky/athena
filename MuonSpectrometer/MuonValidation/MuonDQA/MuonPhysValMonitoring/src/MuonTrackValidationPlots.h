/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MUONPHYSVALMONITORING_MUONTRACKVALIDATIONPLOTS_H
#define MUONPHYSVALMONITORING_MUONTRACKVALIDATIONPLOTS_H

#include "RecoMuonTrackPlots.h"
#include "TruthRelatedMuonTrackPlots.h"

#include "xAODTracking/TrackParticle.h"
#include "xAODTruth/TruthParticle.h"

class MuonTrackValidationPlots:public PlotBase {
  public:
    MuonTrackValidationPlots(PlotBase* pParent, std::string sDir);

    // Reco only information
    RecoMuonTrackPlots          m_oRecoMuonTrackPlots;
    // Truth related plots
    TruthRelatedMuonTrackPlots  m_oMatchedMuonTrackPlots;

    void fill(const xAOD::TrackParticle& muTP);
    void fill(const xAOD::TruthParticle* truthMu, const xAOD::TrackParticle* muTP);

};

#endif
