/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "MuonSegmentValidationPlots.h"

MuonSegmentValidationPlots::MuonSegmentValidationPlots(PlotBase* pParent, std::string sDir):PlotBase(pParent, sDir),
m_oRecoSegmPlots(this, "reco/MuonSegments/"),
m_oMatchedMuonSegmPlots(this, "matched/MuonSegments/"),
m_oMuonSegmResolutionPlots(this, "matched/MuonSegments/"),
m_oTruthMuonSegmPlots(this, "truth/MuonSegments/"){}

void MuonSegmentValidationPlots::fill(const xAOD::MuonSegment* truthMuSeg, const xAOD::MuonSegment* muSeg)
{
  if (truthMuSeg) 
    m_oTruthMuonSegmPlots.fill( *truthMuSeg );
 
  if ( (truthMuSeg) && (muSeg) ) {
    m_oMatchedMuonSegmPlots.fill( *truthMuSeg );
    m_oMuonSegmResolutionPlots.fill( *truthMuSeg, *muSeg );
  }
}

void MuonSegmentValidationPlots::fill(const xAOD::MuonSegment* muSeg)
{
  m_oRecoSegmPlots.fill(*muSeg);
}
