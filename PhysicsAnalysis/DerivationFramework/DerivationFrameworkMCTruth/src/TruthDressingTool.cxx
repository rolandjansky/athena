/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/////////////////////////////////////////////////////////////////
// TruthDressingTool.cxx, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////
// Author: Kevin Finelli (kevin.finelli@cern.ch)
// Create dressed (i.e. including FSR photons) 4-vectors of truth objects

#include "DerivationFrameworkMCTruth/TruthDressingTool.h"
#include "xAODTruth/TruthEventContainer.h"
#include "fastjet/PseudoJet.hh"
#include "fastjet/ClusterSequence.hh"
#include <vector>
#include <string>
#include <algorithm>

// Constructor
DerivationFramework::TruthDressingTool::TruthDressingTool(const std::string& t,
        const std::string& n,
        const IInterface* p ) :
    AthAlgTool(t,n,p)
{
    declareInterface<DerivationFramework::IAugmentationTool>(this);
    declareProperty ("particlesKey",
            m_particlesKey = "TruthParticles",
            "Name of TruthParticles key for photon list input");
    declareProperty ("dressParticlesKey",
            m_dressParticlesKey = "TruthParticles",
            "Name of TruthParticles key for input particles to be dressed");
    declareProperty ("usePhotonsFromHadrons",
            m_usePhotonsFromHadrons = false,
            "Add photons coming from hadron decays while dressing particles?");
    declareProperty ("dressingConeSize", m_coneSize = 0.1,
            "Size of dR cone in which to include FSR photons in dressing");
    declareProperty ("particleIDsToDress", m_listOfPIDs = std::vector<int>{11,13},
            "List of the pdgID's of particles to be dressed (usually 11,13)");
    declareProperty ("useAntiKt", m_useAntiKt = false,
        "use anti-k_T instead of fixed-cone dressing");

}

// Destructor
DerivationFramework::TruthDressingTool::~TruthDressingTool() {
}

// Athena initialize and finalize
StatusCode DerivationFramework::TruthDressingTool::initialize()
{
    return StatusCode::SUCCESS;
}

StatusCode DerivationFramework::TruthDressingTool::finalize()
{
    ATH_MSG_VERBOSE("finalize() TruthDressingTool");
    return StatusCode::SUCCESS;
}

// Function to do dressing, implements interface in IAugmentationTool
StatusCode DerivationFramework::TruthDressingTool::addBranches() const
{
    // Retrieve the truth collections
    const xAOD::TruthParticleContainer* importedTruthParticles(nullptr);
    if (evtStore()->retrieve(importedTruthParticles, m_particlesKey).isFailure()) {
        ATH_MSG_ERROR("No TruthParticleContainer with name " << m_particlesKey << " found in StoreGate!");
        return StatusCode::FAILURE;
    }
    const xAOD::TruthParticleContainer* importedDressTruthParticles(nullptr);
    if (evtStore()->retrieve(importedDressTruthParticles, m_dressParticlesKey).isFailure()) {
        ATH_MSG_ERROR("No TruthParticleContainer with name " << m_dressParticlesKey << " found in StoreGate!");
        return StatusCode::FAILURE;
    }
    SG::AuxElement::Decorator< float > decorator_e("e_dressed");
    SG::AuxElement::Decorator< float > decorator_pt("pt_dressed");
    SG::AuxElement::Decorator< float > decorator_eta("eta_dressed");
    SG::AuxElement::Decorator< float > decorator_phi("phi_dressed");
    SG::AuxElement::Decorator< int > decorator_nphoton("nPhotons_dressed");

    //antikt dressing
    SG::AuxElement::Decorator< float > decorator_e_akt("e_dressed_akt");
    SG::AuxElement::Decorator< float > decorator_pt_akt("pt_dressed_akt");
    SG::AuxElement::Decorator< float > decorator_eta_akt("eta_dressed_akt");
    SG::AuxElement::Decorator< float > decorator_phi_akt("phi_dressed_akt");

    //get struct of helper functions
    DerivationFramework::DecayGraphHelper decayHelper;

    // Dressing is applied to electrons and muons only
    std::vector<const xAOD::TruthParticle*> listOfParticlesToDress;
    decayHelper.constructListOfFinalParticles(importedDressTruthParticles, listOfParticlesToDress, m_listOfPIDs);

    // Vectors to store particles which will be dressed
    std::vector<const xAOD::TruthParticle*>  photonsFSRList;

    //fill the photon list
    std::vector<int> photonPID{22};
    decayHelper.constructListOfFinalParticles(importedTruthParticles, photonsFSRList, photonPID, m_usePhotonsFromHadrons);

    //build the antikt jet list
    std::vector<fastjet::PseudoJet> sorted_jets;
    std::vector<fastjet::PseudoJet> particles;
    if (m_useAntiKt) {
        for (auto pItr=listOfParticlesToDress.begin(); pItr!=listOfParticlesToDress.end(); ++pItr) {
            particles.push_back(fastjet::PseudoJet((*pItr)->px(), (*pItr)->py(), (*pItr)->pz(), (*pItr)->e()));
            particles.back().set_user_index((*pItr)->barcode());
        }
        for (auto pItr=photonsFSRList.begin(); pItr!=photonsFSRList.end(); ++pItr) {
            particles.push_back(fastjet::PseudoJet((*pItr)->px(), (*pItr)->py(), (*pItr)->pz(), (*pItr)->e()));
            particles.back().set_user_index((*pItr)->barcode());
        }
    }
    //run the clustering
    fastjet::JetAlgorithm alg=fastjet::antikt_algorithm;
    const fastjet::JetDefinition jet_def(alg, m_coneSize);
    fastjet::ClusterSequence cseq(particles, jet_def);
    sorted_jets = sorted_by_pt(cseq.inclusive_jets(0));
    // Standard particle loop over final state leptons
    for (auto pItr=listOfParticlesToDress.begin(); pItr!=listOfParticlesToDress.end(); ++pItr) {
      //copy the bare vector, then dress it and register it with SG
      xAOD::TruthParticle::FourMom_t dressedVec = (*pItr)->p4();
      int nPhot = 0;
      dressVector(*pItr, dressedVec, photonsFSRList, nPhot);
      decorator_e(**pItr)       = dressedVec.E();
      decorator_pt(**pItr)      = dressedVec.Pt();
      decorator_eta(**pItr)     = dressedVec.Eta();
      decorator_phi(**pItr)     = dressedVec.Phi();
      decorator_nphoton(**pItr) = nPhot;

      if (m_useAntiKt) {
          //loop over fastjet pseudojets and associate one with this particle by barcode
          bool found=false;
          auto pjItr=sorted_jets.begin();
          while(!found && pjItr!=sorted_jets.end()) {
              std::vector<fastjet::PseudoJet> constituents = pjItr->constituents();
              for(auto cItr=constituents.begin(); cItr!=constituents.end(); ++cItr) {
                  if ((*pItr)->barcode()==cItr->user_index()) {
                      decorator_e_akt(**pItr)       = pjItr->e();
                      decorator_pt_akt(**pItr)      = pjItr->pt();
                      decorator_eta_akt(**pItr)     = pjItr->pseudorapidity();
                      decorator_phi_akt(**pItr)     = pjItr->phi_std(); //returns phi in [-pi,pi]
                      found=true;
                      break;
                  }

              }
              ++pjItr;
          }
          if (!found) {
              decorator_e_akt(**pItr)       = 0;
              decorator_pt_akt(**pItr)      = 0;
              decorator_eta_akt(**pItr)     = 0;
              decorator_phi_akt(**pItr)     = 0;
              ATH_MSG_WARNING("Bare particle not found in constituents ");
          }
      }
    }

    return StatusCode::SUCCESS;
}

void DerivationFramework::TruthDressingTool::dressVector(const xAOD::TruthParticle* bareParticle,
        xAOD::TruthParticle::FourMom_t& dressedVec, const std::vector<const xAOD::TruthParticle*> &photonlist,
        int &nPhot) const
{
    //given a bare TruthParticle bareParticle, dress the vector dressedVec using
    //the photons in photonlist
    nPhot = 0;
    ATH_MSG_DEBUG("bare p4 E: " << dressedVec.E());

    for (auto pItr=photonlist.begin(); pItr!=photonlist.end(); ++pItr) {
        if (calculateDeltaR(*pItr, bareParticle) < m_coneSize) {
            dressedVec += (*pItr)->p4();
            ++nPhot;
        }
    }

    ATH_MSG_DEBUG("dressed E: " << dressedVec.E());
}

float DerivationFramework::TruthDressingTool::calculateDeltaR(const xAOD::IParticle *p1, const xAOD::IParticle *p2) const
{
  float phi1 = p1->phi();
  float phi2 = p2->phi();
  float eta1 = p1->eta();
  float eta2 = p2->eta();
  float deltaPhi = fabs(phi1-phi2);
  if (deltaPhi>TMath::Pi()) deltaPhi = 2.0*TMath::Pi() - deltaPhi;
  float deltaPhiSq = (phi1-phi2)*(phi1-phi2);
  float deltaEtaSq = (eta1-eta2)*(eta1-eta2);
  float deltaR = sqrt(deltaPhiSq+deltaEtaSq);
  return deltaR;
}
