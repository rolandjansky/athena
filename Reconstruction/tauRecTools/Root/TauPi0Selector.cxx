/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#include "tauRecTools/TauPi0Selector.h"



TauPi0Selector::TauPi0Selector(const std::string& name) : 
    TauRecToolBase(name) {
  declareProperty("ClusterEtCut", m_clusterEtCut);
  declareProperty("ClusterBDTCut_1prong", m_clusterBDTCut_1prong);
  declareProperty("ClusterBDTCut_mprong", m_clusterBDTCut_mprong);
}



StatusCode TauPi0Selector::executePi0nPFO(xAOD::TauJet& pTau, xAOD::PFOContainer& neutralPFOContainer) const {
  // Clear vector of cell-based pi0 PFO Links. Required when rerunning on xAOD level.
  pTau.clearProtoPi0PFOLinks();
  
  // Decay mode enum
  auto kDecayModeProto = xAOD::TauJetParameters::PanTau_DecayModeProto;

  // 0, >=5 prong taus will have Mode_NotSet 
  if (pTau.nTracks() == 0 || pTau.nTracks() >5 ) {
      pTau.setPanTauDetail(kDecayModeProto, xAOD::TauJetParameters::DecayMode::Mode_NotSet);
      return StatusCode::SUCCESS;
  }

  // 1-5 prong taus have Mode_Other by default
  // 1, 3 prong taus will be over-written
  pTau.setPanTauDetail(kDecayModeProto, xAOD::TauJetParameters::DecayMode::Mode_Other);

  // Apply selection to the pi0, and count the number
  int nRecoPi0s=0;
  for(size_t i=0; i<pTau.nProtoNeutralPFOs(); i++) {
    xAOD::PFO* neutralPFO = neutralPFOContainer.at( pTau.protoNeutralPFO(i)->index() );

    // Set number of pi0s to 0 for all neutral PFOs. Required when rerunning on xAOD level
    neutralPFO->setAttribute<int>(xAOD::PFODetails::PFOAttributes::nPi0Proto, 0);

    // Only consider PFOs within 0.2 cone of the tau axis
    if (pTau.p4().DeltaR(neutralPFO->p4()) > 0.2) continue;
    
    int etaBin = getEtaBin( neutralPFO->cluster(0)->eta() );
    
    // Apply Et cut
    if (neutralPFO->p4().Et() < m_clusterEtCut.at(etaBin)) continue;
    
    // Apply BDT score cut
    double BDTScore = neutralPFO->bdtPi0Score();
    if ((pTau.nTracks() == 1 && BDTScore < m_clusterBDTCut_1prong.at(etaBin)) || 
        (pTau.nTracks() > 1 && BDTScore < m_clusterBDTCut_mprong.at(etaBin))) continue;

    int nHitsInEM1 = 0;
    if (!neutralPFO->attribute(xAOD::PFODetails::cellBased_NHitsInEM1, nHitsInEM1)) { 
      ATH_MSG_WARNING("Couldn't retrieve nHitsInEM1. Will set it to 0.");
    }

    // nHitsInEM1 < 3 --- one pi0; nHitsInEM1 >= 3 --- two pi0s
    // FIXME: what about nHitsInEM1 == 0 ?
    if (nHitsInEM1 < 3) { 
      neutralPFO->setAttribute<int>(xAOD::PFODetails::PFOAttributes::nPi0Proto, 1);
      ++nRecoPi0s;
    }   
    else {
      neutralPFO->setAttribute<int>(xAOD::PFODetails::PFOAttributes::nPi0Proto, 2);
      nRecoPi0s += 2;
    }

    pTau.addProtoPi0PFOLink(ElementLink< xAOD::PFOContainer > (neutralPFO, neutralPFOContainer));
  }

  // Set Proto Decay Mode based on the number charged tracks and pi0s
  if (pTau.nTracks()==1) {
    if (nRecoPi0s==0) {
      pTau.setPanTauDetail(kDecayModeProto, xAOD::TauJetParameters::DecayMode::Mode_1p0n);
    }
    else if (nRecoPi0s==1) {
      pTau.setPanTauDetail(kDecayModeProto, xAOD::TauJetParameters::DecayMode::Mode_1p1n);
    }
    else {
      pTau.setPanTauDetail(kDecayModeProto, xAOD::TauJetParameters::DecayMode::Mode_1pXn);
    }
  }
  else if (pTau.nTracks()==3) {
    if (nRecoPi0s==0) {
      pTau.setPanTauDetail(kDecayModeProto, xAOD::TauJetParameters::DecayMode::Mode_3p0n);
    }
    else {
      pTau.setPanTauDetail(kDecayModeProto, xAOD::TauJetParameters::DecayMode::Mode_3pXn);
    }
  }
  
  return StatusCode::SUCCESS;
}



int TauPi0Selector::getEtaBin(double eta) const {
  int etaBin = -1;
  
  double absEta = std::abs(eta);

  if (absEta < 0.80) {
    etaBin = 0;
  }
  else if (absEta < 1.40) {
    etaBin = 1;
  }
  else if (absEta < 1.50) {
    etaBin = 2;
  }
  else if (absEta < 1.90) {
    etaBin = 3;
  }
  else {
    etaBin = 4;
  }

  return etaBin;
}
