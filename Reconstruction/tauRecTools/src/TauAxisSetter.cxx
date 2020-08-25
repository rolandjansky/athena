/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef XAOD_ANALYSIS

#include "TauAxisSetter.h"
#include "tauRecTools/HelperFunctions.h"

#include "xAODTau/TauJetContainer.h"
#include "xAODTau/TauJetAuxContainer.h"
#include "xAODTau/TauJet.h"
#include "CaloUtils/CaloVertexedCluster.h"



TauAxisSetter::TauAxisSetter(const std::string& name) :
TauRecToolBase(name) {
}



TauAxisSetter::~TauAxisSetter() { 
}



StatusCode TauAxisSetter::initialize() {
  return StatusCode::SUCCESS;
}



StatusCode TauAxisSetter::execute(xAOD::TauJet& pTau) const {

  const xAOD::Jet* pJetSeed = (*pTau.jetLink());
  if (!pJetSeed) {
    ATH_MSG_ERROR("Tau jet link is invalid.");
    return StatusCode::FAILURE;
  }

  // Barycenter is the sum of cluster p4 in the seed jet
  TLorentzVector baryCenter;  
  
  xAOD::JetConstituentVector constituents = pJetSeed->getConstituents();
  for (const xAOD::JetConstituent* constituent : constituents) {
    TLorentzVector constituentP4;
    constituentP4.SetPtEtaPhiE(constituent->pt(), constituent->eta(), constituent->phi(), constituent->e());
    baryCenter += constituentP4;
  }
  
  ATH_MSG_DEBUG("barycenter (eta, phi): "  << baryCenter.Eta() << " " << baryCenter.Phi());

  // Detector axis is the sum of cluster p4 within dR core of the seed jet 
  TLorentzVector tauDetectorAxis;
  int nConstituents = 0;

  for (const xAOD::JetConstituent* constituent : constituents) {
    TLorentzVector constituentP4;
    constituentP4.SetPtEtaPhiE(constituent->pt(), constituent->eta(), constituent->phi(), constituent->e());
    
    double dR = baryCenter.DeltaR(constituentP4);
    if (dR > m_clusterCone) continue;

    ElementLink<xAOD::IParticleContainer> linkToCluster;
    linkToCluster.toContainedElement(
      *(static_cast<const xAOD::IParticleContainer*> (constituent->rawConstituent()->container())), 
      constituent->rawConstituent()
      );
    pTau.addClusterLink(linkToCluster);

    tauDetectorAxis += constituentP4;
    ++nConstituents;
  }

  if (0 == nConstituents) {
    ATH_MSG_DEBUG("this tau candidate does not have any constituent clusters!");
    return StatusCode::FAILURE;
  }

  ATH_MSG_DEBUG("detector axis:" << tauDetectorAxis.Pt()<< " " << tauDetectorAxis.Eta() << " " << tauDetectorAxis.Phi()  << " " << tauDetectorAxis.E());
  pTau.setP4(tauDetectorAxis.Pt(), tauDetectorAxis.Eta(), tauDetectorAxis.Phi(), pTau.m());
  pTau.setP4(xAOD::TauJetParameters::DetectorAxis, tauDetectorAxis.Pt(), tauDetectorAxis.Eta(), tauDetectorAxis.Phi(), tauDetectorAxis.M());


  if(m_doVertexCorrection) {
    // calculate tau intermediate axis (corrected for tau vertex)
    TLorentzVector tauInterAxis;

    std::vector<const xAOD::CaloCluster*> clusterList;
    ATH_CHECK(tauRecTools::GetJetClusterList(pJetSeed, clusterList, m_incShowerSubtr, baryCenter, m_clusterCone));
    for (auto cluster : clusterList){
      if (pTau.vertexLink()) {
        tauInterAxis += xAOD::CaloVertexedCluster(*cluster, (*pTau.vertexLink())->position()).p4();
      }
      else {
        tauInterAxis += xAOD::CaloVertexedCluster(*cluster).p4();
      }
    }

    ATH_MSG_DEBUG("tau axis:" << tauInterAxis.Pt()<< " " << tauInterAxis.Eta() << " " << tauInterAxis.Phi()  << " " << tauInterAxis.E() );
    pTau.setP4(tauInterAxis.Pt(), tauInterAxis.Eta(), tauInterAxis.Phi(), pTau.m());
    pTau.setP4(xAOD::TauJetParameters::IntermediateAxis, tauInterAxis.Pt(), tauInterAxis.Eta(), tauInterAxis.Phi(), tauInterAxis.M());
  }

  return StatusCode::SUCCESS;
}



StatusCode TauAxisSetter::finalize() {
    return StatusCode::SUCCESS;
}

#endif
