/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

/////////////////////////////////////////////////////////////////
// MuonJetDrTool.cxx, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////
// Finds the nearest jet and adds its info to the muon.
#include "DerivationFrameworkMuons/MuonJetDrTool.h"
#include "xAODMuon/MuonContainer.h"
#include "xAODJet/JetContainer.h"
#include "FourMomUtils/xAODP4Helpers.h"

// Constructor
DerivationFramework::MuonJetDrTool::MuonJetDrTool(const std::string& t,
							    const std::string& n,
							    const IInterface* p):
  AthAlgTool(t, n, p)
{
  declareInterface<DerivationFramework::IAugmentationTool>(this);
  declareProperty("ContainerKey", m_muonSGKey="Muons");
  declareProperty("JetContainerKey", m_jetSGKey="AntiKt4EMTopoJets");
  declareProperty("JetMinPt", m_jetMinPt=20e3);
}
  
StatusCode DerivationFramework::MuonJetDrTool::addBranches() const
{
 
  // Retrieve main muonicle collection
  const xAOD::IParticleContainer* muons = nullptr;
  ATH_CHECK(evtStore()->retrieve(muons,m_muonSGKey));
  const xAOD::JetContainer* jets = nullptr;
  ATH_CHECK(evtStore()->retrieve(jets,m_jetSGKey));

  // Set up the decorator
  SG::AuxElement::Decorator<float> decorator_jetdR("DFCommonJetDr"); 

  for (auto muon : *muons) {
    float new_jetdR = -1;
    for (auto jet : *jets) {
      if (jet->pt()>m_jetMinPt && (new_jetdR<0 || xAOD::P4Helpers::deltaR(jet,muon)<new_jetdR)) new_jetdR = xAOD::P4Helpers::deltaR(jet,muon);
    }
    decorator_jetdR(*muon) = new_jetdR;
  }
  return StatusCode::SUCCESS;
}
