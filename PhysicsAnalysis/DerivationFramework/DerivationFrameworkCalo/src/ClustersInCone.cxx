/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "xAODMuon/Muon.h"
#include "xAODEgamma/Egamma.h"
#include "xAODCaloEvent/CaloCluster.h"
#include "FourMomUtils/P4Helpers.h"
#include "DerivationFrameworkCalo/ClustersInCone.h"
#include "GaudiKernel/MsgStream.h"

//Select for generic IParticle
void DerivationFramework::ClustersInCone::select(const xAOD::IParticle* particle, 
						 const float coneSize, const xAOD::CaloClusterContainer* clusters, std::vector<bool> &mask) {
  float particleEta = particle->eta();
  float particlePhi = particle->phi();
  unsigned int i(0);
  for (xAOD::CaloClusterContainer::const_iterator clIt = clusters->begin(); clIt!=clusters->end(); ++clIt, ++i) {
    float deltaEta = (*clIt)->eta() - particleEta;
    float deltaPhi = P4Helpers::deltaPhi((*clIt)->phi(), particlePhi);
    float deltaR = sqrt(deltaEta*deltaEta + deltaPhi*deltaPhi);
    if (deltaR < coneSize) {
      mask[i] = true;
    }
  }
  return;
}
    
//Overload for Egamma
void DerivationFramework::ClustersInCone::select(const xAOD::Egamma* particle, 
						 const float coneSize, const xAOD::CaloClusterContainer* clusters, std::vector<bool> &mask) {
  float particleEta = particle->caloCluster()->etaBE(2);
  float particlePhi = particle->caloCluster()->phiBE(2);
  unsigned int i(0);
  for (xAOD::CaloClusterContainer::const_iterator clIt = clusters->begin(); clIt!=clusters->end(); ++clIt, ++i) {
    float deltaEta = (*clIt)->eta() - particleEta;
    float deltaPhi = P4Helpers::deltaPhi((*clIt)->phi(), particlePhi);
    float deltaR = sqrt(deltaEta*deltaEta + deltaPhi*deltaPhi);
    if (deltaR < coneSize) {
      mask[i] = true;
    }
  }
  return;
}
    
//Helper for the muon overload
bool DerivationFramework::ClustersInCone::GetExtrapEtaPhi(const xAOD::Muon* mu, float& eta, float& phi){
  auto cluster = mu->cluster();
  if(cluster){
    float etaT = 0, phiT = 0;
    int nSample = 0;
    for(unsigned int i=0; i<CaloSampling::Unknown; i++) {
      auto s = static_cast<CaloSampling::CaloSample>(i);
      if(!cluster->hasSampling(s)) continue;
      etaT += cluster->etaSample(s);
      phiT += cluster->phiSample(s);
      nSample++;
    }
    if(nSample>0){
      eta = etaT/nSample;
      phi = phiT/nSample;
    }else{
      return false;
    }
  }else{
    return false;
  }
  return true;
}

//Overload for Muons 
void DerivationFramework::ClustersInCone::select(const xAOD::Muon* particle, const float coneSize, 
						 const xAOD::CaloClusterContainer* clusters, std::vector<bool> &mask, MsgStream& msg ) {
  float particleEta = particle->eta();
  float particlePhi = particle->phi();
  if(!GetExtrapEtaPhi(particle, particleEta, particlePhi)){
    msg<<MSG::WARNING<<"Failed to get the eta-phi of the muon calo extention "<<endmsg;    
  }
  unsigned int i(0);
  float coneSize2 = coneSize*coneSize;
  for (xAOD::CaloClusterContainer::const_iterator clIt = clusters->begin(); clIt!=clusters->end(); ++clIt, ++i) {
    float deltaEta = (*clIt)->eta() - particleEta;
    float deltaPhi = P4Helpers::deltaPhi((*clIt)->phi(), particlePhi);
    if(deltaEta*deltaEta + deltaPhi*deltaPhi < coneSize2) {
      mask[i] = true;
    }
  }
  return;
}
