/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TruthMuonPlots.h"

TruthMuonPlots::TruthMuonPlots(PlotBase* pParent, std::string sDir):PlotBase(pParent, sDir),
// truth related information
m_oTruthPlots(this, "/", "Truth Muons"),
m_oTruthInfoPlots(this,"/"),
m_oTruthTrkExtrapolationPlots(this, "/"),
m_oTruthMSHitPlots(this, "/")
{}	

void TruthMuonPlots::fill(const xAOD::TruthParticle& truthMu){
  //General Truth Plots
  m_oTruthPlots.fill(truthMu);
  m_oTruthInfoPlots.fill(truthMu);
  m_oTruthTrkExtrapolationPlots.fill(truthMu);
  m_oTruthMSHitPlots.fill( truthMu );
}
