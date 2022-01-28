/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "MuonHistUtils/HLTriggerMuonPlots.h"
#include "xAODMuon/Muon.h"
#include "xAODMuon/MuonContainer.h"
#include "xAODMuon/MuonAuxContainer.h"

HLTriggerMuonPlots::HLTriggerMuonPlots(PlotBase* pParent, const std::string& sDir):PlotBase(pParent, sDir),
m_oHLTriggerPlots(this, "/", "HLT Muons")
{}	

void HLTriggerMuonPlots::fill(const xAOD::Muon& Trigmu){
  //General HL Plots
  m_oHLTriggerPlots.fill(Trigmu);
}

void HLTriggerMuonPlots::fill(const xAOD::Muon& /*mu*/,const xAOD::Muon& Trigmu){
  //General HL Plots
  m_oHLTriggerPlots.fill(Trigmu);
}

void HLTriggerMuonPlots::fill(const xAOD::L2StandAloneMuon& L2SAmu){
  //General HL Plots
  xAOD::MuonContainer* L2SAinMuons = new xAOD::MuonContainer;
  xAOD::MuonAuxContainer* L2SAinMuonsAux = new xAOD::MuonAuxContainer;
  L2SAinMuons->setStore( L2SAinMuonsAux );
  xAOD::Muon* myL2SA=new xAOD::Muon();
  L2SAinMuons->push_back(myL2SA);
  if (L2SAmu.pt()>0.) myL2SA->setP4(L2SAmu.pt()*1000.,L2SAmu.eta(),L2SAmu.phi());
  if (L2SAmu.pt()<0.) myL2SA->setP4(L2SAmu.pt()*(-1000.),L2SAmu.eta(),L2SAmu.phi());
 
  m_oHLTriggerPlots.fill(*myL2SA);
  delete L2SAinMuons;
  delete L2SAinMuonsAux;


}

void HLTriggerMuonPlots::fill(const xAOD::L2CombinedMuon& L2CBmu){
  //General HL Plots
  m_oHLTriggerPlots.fill(L2CBmu);
}

void HLTriggerMuonPlots::fill(const xAOD::L2IsoMuon& L2Isomu){
  //General HL Plots
  m_oHLTriggerPlots.fill(L2Isomu);
}
