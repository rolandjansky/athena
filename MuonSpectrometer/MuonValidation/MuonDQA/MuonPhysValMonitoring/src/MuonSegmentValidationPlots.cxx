/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "MuonSegmentValidationPlots.h"

MuonSegmentValidationPlots::MuonSegmentValidationPlots(PlotBase* pParent, const std::string& sDir, bool isData) :
    PlotBase(pParent, sDir),
    m_oRecoSegmPlots(nullptr),
    m_oMatchedMuonSegmPlots(nullptr),
    m_oMatchedMuonSegmDiffPlots(nullptr),
    m_oTruthMuonSegmPlots(nullptr) {
    m_oRecoSegmPlots = new Muon::MuonSegmentPlots(this, "reco/MuonSegments/");
    if (!isData) {
        m_oMatchedMuonSegmPlots = new Muon::MuonSegmentPlots(this, "matched/MuonSegments/");
        m_oMatchedMuonSegmDiffPlots = new Muon::MuonSegmentTruthRelatedPlots(this, "matched/MuonSegments/");
        m_oTruthMuonSegmPlots = new Muon::MuonSegmentPlots(this, "truth/MuonSegments/");
    }
}

MuonSegmentValidationPlots::~MuonSegmentValidationPlots() {
    delete m_oRecoSegmPlots;
    if (m_oMatchedMuonSegmPlots) delete m_oMatchedMuonSegmPlots;
    if (m_oMatchedMuonSegmDiffPlots) delete m_oMatchedMuonSegmDiffPlots;
    if (m_oTruthMuonSegmPlots) delete m_oTruthMuonSegmPlots;
}

void MuonSegmentValidationPlots::fill(const xAOD::MuonSegment* truthMuSeg, const xAOD::MuonSegment* muSeg, float weight) {
    if (truthMuSeg) m_oTruthMuonSegmPlots->fill(*truthMuSeg, weight);
    if (muSeg) m_oRecoSegmPlots->fill(*muSeg, weight);

    if ((truthMuSeg) && (muSeg)) {
        m_oMatchedMuonSegmPlots->fill(*truthMuSeg, weight);
        m_oMatchedMuonSegmDiffPlots->fill(*truthMuSeg, *muSeg, weight);
    }
}

void MuonSegmentValidationPlots::fill(const xAOD::MuonSegment* muSeg, float weight) { m_oRecoSegmPlots->fill(*muSeg, weight); }
