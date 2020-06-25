/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/////////////////////////////////////////////////////////////////
// TruthLinkRepointTool.cxx
// Truth links on some objects point to the main truth particle
// container, or to some other container that won't be saved in the
// output derivation.  This re-points the links from the old 
// container to the new container (and serves as a chance to clean
// up / harmonize the names of the decorations).

#include "TruthLinkRepointTool.h"
#include "xAODMuon/MuonContainer.h"
#include "xAODEgamma/ElectronContainer.h"
#include "xAODEgamma/PhotonContainer.h"
#include "xAODTruth/xAODTruthHelpers.h"
#include "xAODTruth/TruthParticleContainer.h"
#include <string>
#include <vector>

// Constructor
DerivationFramework::TruthLinkRepointTool::TruthLinkRepointTool(const std::string& t,
        const std::string& n,
        const IInterface* p ) :
    AthAlgTool(t,n,p)
{
  declareInterface<DerivationFramework::IAugmentationTool>(this);

  declareProperty ("RecoCollection", m_recoKey = "Muons", "Name of reco collection for decoration");
  declareProperty ("OutputDecoration", m_decOutput = "TruthLink", "Name of the output decoration on the reco object");
  declareProperty ("TargetCollections", m_targetKeys = {"TruthMuons","TruthPhotons","TruthElectrons"}, "Name of target truth collections");
}

// Destructor
DerivationFramework::TruthLinkRepointTool::~TruthLinkRepointTool() {
}

// Function to do dressing, implements interface in IAugmentationTool
StatusCode DerivationFramework::TruthLinkRepointTool::addBranches() const
{
  // Retrieve the truth collections
  const std::vector<DataHandle<xAOD::TruthParticleContainer> > target(m_targetKeys.size(),nullptr);
  for (size_t i=0;i<m_targetKeys.size();++i) ATH_CHECK(evtStore()->retrieve(target[i], m_targetKeys[i]));

  SG::AuxElement::Decorator< ElementLink<xAOD::TruthParticleContainer> > output_decorator(m_decOutput);

  // Handle separate cases: Photons, Electrons, Muons, Jets
  if (std::string::npos!=m_recoKey.find("Electron")){
    const DataHandle<xAOD::ElectronContainer> inputCont(nullptr);
    ATH_CHECK(evtStore()->retrieve(inputCont, m_recoKey));
    for (auto input : *inputCont){
      const xAOD::TruthParticle* truthPart = xAOD::TruthHelpers::getTruthParticle(*input); 
      int index = -1;
      for (size_t i=0;i<m_targetKeys.size() && index<0 && truthPart;++i){
        index = find_match(truthPart,target[i]);
        if (index>=0) output_decorator(*input) = ElementLink<xAOD::TruthParticleContainer>(*(target[i]),index);
      }
      if (index<0){
        if (truthPart){
          ATH_MSG_DEBUG("No particle with barcode " << truthPart->barcode() << " PDG ID " << truthPart->pdgId() 
                       << " pT=" << truthPart->pt()*0.001 << " GeV and status " << truthPart->status() 
                       << " found in " << m_targetKeys.size() << " target containers");
        }
        output_decorator(*input) = ElementLink<xAOD::TruthParticleContainer>();
      }
    } // Loop over input particles
  } else if (std::string::npos!=m_recoKey.find("Photon")){
    const DataHandle<xAOD::PhotonContainer> inputCont(nullptr);
    ATH_CHECK(evtStore()->retrieve(inputCont, m_recoKey));
    for (auto input : *inputCont){
      const xAOD::TruthParticle* truthPart = xAOD::TruthHelpers::getTruthParticle(*input);
      int index = -1;
      for (size_t i=0;i<m_targetKeys.size() && index<0 && truthPart;++i){
        index = find_match(truthPart,target[i]);
        if (index>=0) output_decorator(*input) = ElementLink<xAOD::TruthParticleContainer>(*(target[i]),index);
      }
      if (index<0){
        if (truthPart){
          ATH_MSG_DEBUG("No particle with barcode " << truthPart->barcode() << " PDG ID " << truthPart->pdgId()
                       << " pT=" << truthPart->pt()*0.001 << " GeV and status " << truthPart->status()
                       << " found in " << m_targetKeys.size() << " target containers");
        }
        output_decorator(*input) = ElementLink<xAOD::TruthParticleContainer>();
      }
    } // Loop over input particles
  } else if (std::string::npos!=m_recoKey.find("Muon")){
    const DataHandle<xAOD::MuonContainer> inputCont(nullptr);
    ATH_CHECK(evtStore()->retrieve(inputCont, m_recoKey));
    for (auto input : *inputCont){
      const xAOD::TruthParticle* truthPart = xAOD::TruthHelpers::getTruthParticle(*input);
      int index = -1;
      for (size_t i=0;i<m_targetKeys.size() && index<0 && truthPart;++i){
        index = find_match(truthPart,target[i]);
        if (index>=0) output_decorator(*input) = ElementLink<xAOD::TruthParticleContainer>(*(target[i]),index);
      }
      if (index<0){
        if (truthPart){
          ATH_MSG_DEBUG("No particle with barcode " << truthPart->barcode() << " PDG ID " << truthPart->pdgId()
                       << " pT=" << truthPart->pt()*0.001 << " GeV and status " << truthPart->status()
                       << " found in " << m_targetKeys.size() << " target containers");
        }
        output_decorator(*input) = ElementLink<xAOD::TruthParticleContainer>();
      }
    } // Loop over input particles
  }

  return StatusCode::SUCCESS;
}

// Find a match by barcode in a different container
int DerivationFramework::TruthLinkRepointTool::find_match(const xAOD::TruthParticle* p, const DataHandle<xAOD::TruthParticleContainer> & c) const
{
  // See if it's already gone
  if (!p) return -1;
  // Look through the mini-collection
  for (int i=0;i<int(c->size());++i){
    if (c->at(i) && p->barcode()==c->at(i)->barcode()) return i;
  }
  // Note: just fine if it wasn't in the mini-collection
  return -1;
}
