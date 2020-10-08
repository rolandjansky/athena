/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "GeneratorFilters/HeavyFlavorHadronFilter.h"
#include "GaudiKernel/SystemOfUnits.h"
#include "xAODJet/JetContainer.h"
#include "CxxUtils/BasicTypes.h"
#include <cmath>


HeavyFlavorHadronFilter::HeavyFlavorHadronFilter(const std::string& name, ISvcLocator* pSvcLocator)
  : GenFilter(name, pSvcLocator),
    m_NPass(0), m_Nevt(0), m_NbPass(0), 
    m_NcPass(0), m_NBHadronPass(0), m_NDHadronPass(0), 
    m_NPDGIDPass(0)
{
  declareProperty("CharmPtMin",m_charmPtMin=2.0*Gaudi::Units::GeV);
  declareProperty("BottomPtMin",m_bottomPtMin=5.0*Gaudi::Units::GeV);
  declareProperty("CharmEtaMax",m_charmEtaMax=3.0);
  declareProperty("BottomEtaMax",m_bottomEtaMax=3.0);
  declareProperty("cPtMin",m_cPtMin=0.0*Gaudi::Units::GeV);
  declareProperty("bPtMin",m_bPtMin=0.0*Gaudi::Units::GeV);
  declareProperty("cEtaMax",m_cEtaMax=5.0);
  declareProperty("bEtaMax",m_bEtaMax=5.0);
  declareProperty("JetPtMin",m_jetPtMin=20.0*Gaudi::Units::GeV);
  declareProperty("JetEtaMax",m_jetEtaMax=2.5);
  declareProperty("PDGPtMin", m_PDGPtMin=2.0*Gaudi::Units::GeV);
  declareProperty("PDGEtaMax",m_PDGEtaMax=2.5);
  declareProperty("PDGID",m_PDGID=0);
  declareProperty("PDGAntiParticleToo",m_PDGAntiParticleToo=true);
  declareProperty("RequestCharm",m_RequestCharm=true);
  declareProperty("RequestBottom",m_RequestBottom=true);
  declareProperty("Request_cQuark",m_Request_cQuark=true);
  declareProperty("Request_bQuark",m_Request_bQuark=true);
  declareProperty("RequestSpecificPDGID",m_RequestSpecificPDGID=false);
  declareProperty("RequireTruthJet",m_RequireTruthJet=false);
  declareProperty("DeltaRFromTruth",m_deltaRFromTruth=0.4);
  declareProperty("TruthContainerName",m_TruthJetContainerName="AntiKt4TruthJets");
}


StatusCode HeavyFlavorHadronFilter::filterInitialize() {
  m_Nevt = 0;
  m_NPass = 0;
  m_NbPass = 0;
  m_NcPass = 0;
  m_NBHadronPass = 0;
  m_NDHadronPass = 0;
  m_NPDGIDPass = 0;
  return StatusCode::SUCCESS;
}


StatusCode HeavyFlavorHadronFilter::filterFinalize() {
  ATH_MSG_INFO(m_NPass << " events out of " << m_Nevt << " passed the filter");
  if (m_Request_bQuark) ATH_MSG_INFO(m_NbPass << " passed b-quark selection");
  if (m_Request_cQuark) ATH_MSG_INFO(m_NcPass << " passed c-quark selection");
  if (m_RequestBottom) ATH_MSG_INFO(m_NBHadronPass << " passed B Hadron selection");
  if (m_RequestCharm) ATH_MSG_INFO(m_NDHadronPass << " passed Charm Hadron selection");
  if (m_RequestSpecificPDGID) ATH_MSG_INFO(m_NPDGIDPass << " passed selection for PDG code " << m_PDGID);
  return StatusCode::SUCCESS;
}


StatusCode HeavyFlavorHadronFilter::filterEvent() {
  bool pass = false;
  bool bPass = false;
  bool cPass = false;
  bool BHadronPass = false;
  bool DHadronPass = false;
  bool PDGIDPass = false;

  m_Nevt++;

  std::vector<xAOD::JetContainer::const_iterator> jets;
  if (m_RequireTruthJet) {
    const xAOD::JetContainer* truthjetTES;
    CHECK(evtStore()->retrieve( truthjetTES, m_TruthJetContainerName));
    for (xAOD::JetContainer::const_iterator j = truthjetTES->begin(); j != truthjetTES->end() ; ++j) {
      if ((*j)->pt() > m_jetPtMin && std::abs((*j)->eta()) < m_jetEtaMax) {
        jets.push_back(j);
      }
    }
  }

  for (McEventCollection::const_iterator itr = events()->begin(); itr != events()->end(); ++itr) {
    const HepMC::GenEvent* genEvt = *itr;

    // Loop over all truth particles in the event
    for (auto part: *genEvt) {
      /// @todo This could be so much more efficient! And with so much less code duplication...

      // b-quarks
      // ==========
      // Warning:  Since no navigation is done, the filter does not distinguish
      // between the final quark in the decay chain and intermediates
      // That means the code is NOT appropriate for counting the number
      // of heavy flavor quarks!
      if (m_Request_bQuark && std::abs(part->pdg_id())==5 &&
          part->momentum().perp()>m_bPtMin &&
          std::abs(part->momentum().pseudoRapidity())<m_bEtaMax) {
        if (m_RequireTruthJet) {
          HepMC::FourVector tmp = part->momentum();
          TLorentzVector genpart(tmp.x(), tmp.y(), tmp.z(), tmp.t());
          for (uint i=0; i<jets.size(); i++) {
            double dR = (*jets[i])->p4().DeltaR(genpart);
            if (dR<m_deltaRFromTruth) bPass=true;
          }
        } else {
          bPass = true;
        }
      }

      // c-quarks
      // ========
      // Warning:  Since no navigation is done, the filter does not distinguish
      // between the final quark in the decay chain and intermediates
      // That means the code is NOT appropriate for counting the number
      // of heavy flavor quarks!
      if (m_Request_cQuark &&
          std::abs(part->pdg_id())==4 &&
          part->momentum().perp()>m_cPtMin &&
          std::abs(part->momentum().pseudoRapidity())<m_cEtaMax) {
        if (m_RequireTruthJet) {
          HepMC::FourVector tmp = part->momentum();
          TLorentzVector genpart(tmp.x(), tmp.y(), tmp.z(), tmp.t());
          for (uint i=0; i<jets.size(); i++) {
            double dR = (*jets[i])->p4().DeltaR(genpart);
            if (dR<m_deltaRFromTruth) cPass=true;
          }
        } else {
          cPass = true;
        }
      }

      // B hadrons
      // =========
      if (m_RequestBottom &&
          isBwithWeakDK(part->pdg_id()) &&
          part->momentum().perp()>m_bottomPtMin &&
          std::abs(part->momentum().pseudoRapidity())<m_bottomEtaMax) {
        if (m_RequireTruthJet) {
          HepMC::FourVector tmp = part->momentum();
          TLorentzVector genpart(tmp.x(), tmp.y(), tmp.z(), tmp.t());
          for (uint i=0; i<jets.size(); i++) {
            double dR = (*jets[i])->p4().DeltaR(genpart);
            if (dR < m_deltaRFromTruth) BHadronPass=true;
          }
        } else {
          BHadronPass = true;
        }
      }

      // Charm Hadrons
      // ==============
      if (m_RequestCharm &&
          isDwithWeakDK(part->pdg_id()) &&
          part->momentum().perp()>m_charmPtMin &&
          std::abs(part->momentum().pseudoRapidity())<m_charmEtaMax) {
        if (m_RequireTruthJet) {
          HepMC::FourVector tmp = part->momentum();
          TLorentzVector genpart(tmp.x(), tmp.y(), tmp.z(), tmp.t());
          for (uint i=0; i<jets.size(); i++) {
            double dR = (*jets[i])->p4().DeltaR(genpart);
            if (dR < m_deltaRFromTruth) DHadronPass=true;
          }
        } else {
          DHadronPass = true;
        }
      }

      // Request Specific PDGID
      // =========================
      bool pdgok = m_RequestSpecificPDGID &&
        (part->pdg_id() == m_PDGID ||
         (m_PDGAntiParticleToo && std::abs(part->pdg_id()) == m_PDGID));
      if (pdgok && part->momentum().perp() > m_PDGPtMin &&
          std::abs(part->momentum().pseudoRapidity()) < m_PDGEtaMax) {
        if (m_RequireTruthJet) {
          HepMC::FourVector tmp = part->momentum();
          TLorentzVector genpart(tmp.x(), tmp.y(), tmp.z(), tmp.t());
          for (size_t i = 0; i < jets.size(); ++i) {
            double dR = (*jets[i])->p4().DeltaR(genpart);
            if (dR < m_deltaRFromTruth) PDGIDPass = true;
          }
        } else {
          PDGIDPass = true;
        }
      }

    } //end of particles in one event
  }

  /// @todo This could be so much more efficient!
  pass = BHadronPass || DHadronPass || bPass || cPass || PDGIDPass;
  if (pass)  m_NPass++;
  if (bPass) m_NbPass++;
  if (cPass) m_NcPass++;
  if (BHadronPass) m_NBHadronPass++;
  if (DHadronPass) m_NDHadronPass++;
  if (PDGIDPass)   m_NPDGIDPass++;
  setFilterPassed(pass);

  return StatusCode::SUCCESS;
}


bool HeavyFlavorHadronFilter::isBwithWeakDK(const int pID) const {
  int id = std::abs(pID);
  return ( id == 511   || // B+
           id == 521   || // B0
           id == 531   || // Bs
           id == 541   || // Bc
           id == 5122  || // Lambda_B
           id == 5132  || // Xi_b-
           id == 5232  || // X_b0
           id == 5112  || // Sigma_b-
           id == 5212  || // Sigma_b0
           id == 5222  || // Sigma_b+
           id == 5332 );  // Omega_B
}


bool HeavyFlavorHadronFilter::isDwithWeakDK(const int pID) const {
  int id = std::abs(pID);
  return ( id == 411   || // D+
           id == 421   || // D0
           id == 431   || // Ds
           id == 4122  || // Lambda_C
           id == 4132  || // Xi_C^0
           id == 4232  || // Xi_C^+
           id == 4212  || // Xi_C^0
           id == 4322  || // Xi'_C+  This is in fact EM not weak
           id == 4332); // Omega_C

}
