/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifdef XAOD_ANALYSIS 
#include "IsolationTool/CaloIsolationTool.h"
namespace xAOD {

  // particlesInCone tool will not be avaible. Write our own...
  bool CaloIsolationTool::particlesInCone( float eta, float phi, float dr, std::vector<const CaloCluster*>& output ) const {
    /// retrieve container
    const CaloClusterContainer* caloClusters = 0;
    std::string m_caloClusterLocation = "CaloCalTopoClusters";
    if(evtStore()->retrieve(caloClusters, m_caloClusterLocation).isFailure()) {
      ATH_MSG_FATAL( "Unable to retrieve " << m_caloClusterLocation );
      return false;
    }

    const float m_minPt = 1e-3;
    /// make selection
    float dr2 = dr*dr;
    for(auto trk: *caloClusters){
      if(trk->pt()<m_minPt) continue;

      float dEta = eta-trk->eta();
      float dPhi = Phi_mpi_pi(phi-trk->phi());
      if(dr2<(dEta*dEta+dPhi*dPhi)) continue;
      output.push_back(trk);
    }

    return true;
  }


  bool CaloIsolationTool::particlesInCone( float eta, float phi, float dr, std::vector<const PFO*>& output ){
    /// retrieve container
    const PFOContainer* Clusters = 0;
    std::string m_ClusterLocation = "JetETMissNeutralParticleFlowObjects";
    if(evtStore()->retrieve(Clusters,m_ClusterLocation).isFailure()) {
      ATH_MSG_FATAL( "Unable to retrieve " << m_ClusterLocation);
      return false;
    }

    const float m_minPt = 1e-3;
    /// make selection
    float dr2 = dr*dr;
    for(auto trk: *Clusters){
      if(trk->pt()<m_minPt) continue;
      float dEta = eta-trk->eta();
      float dPhi = Phi_mpi_pi(phi-trk->phi());
      if(dr2<(dEta*dEta+dPhi*dPhi))continue;
      output.push_back(trk);
    }
    return true;
  }
}// end of namespace

#endif


