/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// ======================================================================
// DecayGraphHelper.h 
// James.Catmore@cern.ch
// Helper struct which implements two simple recursive functions; 
// descends or ascends through a decay from a head particle provided by 
// the user, and records all particles and vertices it encounters in the 
// masks
// 
// Also contains functions shared by TruthDressing tool and
// TruthIsolationTool.
// ======================================================================
#pragma once

#include "xAODTruth/TruthParticleContainer.h"
#include "xAODTruth/TruthVertexContainer.h"
#include "HepPID/ParticleIDMethods.hh"
#include<unordered_set>

namespace DerivationFramework {
    struct DecayGraphHelper {
        // Geant4 barcode offset
        int g4BarcodeOffset; 
        DecayGraphHelper(int init_g4BarcodeOffset) {
            g4BarcodeOffset = init_g4BarcodeOffset;
        }
        DecayGraphHelper(void) {
            g4BarcodeOffset = 200000;
        }
        // descendants: starting from particle (simple listing version)
        void descendants(const xAOD::TruthParticle* pHead,
                         std::vector<int> &particleList,
                         std::unordered_set<int> &encounteredBarcodes) {

            // Check that this barcode hasn't been seen before (e.g. we are in a loop)
            //if ( find(encounteredBarcodes.begin(),encounteredBarcodes.end(),pHead->barcode()) != encounteredBarcodes.end()) return;
            std::unordered_set<int>::const_iterator found = encounteredBarcodes.find(pHead->barcode());
            if (found!=encounteredBarcodes.end()) return;
            encounteredBarcodes.insert(pHead->barcode());

            // Get the decay vertex
            const xAOD::TruthVertex* decayVtx(0);
            if (pHead->hasDecayVtx()) {decayVtx = pHead->decayVtx();}
            else {return;}

            // Save the PDG ID number of the particle 
            particleList.push_back(pHead->pdgId());

            // Get children particles and self-call
            int nChildren = decayVtx->nOutgoingParticles();
            for (int i=0; i<nChildren; ++i) descendants(decayVtx->outgoingParticle(i),particleList,encounteredBarcodes);
            return;
        } 

        // descendants: starting from particle
        void descendants(const xAOD::TruthParticle* pHead,
                         std::vector<bool> &particleMask,
                         std::vector<bool> &vertexMask,
                         std::unordered_set<int> &encounteredBarcodes,
                         bool includeGeant) {
            
            // Check that this barcode hasn't been seen before (e.g. we are in a loop)
            //if ( find(encounteredBarcodes.begin(),encounteredBarcodes.end(),pHead->barcode()) != encounteredBarcodes.end()) return;
            std::unordered_set<int>::const_iterator found = encounteredBarcodes.find(pHead->barcode());
            if (found!=encounteredBarcodes.end()) return;
            encounteredBarcodes.insert(pHead->barcode());
	
            // Save the particle position in the mask
            // Note: is this safe???
            int headIndex = pHead->index();
            particleMask[headIndex] = true;
            
            // Get the decay vertex
            const xAOD::TruthVertex* decayVtx(0);
            if (pHead->hasDecayVtx()) {decayVtx = pHead->decayVtx();}
            else {return;}
            
            // If user doesn't want Geant, check and reject
            if (!includeGeant && pHead->barcode()>g4BarcodeOffset) return;  

            // Save the decay vertex
            int vtxIndex = decayVtx->index();
            vertexMask[vtxIndex] = true;
            
            // Get children particles and self-call
            int nChildren = decayVtx->nOutgoingParticles();
            for (int i=0; i<nChildren; ++i) descendants(decayVtx->outgoingParticle(i),particleMask,vertexMask,encounteredBarcodes,includeGeant);
            return;
        }

        // ancestors: starting from particle
        void ancestors(const xAOD::TruthParticle* pHead,
                       std::vector<bool> &particleMask,
                       std::vector<bool> &vertexMask,
                       std::unordered_set<int> &encounteredBarcodes ) {
            
            // Check that this barcode hasn't been seen before (e.g. we are in a loop)
            //if ( find(encounteredBarcodes.begin(),encounteredBarcodes.end(),pHead->barcode()) != encounteredBarcodes.end()) return;
            std::unordered_set<int>::const_iterator found = encounteredBarcodes.find(pHead->barcode());
            if (found!=encounteredBarcodes.end()) return;
            encounteredBarcodes.insert(pHead->barcode());

	    // Save particle position in the mask	
            int headIndex = pHead->index();
            particleMask[headIndex] = true;

	    // Get the production vertex
            const xAOD::TruthVertex* prodVtx(0);
            if (pHead->hasProdVtx()) {prodVtx = pHead->prodVtx();}
            else {return;}

            // Save the production vertex
            int vtxIndex = prodVtx->index();
            vertexMask[vtxIndex] = true;
            
            // Get children particles and self-call
            int nParents = prodVtx->nIncomingParticles();
            for (int i=0; i<nParents; ++i) ancestors(prodVtx->incomingParticle(i),particleMask,vertexMask,encounteredBarcodes);
            return;
	} 

        void constructListOfFinalParticles(const xAOD::TruthParticleContainer* allParticles,
            std::vector<const xAOD::TruthParticle*> &selectedlist,
            const std::vector<int> &pdgId, bool usePhotonsFromHadrons = false,
            bool chargedOnly = false) const
        {
          //fill the vector selectedlist with only particles with abs(ID) in the list
          //pdgID that are 'good' for dressing: status==1, barcode < 2e5 (for the
          //photons), not from hadron decay,
          //skip pdgId check if pdgId is an empty vector,
          //ignore photons coming from hadrons if usePhotonsFromHadrons=false,
          //only use charged particles if chargedOnly=true

          bool skipPdgCheck = (pdgId.size()==0);
          //bypass the hadron veto?

          for (xAOD::TruthParticleContainer::const_iterator pItr=allParticles->begin(); pItr!=allParticles->end(); ++pItr) {
            const xAOD::TruthParticle *particle = *pItr;

            if (particle->status() != 1) continue;

            if (!skipPdgCheck && find(pdgId.begin(), pdgId.end(), abs(particle->pdgId())) == pdgId.end()) continue;

            //ensure particles are not from GEANT
            if (particle->barcode() >= g4BarcodeOffset) continue;

            //check if we have a neutral particle (threeCharge returns int)
            if (chargedOnly && HepPID::threeCharge(particle->pdgId()) == 0) continue;

            //if we have a photon from hadron decay, and usePhotonsFromHadrons=false, skip this particle
            if (!usePhotonsFromHadrons && fromHadronDecay(particle) && particle->pdgId()==22) continue;

            //good particle, add to list
            selectedlist.push_back(particle);
          }
        }

        bool fromHadronDecay(const xAOD::TruthParticle* particle) const
        {
          //does this particle come from a hadron decay?
          //copied and modified from
          //MenuTruthDressingTool::isLeptonFromTau()

          const xAOD::TruthVertex* prod = particle->prodVtx();
          if(!prod) return false; // no parent.

          // Simple loop catch
          if (prod==particle->decayVtx()) return false;

          // Loop over the parents of this particle.
          unsigned int nIncoming = prod->nIncomingParticles();
          for(unsigned int itr = 0; itr<nIncoming; ++itr){
            int parentId = prod->incomingParticle(itr)->pdgId();
            if(HepPID::isHadron(parentId)) {
              return true;
            }

            //note from tancredi: should we search up the chain for leptons as
            //well? B->l->photon should be rejected?
            if(parentId == particle->pdgId()) { 
              //continue searching up photon->photon chains
              if(fromHadronDecay(prod->incomingParticle(itr))) {
                return true;
              }
            }
          }

          return false;
        }

        float calculateDeltaR(const xAOD::IParticle *p1, const xAOD::IParticle *p2) const
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
    };
}


