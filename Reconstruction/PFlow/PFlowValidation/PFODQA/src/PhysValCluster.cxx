/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#include "PhysValCluster.h"
#include "xAODCaloEvent/CaloClusterContainer.h"
#include "xAODEventInfo/EventInfo.h"
#include "GaudiKernel/EventContext.h"

PhysValCluster::PhysValCluster (const std::string& type, const std::string& name, const IInterface* parent ) : ManagedMonitorToolBase( type, name, parent ) {
}

StatusCode PhysValCluster::initialize(){
  ATH_CHECK(m_clusterContainerName.initialize());
  ATH_CHECK(m_eventInfoName.initialize());
  return ManagedMonitorToolBase::initialize();
}


StatusCode PhysValCluster::bookHistograms(){

  std::string theName = "TopoClusters/TopoClusters_JetETMiss/JetETMiss_"+m_clusterContainerName.key();

  m_clusterValidationPlots.reset(new ClusterValidationPlots(0,theName, m_clusterContainerName));
  m_clusterValidationPlots->setDetailLevel(100);
  m_clusterValidationPlots->initialize();
  std::vector<HistData> hists = m_clusterValidationPlots->retrieveBookedHistograms();

  for (auto hist : hists) {
    ATH_CHECK(regHist(hist.first,hist.second,all));
  }
  
  return StatusCode::SUCCESS;     
   
}

StatusCode PhysValCluster::fillHistograms(){

  const EventContext& ctx = Gaudi::Hive::currentContext();
  SG::ReadHandle<xAOD::EventInfo> evInfoHdl{m_eventInfoName, ctx}; 
  if(!evInfoHdl.isValid()){
     ATH_MSG_ERROR("Do not have EventInfo with key " << m_eventInfoName.key());
     return StatusCode::FAILURE;
  }

  SG::ReadHandle<xAOD::CaloClusterContainer> clusHdl{m_clusterContainerName, ctx};

  if(clusHdl.isValid()){
    for(auto cluster: *clusHdl) {
      m_clusterValidationPlots->fill(*cluster,*evInfoHdl);
    }
  }
  else ATH_MSG_WARNING(" Cluster container : " << m_clusterContainerName.key() << " not found");
  return StatusCode::SUCCESS;

}

