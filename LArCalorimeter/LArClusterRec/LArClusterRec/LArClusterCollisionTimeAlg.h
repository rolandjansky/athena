/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef LARCLUSTERCOLLISIONTIMEALG_H
#define LARCLUSTERCOLLISIONTIMEALG_H

#include "Gaudi/Property.h"

#include "AthenaBaseComps/AthAlgorithm.h"
#include "xAODCaloEvent/CaloClusterContainer.h"

#include "LArRecEvent/LArCollisionTime.h"

#include "StoreGate/ReadHandleKey.h"
#include "StoreGate/WriteHandleKey.h"


class LArClusterCollisionTimeAlg : public AthAlgorithm {
 public:
  LArClusterCollisionTimeAlg(const std::string& name, ISvcLocator* pSvcLocator);
  virtual ~LArClusterCollisionTimeAlg();
    
  /** standard Athena-Algorithm method */
  StatusCode          initialize() override final;
  /** standard Athena-Algorithm method */
  StatusCode          execute() override final;
  /** standard Athena-Algorithm method */
  StatusCode          finalize() override final;

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
  unsigned m_nEvt;
  unsigned m_nCollEvt;

  //---------------------------------------------------
  // Properties
  //--------------------------------------------------- 
  Gaudi::Property<float> m_timeCut { this, "timeDiffCut", 2., "max |A-C| time difference tu pass the filter" };
  Gaudi::Property<size_t> m_maxClusters { this, "maxNClusters", 3, "how many clusters taken into sum" };

  SG::ReadHandleKey<xAOD::CaloClusterContainer> m_clusterContainerName;
  SG::WriteHandleKey<LArCollisionTime> m_outputName;
  
};
#endif
