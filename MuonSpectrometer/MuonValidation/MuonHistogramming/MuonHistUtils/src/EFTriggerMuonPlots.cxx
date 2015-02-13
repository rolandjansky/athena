/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "MuonHistUtils/EFTriggerMuonPlots.h"

EFTriggerMuonPlots::EFTriggerMuonPlots(PlotBase* pParent, std::string sDir):PlotBase(pParent, sDir),
m_oHLTriggerPlots(this, "/", "EFTrigger Muons")
{}	

void EFTriggerMuonPlots::fill(const xAOD::Muon& /*mu*/,const xAOD::Muon& Trigmu){
  //General EF Plots
  m_oHLTriggerPlots.fill(Trigmu);
}


