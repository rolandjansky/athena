/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#include "GeneratorFilters/xAODMultiElecMuTauFilter.h"
#include "CLHEP/Vector/LorentzVector.h"


xAODMultiElecMuTauFilter::xAODMultiElecMuTauFilter(const std::string& name, ISvcLocator* pSvcLocator)
  : GenFilter(name,pSvcLocator)
{
declareProperty("MinPt",                             m_minPt                            = 5000. );
declareProperty("MaxEta",                            m_maxEta                           = 10.0  );
declareProperty("MinVisPtHadTau",                    m_minVisPtHadTau                   = 10000.);
declareProperty("NLeptons",                          m_NLeptons                         = 4     );
declareProperty("IncludeHadTaus",                    m_incHadTau                        = true  );
declareProperty("TwoSameSignLightLeptonsOneHadTau",  m_TwoSameSignLightLeptonsOneHadTau = false );
}


StatusCode xAODMultiElecMuTauFilter::filterEvent() {
  int numLeptons = 0;
  int numLightLeptons = 0;
  int numHadTaus = 0;
  if (m_TwoSameSignLightLeptonsOneHadTau) {
    if (m_NLeptons!=3) ATH_MSG_ERROR("TwoSameSignLightLeptonsOneHadTau request possible only for NLeptons = 3. Check your jobOptions.");
  }
  int charge1 = 0; 
  int charge2 = 0; 

  // Retrieve full TruthEvent container
  const xAOD::TruthEventContainer *xTruthEventContainer;
  if (evtStore()->retrieve(xTruthEventContainer, "TruthEvents").isFailure())
  {
    ATH_MSG_ERROR("No TruthEvents collection with name " << "TruthEvents" << " found in StoreGate!");
    return StatusCode::FAILURE;
  }

  xAOD::TruthEventContainer::const_iterator itr;
  for (itr = xTruthEventContainer->begin(); itr!=xTruthEventContainer->end(); ++itr) {
    const xAOD::TruthEvent *genEvt = (*itr);
    unsigned int nPart = genEvt->nTruthParticles();
    for (unsigned int iPart = 0; iPart < nPart; ++iPart) {
       const xAOD::TruthParticle* pitr =  genEvt->truthParticle(iPart);
       if (pitr->status() == 1 && (std::abs(pitr->pdgId()) == 11 || std::abs(pitr->pdgId()) == 13)) {
         if (pitr->pt() >= m_minPt && std::abs(pitr->eta()) <= m_maxEta) {
           ATH_MSG_DEBUG("Found lepton with PDG ID = " << pitr->pdgId()
                         << ", status = " <<  pitr->status()
                         << ", pt = "     <<  pitr->pt()
                         << ", eta = "    <<  pitr->eta());
            numLeptons++;
            numLightLeptons++;
            if (numLightLeptons==1) { if (pitr->pdgId() < 0) { charge1 = -1; } else { charge1 = 1; } } 
           if (numLightLeptons==2) { if (pitr->pdgId() < 0) { charge2 = -1; } else { charge2 = 1; } } 
         }
         continue;
       }
       //Look for Hadronic taus (leptonic ones will be saved by above) that have status!=3 and don't decay to another tau (FSR)
       if (!m_incHadTau) continue;
       const xAOD::TruthParticle *tau= nullptr;
       const xAOD::TruthParticle *taunu= nullptr;
       if (std::abs(pitr->pdgId()) != 15 || pitr->status() == 3) continue;
         tau = pitr;
         if(!tau->decayVtx()) continue;
         // Loop over children and:
         // 1. Find if it is hadronic (i.e. no decay lepton).
         // 2. Veto tau -> tau (FSR)
         // 3. Store the tau neutino to calculate the visible momentum
         for (unsigned int iChild = 0; iChild < tau->decayVtx()->nOutgoingParticles(); ++iChild) {
             const xAOD::TruthParticle* citr =  tau->decayVtx()->outgoingParticle(iChild);
             //Ignore tau -> tau (FSR)
             if (pitr->pdgId() == citr->pdgId()) {
               ATH_MSG_DEBUG("FSR tau decay.  Ignoring!");
               tau = nullptr;
               break;
           }
             // Ignore leptonic decays
             if (std::abs(citr->pdgId()) == 13 || std::abs(citr->pdgId()) == 11) {
               tau = nullptr;
               break;
          }
            // Find tau decay nu
            if (std::abs(citr->pdgId()) == 16) {
               taunu = citr;
           }
         }
         if (tau) {
           // Good hadronic decay
           CLHEP::HepLorentzVector tauVisMom = CLHEP::HepLorentzVector(tau->px() - taunu->px(),
                                                         tau->py() - taunu->py(),
                                                         tau->pz() - taunu->pz(),
                                                         tau->e()  - taunu->e());
           if (tauVisMom.perp() >= m_minVisPtHadTau && std::abs(tauVisMom.eta()) <= m_maxEta) {
             ATH_MSG_DEBUG("Found hadronic tau decay with PDG ID = " << tau->pdgId()
                           << ", status = " << tau->status()
                           << ", vis pt = " << tauVisMom.perp()
                           << ", eta = " <<  tauVisMom.eta());
             numLeptons++;
             numHadTaus++;
        
           }
       }
    }
  }

 bool passed_event = false;
  if (m_TwoSameSignLightLeptonsOneHadTau) {
    if ( ((numLightLeptons+numHadTaus)==numLeptons) && numLightLeptons == 2 && numHadTaus == 1 && charge1==charge2 ) {
      ATH_MSG_INFO("Found " << numLeptons << " Leptons: two same sign ligh leptons + one hadronic tau! Event passed.");
      passed_event = true;
    }
  } else {
    if ( numLeptons >= m_NLeptons ) {
      ATH_MSG_INFO("Found " << numLeptons << "Leptons. Event passed.");
      passed_event = true;
    }
  }
  setFilterPassed(passed_event);

  return StatusCode::SUCCESS;
}
