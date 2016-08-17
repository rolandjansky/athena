/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// ======================================================================
// ClusterNearMuon
// dongliang.zhang@cern.ch
// Given an xAOD::IParticle and an xAOD::CaloClusterContainer container, checks
// which clusters are in a cone of a given size around the IParticle. Sets
// a mask accordingly. Intended for thinning use.
// ======================================================================
#pragma once
#include <ostream>
#include "xAODMuon/Muon.h"
#include "xAODCaloEvent/CaloClusterContainer.h"
#include "xAODCaloEvent/CaloCluster.h"
#include "CaloGeoHelpers/CaloSampling.h"
#include <vector>

namespace DerivationFramework {
  struct ClusterNearMuon{
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
    bool GetExtrapEtaPhi(const xAOD::Muon* mu, float& eta, float& phi)
     {
      auto cluster = mu->cluster();
      if(cluster){
        float etaT = 0, phiT = 0;
        int nSample = 0;
        for(unsigned int i=0; i<CaloSampling::Unknown; i++) // dangerous?
        { 
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
