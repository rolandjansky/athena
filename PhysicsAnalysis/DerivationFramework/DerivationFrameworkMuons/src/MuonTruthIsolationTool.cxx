/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

/////////////////////////////////////////////////////////////////
// MuonTruthIsolationTool.cxx, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////
// Runs on muons without a truth particle link. 
// Finds the nearest stable truth particle and adds its info to the muon.
#include "DerivationFrameworkMuons/MuonTruthIsolationTool.h"
#include <vector>
#include <string>
#include <memory>
#include "xAODMuon/MuonContainer.h"
#include "xAODTruth/TruthEventContainer.h"

// Constructor
DerivationFramework::MuonTruthIsolationTool::MuonTruthIsolationTool(const std::string& t,
							    const std::string& n,
							    const IInterface* p):
  AthAlgTool(t, n, p)
{
  declareInterface<DerivationFramework::IAugmentationTool>(this);
  declareProperty("ContainerKey", m_partSGKey="");
  declareProperty("TruthContainerKey", m_truthSGKey="TruthEvents");
}
  
// Destructor
DerivationFramework::MuonTruthIsolationTool::~MuonTruthIsolationTool() {
}  

// Athena initialize and finalize
StatusCode DerivationFramework::MuonTruthIsolationTool::initialize()
{
  ATH_MSG_VERBOSE("initialize() ...");
  return StatusCode::SUCCESS;
}

StatusCode DerivationFramework::MuonTruthIsolationTool::finalize()
{
  ATH_MSG_VERBOSE("finalize() ...");
  return StatusCode::SUCCESS;
}

StatusCode DerivationFramework::MuonTruthIsolationTool::addBranches() const
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

  // Set up the decorator
  SG::AuxElement::Decorator<float> decorator_topoetcone20("topoetcone20_truth"); 
  SG::AuxElement::Decorator<float> decorator_ptcone20("ptcone20_truth"); 
  SG::AuxElement::Decorator<float> decorator_ptvarcone20("ptvarcone20_truth"); 
  SG::AuxElement::Decorator<float> decorator_ptvarcone30("ptvarcone30_truth"); 
  SG::AuxElement::Decorator<float> decorator_ptcone20_pt500("ptcone20_pt500_truth"); 
  SG::AuxElement::Decorator<float> decorator_ptvarcone20_pt500("ptvarcone20_pt500_truth"); 
  SG::AuxElement::Decorator<float> decorator_ptvarcone30_pt500("ptvarcone30_pt500_truth"); 

  static SG::AuxElement::ConstAccessor<ElementLink<xAOD::TruthParticleContainer> > acc_tpl("truthParticleLink") ;


  for (auto part : *parts) {
    float new_topoetcone20 = 0;
    float new_ptcone20 = 0;
    float new_ptvarcone20 = 0;
    float new_ptvarcone30 = 0;
    float new_ptcone20_pt500 = 0;
    float new_ptvarcone20_pt500 = 0;
    float new_ptvarcone30_pt500 = 0;
    float varradius20 = std::min(10e3/part->pt(),0.2);
    float varradius30 = std::min(10e3/part->pt(),0.3);
    const xAOD::TruthParticle *truthLink = (acc_tpl(*part).isValid()?*acc_tpl(*part):nullptr);
    for (auto event : *tec) {
      for (size_t parti = 0; parti < event->nTruthParticles(); parti++) {
        const xAOD::TruthParticle *tpart = event->truthParticle(parti);
        if (!tpart) continue;
        if (tpart==truthLink) continue;
        if (truthLink && truthLink->barcode()==tpart->barcode()) continue;
        if (tpart->status()!=1) continue;
        if (tpart->barcode()>2e5) continue;
        if (tpart->p4().DeltaR(part->p4())<0.2 && tpart->p4().DeltaR(part->p4())>0.05) new_topoetcone20 += tpart->pt();
        if (!tpart->charge()) continue;
        if (fabs(tpart->eta())>2.5) continue;
        if (tpart->pt()<500) continue;
        if (tpart->p4().DeltaR(part->p4())<0.2) new_ptcone20_pt500 += tpart->pt();
        if (tpart->p4().DeltaR(part->p4())<varradius20) new_ptvarcone20_pt500 += tpart->pt();
        if (tpart->p4().DeltaR(part->p4())<varradius30) new_ptvarcone30_pt500 += tpart->pt();
        if (tpart->pt()<1000) continue;
        if (tpart->p4().DeltaR(part->p4())<0.2) new_ptcone20 += tpart->pt();
        if (tpart->p4().DeltaR(part->p4())<varradius20) new_ptvarcone20 += tpart->pt();
        if (tpart->p4().DeltaR(part->p4())<varradius30) new_ptvarcone30 += tpart->pt();
      }
    }

    decorator_topoetcone20(*part) = new_topoetcone20;
    decorator_ptcone20(*part) = new_ptcone20;
    decorator_ptvarcone20(*part) = new_ptvarcone20;
    decorator_ptvarcone30(*part) = new_ptvarcone30;
    decorator_ptcone20_pt500(*part) = new_ptcone20_pt500;
    decorator_ptvarcone20_pt500(*part) = new_ptvarcone20_pt500;
    decorator_ptvarcone30_pt500(*part) = new_ptvarcone30_pt500;
  }
  return StatusCode::SUCCESS;
}
