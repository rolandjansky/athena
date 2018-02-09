/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

#include "DiTauRec/HadMuBuilder.h"
#include "xAODTau/DiTauJetContainer.h"
#include "xAODTau/DiTauJetAuxContainer.h"
#include "xAODMuon/MuonContainer.h"
#include "xAODTau/TauJetContainer.h"

using namespace DiTauRec;

HadMuBuilder::HadMuBuilder( const std::string& name ) :
  AsgTool(name),
  m_sDiTauHadMuContainerName("DiTauJetsHadMu"),
  m_sDiTauHadMuAuxContainerName("DiTauJetsHadMuAux."),
  m_sMuonContainerName("Muons"),
  m_sTauContainerName("TauJets")
{
  declareProperty("DiTauHadMuContainer", m_sDiTauHadMuContainerName);
  declareProperty("DiTauHadMuAuxContainer", m_sDiTauHadMuAuxContainerName);
  declareProperty("MuonContainer", m_sMuonContainerName);
  declareProperty("TauContainer", m_sTauContainerName);
}

HadMuBuilder::~HadMuBuilder()
{
  
}

StatusCode HadMuBuilder::initialize()
{
  return StatusCode::SUCCESS;
}

StatusCode HadMuBuilder::execute()
{
  xAOD::DiTauJetContainer* xDiTauJetContainerHadMu        = new xAOD::DiTauJetContainer();
  xAOD::DiTauJetAuxContainer* xDiTauJetAuxContainerHadMu  = new xAOD::DiTauJetAuxContainer();
  xDiTauJetContainerHadMu->setStore(xDiTauJetAuxContainerHadMu);
  
  ANA_CHECK(evtStore()->record(xDiTauJetContainerHadMu, m_sDiTauHadMuContainerName));
  ANA_CHECK(evtStore()->record(xDiTauJetAuxContainerHadMu, m_sDiTauHadMuAuxContainerName));
    
  const xAOD::MuonContainer* muonContainer = evtStore()->retrieve<const xAOD::MuonContainer>(m_sMuonContainerName);
  const xAOD::TauJetContainer* tauContainer = evtStore()->retrieve<const xAOD::TauJetContainer>(m_sTauContainerName);

  static SG::AuxElement::Accessor<int> acc_n_subjets ("n_subjets");
  static SG::AuxElement::Accessor<ElementLink<xAOD::MuonContainer>> acc_muonLink("muonLink");
  static SG::AuxElement::Accessor<ElementLink<xAOD::TauJetContainer>> acc_tauLink("tauLink");

  static SG::AuxElement::Accessor<char> acc_isHadHad ("isHadHad");
  static SG::AuxElement::Accessor<char> acc_isHadMu  ("isHadMu" );
  static SG::AuxElement::Accessor<char> acc_isHadEl  ("isHadEl" );

  static SG::AuxElement::Accessor<double> acc_muon_pt  ("muon_pt"  );
  static SG::AuxElement::Accessor<double> acc_muon_eta ("muon_eta" );
  static SG::AuxElement::Accessor<double> acc_muon_phi ("muon_phi" );
  static SG::AuxElement::Accessor<double> acc_muon_E   ("muon_E"   );
					              
  static SG::AuxElement::Accessor<double> acc_tau_pt   ("tau_pt"  );
  static SG::AuxElement::Accessor<double> acc_tau_eta  ("tau_eta" );
  static SG::AuxElement::Accessor<double> acc_tau_phi  ("tau_phi" );
  static SG::AuxElement::Accessor<double> acc_tau_E    ("tau_E"   );

  static SG::AuxElement::Accessor<double> acc_DeltaR   ("DeltaR" );
  
  for(auto muon : *muonContainer){
    TLorentzVector p4_muon = muon->p4();
    for(auto tau : *tauContainer){
      TLorentzVector p4_tau = tau->p4();
      if(p4_muon.DeltaR(p4_tau) < 1.0){
	xAOD::DiTauJet* hadMuDiTau = new xAOD::DiTauJet();
	xDiTauJetContainerHadMu->push_back(hadMuDiTau);
	acc_n_subjets(*hadMuDiTau) = 1;
	TLorentzVector p4_ditau = p4_muon + p4_tau;
	hadMuDiTau->setP4(p4_ditau.Pt(), p4_ditau.Eta(), p4_ditau.Phi(), p4_ditau.M());
	hadMuDiTau->setSubjetPtEtaPhiE(0, p4_tau.Pt(), p4_tau.Eta(), p4_tau.Phi(), p4_tau.E());
	
	ElementLink<xAOD::TauJetContainer> tauLink(tau, *tauContainer);
	ElementLink<xAOD::MuonContainer> muonLink(muon, *muonContainer);
	acc_muonLink(*hadMuDiTau) = muonLink;
	acc_tauLink(*hadMuDiTau) = tauLink;

	acc_isHadHad(*hadMuDiTau) = false;
	acc_isHadMu (*hadMuDiTau) = true;
	acc_isHadEl (*hadMuDiTau) = false;

	acc_muon_pt (*hadMuDiTau) = p4_muon.Pt();
	acc_muon_eta(*hadMuDiTau) = p4_muon.Eta();
	acc_muon_phi(*hadMuDiTau) = p4_muon.Phi();
	acc_muon_E  (*hadMuDiTau) = p4_muon.E(); 
	             
	acc_tau_pt  (*hadMuDiTau) = p4_tau.Pt();
	acc_tau_eta (*hadMuDiTau) = p4_tau.Eta();
	acc_tau_phi (*hadMuDiTau) = p4_tau.Phi();
	acc_tau_E   (*hadMuDiTau) = p4_tau.E();

	acc_DeltaR  (*hadMuDiTau) = p4_tau.DeltaR(p4_muon);
      }
    }
  }
  
  return StatusCode::SUCCESS;
}
