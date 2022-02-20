/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MUONPHYSVALMONITORING_MUONTRACKVALIDATIONPLOTS_H
#define MUONPHYSVALMONITORING_MUONTRACKVALIDATIONPLOTS_H

#include "MuonHistUtils/RecoMuonTrackPlotOrganizer.h"
#include "MuonHistUtils/TruthRelatedMuonPlotOrganizer.h"
#include "TrkValHistUtils/PlotBase.h"
#include "xAODTracking/TrackParticle.h"
#include "xAODTruth/TruthParticle.h"

class MuonTrackValidationPlots : public PlotBase {
public:
    MuonTrackValidationPlots(PlotBase* pParent, const std::string& sDir, const std::string& sTrackType, bool isData = false);
    ~MuonTrackValidationPlots();

    // Reco only information
    std::unique_ptr<Muon::RecoMuonTrackPlotOrganizer> m_oRecoMuonTrackPlots{};
    // Truth related plots
    std::unique_ptr<Muon::TruthRelatedMuonPlotOrganizer> m_oMatchedMuonTrackPlots{};

    void fill(const xAOD::TrackParticle& muTP, float weight = 1.0);
    void fill(const xAOD::TruthParticle* truthMu, const xAOD::TrackParticle* muTP, float weight = 1.0);

private:
    std::string m_sTrackType;
};

#endif
