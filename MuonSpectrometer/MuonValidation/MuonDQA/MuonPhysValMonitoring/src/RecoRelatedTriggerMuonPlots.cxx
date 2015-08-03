/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "RecoRelatedTriggerMuonPlots.h"

RecoRelatedTriggerMuonPlots::RecoRelatedTriggerMuonPlots(PlotBase* pParent, std::string sDir,bool doBinnedResolutionPlots, bool doMuonTree):
    PlotBase(pParent, sDir),
    // truth related information
    m_doMuonTree(doMuonTree),
    m_oMuonResolutionPlots(this, "/Resolution/","",doBinnedResolutionPlots)
{}


RecoRelatedTriggerMuonPlots::~RecoRelatedTriggerMuonPlots() {}

void RecoRelatedTriggerMuonPlots::fill(const xAOD::Muon& /*Trigmu*/, const xAOD::Muon& /*Recomu*/){

  //  m_oMuonResolutionPlots.fill(Trigmu, Recomu);   ////work in progress
}
