/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "MuonHistUtils/L1TriggerMuonPlots.h"
#include "xAODMuon/Muon.h"
#include "xAODMuon/MuonContainer.h"
#include "xAODMuon/MuonAuxContainer.h"

L1TriggerMuonPlots::L1TriggerMuonPlots(PlotBase* pParent, const std::string& sDir):PlotBase(pParent, sDir),
// trigger related information
m_oL1TriggerPlots(this, "/", "L1Trigger Muons")
{}	


void L1TriggerMuonPlots::fill(const xAOD::MuonRoI& TrigL1mu){
  //General L1 Plots
  xAOD::MuonContainer* ROIinMuons = new xAOD::MuonContainer;
  xAOD::MuonAuxContainer* ROIinMuonsAux = new xAOD::MuonAuxContainer;
  ROIinMuons->setStore( ROIinMuonsAux );
  xAOD::Muon* myROI=new xAOD::Muon();
  ROIinMuons->push_back(myROI);
  myROI->setP4(TrigL1mu.thrValue(),TrigL1mu.eta(),TrigL1mu.phi());

  m_oL1TriggerPlots.fill(*myROI);
  delete ROIinMuons;
  delete ROIinMuonsAux;
}
