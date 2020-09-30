/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
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
  declareProperty("ContainerKey", m_partSGKey="");
  declareProperty("TruthContainerKey", m_truthSGKey="TruthEvents");
  declareProperty("TruthPileupContainerKey", m_truthPileupSGKey="TruthPileupEvents");
  declareProperty("TruthMuonContainerKey", m_truthMuonSGKey="MuonTruthParticles");
  declareProperty("MinPt", m_minPt=2.5e3);
  declareProperty("MaxDr", m_maxDr=0.05);
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
 
  // Retrieve main particle collection
  const xAOD::IParticleContainer* parts = nullptr;
  if (evtStore()->retrieve(parts,m_partSGKey).isFailure()) {
    ATH_MSG_ERROR("No Muon collection with name " << m_partSGKey << " found in StoreGate!");
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
  SG::AuxElement::Decorator<int> decorator_type("TruthClassifierFallback_truthType"); 
  SG::AuxElement::Decorator<int> decorator_origin("TruthClassifierFallback_truthOrigin"); 

  SG::AuxElement::Decorator<float> decorator_pu_dR("PileupTruthClassifierFallback_dR"); 
  SG::AuxElement::Decorator<int> decorator_pu_type("PileupTruthClassifierFallback_truthType"); 
  SG::AuxElement::Decorator<int> decorator_pu_origin("PileupTruthClassifierFallback_truthOrigin"); 

  static SG::AuxElement::Accessor<int> acc_tT("truthType") ;
  static SG::AuxElement::Accessor<int> acc_tO("truthOrigin") ;
  static SG::AuxElement::Decorator<int> decorator_tT("truthType") ;
  static SG::AuxElement::Decorator<int> decorator_tO("truthOrigin") ;


  for (auto part : *parts) {
    const xAOD::TruthParticle *closest = nullptr;

    for (auto muTruth : *truthMuons) {
      if (closest && closest->p4().DeltaR(part->p4()) < muTruth->p4().DeltaR(part->p4())) continue;
      closest = muTruth;
    }

    for (auto event : *tec) {
      for (size_t parti = 0; parti < event->nTruthParticles(); parti++) {
        const xAOD::TruthParticle *tpart = event->truthParticle(parti);
        if (!tpart) continue;
        if (tpart->status()!=1) continue;
        if (tpart->barcode()>2e5) continue;
        if (!tpart->charge()) continue;
        if (abs(tpart->pdgId())==13) continue;
        if (tpart->pt()<m_minPt) continue;
        if (tpart->p4().DeltaR(part->p4())>m_maxDr) continue;
        if (closest && closest->p4().DeltaR(part->p4()) < tpart->p4().DeltaR(part->p4())) continue;
        closest = tpart;
      }
    }

    decorator_dR(*part) = (closest ? closest->p4().DeltaR(part->p4()) : -1);

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
    decorator_type(*part) = newType;
    decorator_origin(*part) = newOrigin;

    if (tpec) {
      const xAOD::TruthParticle *closestPileup = nullptr;
      for (auto event : *tpec) {
        for (size_t parti = 0; parti < event->nTruthParticles(); parti++) {
          const xAOD::TruthParticle *tpart = event->truthParticle(parti);
          if (!tpart) continue;
          if (tpart->status()!=1) continue;
          if (tpart->barcode()>2e5) continue;
          if (!tpart->charge()) continue;
          if (tpart->pt()<m_minPt) continue;
          if (tpart->p4().DeltaR(part->p4())>m_maxDr) continue;
          if (closestPileup && closestPileup->p4().DeltaR(part->p4()) < tpart->p4().DeltaR(part->p4())) continue;
          closestPileup = tpart;
        }
      }
      decorator_pu_dR(*part) = (closestPileup ? closestPileup->p4().DeltaR(part->p4()) : -1);
      int newPileupType = -1;
      int newPileupOrigin = -1;
      if (closestPileup) {
        auto res = m_mcTruthClassifier->particleTruthClassifier(closestPileup);
        newPileupType = res.first;
        newPileupOrigin = res.second;
      }
      decorator_pu_type(*part) = newPileupType;
      decorator_pu_origin(*part) = newPileupOrigin;
    }
  }

  return StatusCode::SUCCESS;
}

