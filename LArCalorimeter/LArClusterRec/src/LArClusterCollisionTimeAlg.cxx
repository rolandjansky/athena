/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "LArClusterRec/LArClusterCollisionTimeAlg.h"
#include "LArRecEvent/LArCollisionTime.h"

#include <algorithm>

//Constructor
LArClusterCollisionTimeAlg:: LArClusterCollisionTimeAlg(const std::string& name, ISvcLocator* pSvcLocator):
    AthAlgorithm(name,pSvcLocator), 
    m_nEvt(0),
    m_nCollEvt(0)
  {
    declareProperty("timeDiffCut",m_timeCut=2);
    declareProperty("maxNClusters",m_maxClusters=3);
    declareProperty("InputName", m_clusterContainerName="LArClusterEM");
    declareProperty("OutputName",m_outputName="LArClusterCollTime"); 
  }
  
//__________________________________________________________________________
//Destructor
  LArClusterCollisionTimeAlg::~LArClusterCollisionTimeAlg()
  {
    ATH_MSG_DEBUG ("LArClusterCollisionTimeAlg destructor called");
  }

//__________________________________________________________________________
StatusCode LArClusterCollisionTimeAlg::initialize()
  {
    ATH_MSG_INFO ("LArClusterCollisionTimeAlg initialize()");
    return StatusCode::SUCCESS; 

  }

//__________________________________________________________________________
StatusCode LArClusterCollisionTimeAlg::finalize()
  {
   
    msg(MSG::INFO) << m_nCollEvt << "/" << m_nEvt << " Events found to be collision events with at least one cluster in each EM endcap." << endreq;

    ATH_MSG_DEBUG ("LArClusterCollisionTimeAlg finalize()");
    return StatusCode::SUCCESS; 
  }
  



LArClusterCollisionTimeAlg::perSide_t LArClusterCollisionTimeAlg::analyseClustersPerSide(std::vector<const xAOD::CaloCluster*>& clusters) const {

  perSide_t result;

  //Sort clusters by Energy
  std::sort(clusters.begin(),clusters.end(),[](const xAOD::CaloCluster* o1, const xAOD::CaloCluster* o2) {return o1->e() > o2->e();}); 

  result.nClusters=std::min(m_maxClusters,clusters.size());
  
  if (result.nClusters>0) {
    for (size_t i=0;i<result.nClusters;++i) {
      const xAOD::CaloCluster* clu=clusters[i];
      result.time+=clu->time();
      result.energy+=clu->e();
    }

    result.time/=(float)(result.nClusters);
    result.energy/=(float)(result.nClusters);
  }

  return result;
}
  
//__________________________________________________________________________
StatusCode LArClusterCollisionTimeAlg::execute() {
  ATH_MSG_DEBUG ("LArClusterCollisionTimeAlg execute()");

  m_nEvt++;
  
  const xAOD::CaloClusterContainer* cluster_container=0;
  StatusCode sc=evtStore()->retrieve(cluster_container, m_clusterContainerName);
  if (sc.isFailure()) {
    msg(MSG::ERROR) << "Failed to retreive xAOD::CaloClusterCollection with name " <<  m_clusterContainerName << endreq;
    return sc;
  }
   

  std::vector<const xAOD::CaloCluster*> clustersEMECA,clustersEMECC;

  const unsigned barrelPattern=CaloSampling::barrelPattern();
  
  for(const xAOD::CaloCluster* pCluster :  *cluster_container) {
    if ((pCluster->samplingPattern() & barrelPattern) == 0 ) { //Take only clusters that have only endcap samplings, ignore barrel
      if (pCluster->eta()>0.)
	clustersEMECA.push_back(pCluster);
      else
	clustersEMECC.push_back(pCluster);
    }
  } //End loop over clusters

  const perSide_t sideA=analyseClustersPerSide(clustersEMECA);
  const perSide_t sideC=analyseClustersPerSide(clustersEMECC);

  LArCollisionTime * larTime = new LArCollisionTime(sideA.nClusters,sideC.nClusters,sideA.energy,sideC.energy,sideA.time,sideC.time);
  ATH_CHECK( evtStore()->record(larTime,m_outputName) );

  ATH_MSG_DEBUG("Number/Time/Energy, Side A: " << sideA.nClusters << "/" << sideA.time << "/" << sideA.energy);
  ATH_MSG_DEBUG("Number/Time/Energy, Side C: " << sideC.nClusters << "/" << sideC.time << "/" << sideC.energy);
  
  if ( sideA.nClusters>0 && sideC.nClusters>0 && std::fabs(sideA.time-sideC.time)<m_timeCut) {
    setFilterPassed(true);
    m_nCollEvt++;
  }
  else {
    setFilterPassed(false);
  }

  
  return StatusCode::SUCCESS;
}
