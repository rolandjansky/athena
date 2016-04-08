/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef LARCLUSTERCOLLISIONTIMEALG_H
#define LARCLUSTERCOLLISIONTIMEALG_H

#include "AthenaBaseComps/AthAlgorithm.h"
#include "xAODCaloEvent/CaloClusterContainer.h"


class LArClusterCollisionTimeAlg : public AthAlgorithm {
 public:
  LArClusterCollisionTimeAlg(const std::string& name, ISvcLocator* pSvcLocator);
  ~LArClusterCollisionTimeAlg();
    
  /** standard Athena-Algorithm method */
  StatusCode          initialize();
  /** standard Athena-Algorithm method */
  StatusCode          execute();
  /** standard Athena-Algorithm method */
  StatusCode          finalize();

 private:

  struct perSide_t {
    float time;
    float energy;
    unsigned nClusters;
  public:
  perSide_t() : time(0.), energy(0.),nClusters(0) {} ;
  perSide_t(const float t, const float e, const unsigned nC) : time(t),energy(e),nClusters(nC) {} ;
  };

  perSide_t analyseClustersPerSide(std::vector<const xAOD::CaloCluster*>& clusters) const;


    
  //---------------------------------------------------
  // Member variables
  //---------------------------------------------------
  unsigned m_nEvt, m_nCollEvt;
  float m_timeCut;
  size_t m_maxClusters;
  std::string m_clusterContainerName;
  std::string m_outputName;
  
};
#endif
