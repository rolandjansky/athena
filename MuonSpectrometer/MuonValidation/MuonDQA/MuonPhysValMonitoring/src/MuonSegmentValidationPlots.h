/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MUONPHYSVALMONITORING_MUONSEGMENTVALIDATIONPLOTS_H
#define MUONPHYSVALMONITORING_MUONSEGMENTVALIDATIONPLOTS_H

#include "MuonHistUtils/MuonSegmentPlots.h"
#include "MuonHistUtils/MuonSegmentTruthRelatedPlots.h"

class MuonSegmentValidationPlots : public PlotBase {
public:
    MuonSegmentValidationPlots(PlotBase* pParent, const std::string& sDir, bool isData);
    ~MuonSegmentValidationPlots();

    // Reco only information
    Muon::MuonSegmentPlots* m_oRecoSegmPlots;
    // Truth related plots
    Muon::MuonSegmentPlots* m_oMatchedMuonSegmPlots;
    Muon::MuonSegmentTruthRelatedPlots* m_oMatchedMuonSegmDiffPlots;
    Muon::MuonSegmentPlots* m_oTruthMuonSegmPlots;

    void fill(const xAOD::MuonSegment* truthMuSeg, const xAOD::MuonSegment* muSeg, float weight = 1.0);
    void fill(const xAOD::MuonSegment* muSeg, float weight = 1.0);
};

#endif
