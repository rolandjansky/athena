/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MUONPHYSVALMONITORING_MUONSEGMENTVALIDATIONPLOTS_H
#define MUONPHYSVALMONITORING_MUONSEGMENTVALIDATIONPLOTS_H

#include "MuonHistUtils/MuonSegmentPlots.h"
#include "MuonHistUtils/MuonSegmentResolutionPlots.h"

class MuonSegmentValidationPlots: public PlotBase {
  public:
    MuonSegmentValidationPlots(PlotBase* pParent, std::string sDir);

    // Reco only information
    Muon::MuonSegmentPlots                 m_oRecoSegmPlots;
    // Truth related plots
    Muon::MuonSegmentPlots                 m_oMatchedMuonSegmPlots;
    Muon::MuonSegmentResolutionPlots       m_oMuonSegmResolutionPlots;
    Muon::MuonSegmentPlots                 m_oTruthMuonSegmPlots;

    void fill(const xAOD::MuonSegment* truthMuSeg, const xAOD::MuonSegment* muSeg);
    void fill(const xAOD::MuonSegment* muSeg);
};

#endif
