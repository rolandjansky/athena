/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "MuonSegmentValidationPlots.h"

MuonSegmentValidationPlots::MuonSegmentValidationPlots(PlotBase* pParent, std::string sDir, bool isData):
  PlotBase(pParent, sDir), m_oRecoSegmPlots(NULL), m_oMatchedMuonSegmPlots(NULL), m_oMatchedMuonSegmDiffPlots(NULL), m_oTruthMuonSegmPlots(NULL)
{
  m_oRecoSegmPlots = new Muon::MuonSegmentPlots(this, "reco/MuonSegments/");
  if (!isData) {
    m_oMatchedMuonSegmPlots = new Muon::MuonSegmentPlots(this, "matched/MuonSegments/");
    m_oMatchedMuonSegmDiffPlots = new Muon::MuonSegmentTruthRelatedPlots(this, "matched/MuonSegments/");
    m_oTruthMuonSegmPlots = new Muon::MuonSegmentPlots(this, "truth/MuonSegments/");
  }
}

MuonSegmentValidationPlots::~MuonSegmentValidationPlots()
{
  delete m_oRecoSegmPlots;
  if (m_oMatchedMuonSegmPlots) delete m_oMatchedMuonSegmPlots;
  if (m_oMatchedMuonSegmDiffPlots) delete m_oMatchedMuonSegmDiffPlots;
  if (m_oTruthMuonSegmPlots) delete m_oTruthMuonSegmPlots;  
}

void MuonSegmentValidationPlots::fill(const xAOD::MuonSegment* truthMuSeg, const xAOD::MuonSegment* muSeg)
{
  if (truthMuSeg) 
    m_oTruthMuonSegmPlots->fill( *truthMuSeg );
  if (muSeg)
    m_oRecoSegmPlots->fill(*muSeg);
  
  if ( (truthMuSeg) && (muSeg) ) {    
    m_oMatchedMuonSegmPlots->fill( *truthMuSeg );
    m_oMatchedMuonSegmDiffPlots->fill( *truthMuSeg, *muSeg );
  }
}

void MuonSegmentValidationPlots::fill(const xAOD::MuonSegment* muSeg)
{
  m_oRecoSegmPlots->fill(*muSeg);
}
