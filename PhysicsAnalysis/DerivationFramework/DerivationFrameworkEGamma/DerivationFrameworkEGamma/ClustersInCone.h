/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// ======================================================================
// ClustersInCone
// simone.mazza@mi.infn.it
// Given an xAOD::IParticle and an xAOD::CaloClusterContainer container, checks
// which clusters are in a cone of a given size around the IParticle. Sets
// a mask accordingly. Intended for thinning use.
// ======================================================================
#ifndef DERIVATIONFRAMEWORK_CLUSTERSINCONE_H
#define DERIVATIONFRAMEWORK_CLUSTERSINCONE_H

#include "xAODMuon/Muon.h"
#include "xAODBase/IParticle.h"
#include "xAODCaloEvent/CaloCluster.h"

namespace DerivationFramework {
  struct ClustersInCone{

    //Select for generic IParticle
    void select(const xAOD::IParticle* particle, float coneSize, const xAOD::CaloClusterContainer* clusters, std::vector<bool> &mask) {
      float particleEta = particle->eta();
  		float particlePhi = particle->phi();
      unsigned int i(0);
      for (xAOD::CaloClusterContainer::const_iterator clIt = clusters->begin(); clIt!=clusters->end(); ++clIt, ++i) {
    		float deltaEta = (*clIt)->eta() - particleEta;
    		float deltaPhi = fabs((*clIt)->phi() - particlePhi);
    		if (deltaPhi > TMath::Pi()) deltaPhi = 2.0*TMath::Pi() - deltaPhi;
    		float deltaR = sqrt(deltaEta*deltaEta + deltaPhi*deltaPhi);
    		if (deltaR < coneSize) mask[i] = true;
      }
      return;
    }

    //Overload for Egamma
    void select(const xAOD::Egamma* particle, float coneSize, const xAOD::CaloClusterContainer* clusters, std::vector<bool> &mask) {
      float particleEta = particle->caloCluster()->etaBE(2);
      float particlePhi = particle->caloCluster()->phiBE(2);
      unsigned int i(0);
      for (xAOD::CaloClusterContainer::const_iterator clIt = clusters->begin(); clIt!=clusters->end(); ++clIt, ++i) {
        float deltaEta = (*clIt)->eta() - particleEta;
        float deltaPhi = fabs((*clIt)->phi() - particlePhi);
        if (deltaPhi > TMath::Pi()) deltaPhi = 2.0*TMath::Pi() - deltaPhi;
        float deltaR = sqrt(deltaEta*deltaEta + deltaPhi*deltaPhi);
        if (deltaR < coneSize) mask[i] = true;
      }
      return;
    }

    //Overload for Muons 
    void select(const xAOD::Muon* particle, float coneSize, const xAOD::CaloClusterContainer* clusters, std::vector<bool> &mask) {
      float particleEta = particle->eta();
      float particlePhi = particle->phi();
      if(!GetExtrapEtaPhi(particle, particleEta, particlePhi)){
        std::cerr << "Failed to get the eta-phi of the muon calo extention" << std::endl;
       }
      unsigned int i(0);
      float coneSize2 = coneSize*coneSize;
      for (xAOD::CaloClusterContainer::const_iterator clIt = clusters->begin(); clIt!=clusters->end(); ++clIt, ++i) {
        float deltaEta = (*clIt)->eta() - particleEta;
        float deltaPhi = fabs((*clIt)->phi() - particlePhi);
        if (deltaPhi > TMath::Pi()) deltaPhi = 2.0*TMath::Pi() - deltaPhi;
        if(deltaEta*deltaEta + deltaPhi*deltaPhi < coneSize2) mask[i] = true;
       }
      return;
    }

    //Helper for the muon overload
    bool GetExtrapEtaPhi(const xAOD::Muon* mu, float& eta, float& phi){
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

  };
}

#endif
