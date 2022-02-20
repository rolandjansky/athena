/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "MuonSegmentValidationPlots.h"

MuonSegmentValidationPlots::MuonSegmentValidationPlots(PlotBase* pParent, const std::string& sDir, bool isData) :
    PlotBase(pParent, sDir) {
    m_oRecoSegmPlots = std::make_unique<Muon::MuonSegmentPlots>(this, "reco/MuonSegments/");
    if (!isData) {
        m_oMatchedMuonSegmPlots = std::make_unique<Muon::MuonSegmentPlots>(this, "matched/MuonSegments/");
        m_oMatchedMuonSegmDiffPlots = std::make_unique<Muon::MuonSegmentTruthRelatedPlots>(this, "matched/MuonSegments/");
        m_oTruthMuonSegmPlots = std::make_unique<Muon::MuonSegmentPlots>(this, "truth/MuonSegments/");
    }
}

MuonSegmentValidationPlots::~MuonSegmentValidationPlots() = default;

void MuonSegmentValidationPlots::fill(const xAOD::MuonSegment* truthMuSeg, const xAOD::MuonSegment* muSeg, float weight) {
    if (truthMuSeg) m_oTruthMuonSegmPlots->fill(*truthMuSeg, weight);
    if (muSeg) m_oRecoSegmPlots->fill(*muSeg, weight);

    if ((truthMuSeg) && (muSeg)) {
        m_oMatchedMuonSegmPlots->fill(*truthMuSeg, weight);
        m_oMatchedMuonSegmDiffPlots->fill(*truthMuSeg, *muSeg, weight);
    }
}

void MuonSegmentValidationPlots::fill(const xAOD::MuonSegment* muSeg, float weight) { m_oRecoSegmPlots->fill(*muSeg, weight); }
