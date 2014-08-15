/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TruthRelatedMuonTrackPlots.h"

TruthRelatedMuonTrackPlots::TruthRelatedMuonTrackPlots(PlotBase* pParent, std::string sDir):PlotBase(pParent, sDir),
// truth related information
m_oMatchedPlots(this, "/", "Matched Muons"),
m_oMSHitDiffPlots(this,"/"){}	

void TruthRelatedMuonTrackPlots::fill(const xAOD::TruthParticle& truthMu, const xAOD::TrackParticle& muTP){  
  m_oMatchedPlots.fill( truthMu );
  m_oMSHitDiffPlots.fill( muTP, truthMu );
}
