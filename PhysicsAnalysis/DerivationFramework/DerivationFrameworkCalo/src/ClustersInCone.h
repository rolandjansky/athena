/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef DERIVATIONFRAMEWORK_CLUSTERSINCONE_H
#define DERIVATIONFRAMEWORK_CLUSTERSINCONE_H

#include "xAODMuon/Muon.h"
#include "xAODBase/IParticle.h"
#include "xAODCaloEvent/CaloClusterFwd.h"
#include "xAODEgamma/EgammaFwd.h"
class MsgStream;


namespace DerivationFramework {
  namespace ClustersInCone{
    //Select for generic IParticle
    void select(const xAOD::IParticle* particle, const float coneSize, const xAOD::CaloClusterContainer* clusters, std::vector<bool> &mask) ;    
    //Overload for Egamma
    void select(const xAOD::Egamma* particle, const float coneSize, const xAOD::CaloClusterContainer* clusters, std::vector<bool> &mask) ;    
    //Overload for Muons 
    void select(const xAOD::Muon* particle, const float coneSize, const xAOD::CaloClusterContainer* clusters, std::vector<bool> &mask, MsgStream& msg );
    //Helper for the muon overload
    bool GetExtrapEtaPhi(const xAOD::Muon* mu, float& eta, float& phi);
  }
}

#endif
