/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

/////////////////////////////////////////////////////////////////
// MuonTruthClassifierFallback.cxx, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////
// Runs on muons without a truth particle link. 
// Finds the nearest stable truth particle and adds its info to the muon.
#include "DerivationFrameworkMuons/MuonTruthClassifierFallback.h"
#include <vector>
#include <string>
#include <memory>
#include "xAODMuon/MuonContainer.h"
#include "xAODTruth/TruthEventContainer.h"
#include "xAODTruth/TruthPileupEventContainer.h"
#include "MCTruthClassifier/IMCTruthClassifier.h"

// Constructor
DerivationFramework::MuonTruthClassifierFallback::MuonTruthClassifierFallback(const std::string& t,
							    const std::string& n,
							    const IInterface* p):
  AthAlgTool(t, n, p)
{
  declareInterface<DerivationFramework::IAugmentationTool>(this);
  declareProperty("MuonContainerKey", m_muonSGKey="Muons");
  declareProperty("TruthContainerKey", m_truthSGKey="TruthEvents");
  declareProperty("TruthPileupContainerKey", m_truthPileupSGKey="TruthPileupEvents");
  declareProperty("TruthMuonContainerKey", m_truthMuonSGKey="MuonTruthParticles");
  declareProperty("MinPt", m_minPt=2.5e3);
  declareProperty("MCTruthClassifierTool", m_mcTruthClassifier,"Handle of MCTruthClassifier");

}
  
// Destructor
DerivationFramework::MuonTruthClassifierFallback::~MuonTruthClassifierFallback() {
}  

// Athena initialize and finalize
StatusCode DerivationFramework::MuonTruthClassifierFallback::initialize()
{
  ATH_MSG_VERBOSE("initialize() ...");
  CHECK( m_mcTruthClassifier.retrieve() );
  return StatusCode::SUCCESS;
}

StatusCode DerivationFramework::MuonTruthClassifierFallback::finalize()
{
  ATH_MSG_VERBOSE("finalize() ...");
  return StatusCode::SUCCESS;
}

StatusCode DerivationFramework::MuonTruthClassifierFallback::addBranches() const
{
 
  // Retrieve main muon collection
  const xAOD::MuonContainer* muons = nullptr;
  if (evtStore()->retrieve(muons,m_muonSGKey).isFailure()) {
    ATH_MSG_ERROR("No Muon collection with name " << m_muonSGKey << " found in StoreGate!");
    return StatusCode::FAILURE;
  }
  const xAOD::TruthEventContainer* tec = nullptr;
  if (evtStore()->retrieve(tec,m_truthSGKey).isFailure()) {
    ATH_MSG_ERROR("No truth collection with name " << m_truthSGKey << " found in StoreGate!");
    return StatusCode::FAILURE;
  }
  const xAOD::TruthPileupEventContainer* tpec = nullptr;
  if (evtStore()->contains<xAOD::TruthPileupEventContainer>(m_truthPileupSGKey)) {
    if (evtStore()->retrieve(tpec,m_truthPileupSGKey).isFailure()) {
      ATH_MSG_ERROR("No truth pileup collection with name " << m_truthPileupSGKey << " found in StoreGate!");
      return StatusCode::FAILURE;
    }
  }
  const xAOD::TruthParticleContainer* truthMuons = nullptr;
  if (evtStore()->retrieve(truthMuons,m_truthMuonSGKey).isFailure()) {
    ATH_MSG_ERROR("No truth muon collection with name " << m_truthMuonSGKey << " found in StoreGate!");
    return StatusCode::FAILURE;
  }

  // Set up the decorator
  SG::AuxElement::Decorator<float> decorator_dR("TruthClassifierFallback_dR"); 
  // There are some cases where reco->truth matching fails (either prompt or nonprompt), in which case we can store the precomputed classification of the truth muon.
  SG::AuxElement::Decorator<int> decorator_type("TruthClassifierFallback_truthType"); 
  SG::AuxElement::Decorator<int> decorator_origin("TruthClassifierFallback_truthOrigin"); 

  SG::AuxElement::Decorator<float> decorator_pu_dR("PileupTruthClassifierFallback_dR"); 
  // There are some cases where reco->truth matching fails (either prompt or nonprompt), in which case we can store the precomputed classification of the truth muon.
  SG::AuxElement::Decorator<int> decorator_pu_type("PileupTruthClassifierFallback_truthType"); 
  SG::AuxElement::Decorator<int> decorator_pu_origin("PileupTruthClassifierFallback_truthOrigin"); 

  static SG::AuxElement::Accessor<int> acc_tT("truthType") ;
  static SG::AuxElement::Accessor<int> acc_tO("truthOrigin") ;
  static SG::AuxElement::Decorator<int> decorator_tT("truthType") ;
  static SG::AuxElement::Decorator<int> decorator_tO("truthOrigin") ;


  for (auto mu : *muons) {
    const xAOD::TruthParticle *closest = nullptr;

    for (auto muTruth : *truthMuons) {
      if (closest && closest->p4().DeltaR(mu->p4()) < muTruth->p4().DeltaR(mu->p4())) continue;
      closest = muTruth;
    }

    for (auto event : *tec) {
      for (size_t parti = 0; parti < event->nTruthParticles(); parti++) {
        const xAOD::TruthParticle *part = event->truthParticle(parti);
        if (!part) continue;
        if (part->status()!=1) continue;
        if (part->barcode()>2e5) continue;
        if (!part->charge()) continue;
        if (abs(part->pdgId())==13) continue;
        if (part->pt()<m_minPt) continue;
        if (closest && closest->p4().DeltaR(mu->p4()) < part->p4().DeltaR(mu->p4())) continue;
        closest = part;
      }
    }

    decorator_dR(*mu) = (closest ? closest->p4().DeltaR(mu->p4()) : -1);

    int newType = -1;
    int newOrigin = -1;
    if (closest && abs(closest->pdgId())==13) {
      newType = acc_tT(*closest);
      newOrigin = acc_tO(*closest);
    } else if (closest) {
      auto res = m_mcTruthClassifier->particleTruthClassifier(closest);
      newType = res.first;
      newOrigin = res.second;
    }
    decorator_type(*mu) = newType;
    decorator_origin(*mu) = newOrigin;

    if (tpec) {
      const xAOD::TruthParticle *closestPileup = nullptr;
      for (auto event : *tpec) {
        for (size_t parti = 0; parti < event->nTruthParticles(); parti++) {
          const xAOD::TruthParticle *part = event->truthParticle(parti);
          if (!part) continue;
          if (part->status()!=1) continue;
          if (part->barcode()>2e5) continue;
          if (!part->charge()) continue;
          if (part->pt()<m_minPt) continue;
          if (closestPileup && closestPileup->p4().DeltaR(mu->p4()) < part->p4().DeltaR(mu->p4())) continue;
          closestPileup = part;
        }
      }
      decorator_pu_dR(*mu) = (closestPileup ? closestPileup->p4().DeltaR(mu->p4()) : -1);
      int newPileupType = -1;
      int newPileupOrigin = -1;
      if (closestPileup) {
        auto res = m_mcTruthClassifier->particleTruthClassifier(closestPileup);
        newPileupType = res.first;
        newPileupOrigin = res.second;
      }
      decorator_pu_type(*mu) = newPileupType;
      decorator_pu_origin(*mu) = newPileupOrigin;
    }
  }

  return StatusCode::SUCCESS;
}

