/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/* 
 * File:   TileClusterFilterAlg.cxx
 * Author: Carlos.Solans@cern.ch
 */

#include "TileClusterFilterAlg.h"

TileClusterFilterAlg::TileClusterFilterAlg( const std::string& name, ISvcLocator* pSvcLocator ): 
  AthAlgorithm( name, pSvcLocator ){
  
  declareProperty("InputContainer", m_inputContainerName="CaloTopoClusters");
  declareProperty("OutputContainer", m_outputContainerName="SelectedClusters");
  declareProperty("TrackContainer", m_trackContainerName="SelectedTracks");
  declareProperty("TrackTools", m_trackInCalo);
  declareProperty("DeltaR", m_deltaR=0.2);
  m_storeGate = 0;
} 

StatusCode TileClusterFilterAlg::initialize(){
    
  ATH_MSG_INFO("TileClusterFilterAlg::initialize() ");
  
  CHECK(m_trackInCalo.retrieve());
  CHECK(service("StoreGateSvc",m_storeGate));
  
  return StatusCode::SUCCESS;
} 

StatusCode TileClusterFilterAlg::execute(){
  
  //Get input clusters
  const CLUSTERCONTAINER* inputClusters=0;
  CHECK( evtStore()->retrieve( inputClusters, m_inputContainerName ) );

  //Allocate output clusters container
  xAOD::CaloClusterContainer* outputClusters = new xAOD::CaloClusterContainer;
  xAOD::CaloClusterAuxContainer* outputAuxClusters = new xAOD::CaloClusterAuxContainer;
  outputClusters->setStore( outputAuxClusters );

  //Get the input tracks
  const TRACKCONTAINER* inputTracks = 0;
  CHECK( m_storeGate->retrieve( inputTracks, m_trackContainerName ) );

  xAOD::CaloClusterContainer::const_iterator clusterItr = inputClusters->begin();
  xAOD::CaloClusterContainer::const_iterator clusterEnd = inputClusters->end();
  while(clusterItr != clusterEnd){
    const CLUSTER* cluster = *clusterItr;
    TRACKCONTAINER::const_iterator it = inputTracks->begin();
    for( ; it != inputTracks->end(); ++it ){
      const TRACK* track = *it;
      std::vector< std::vector<double> > etrack = m_trackInCalo->getXYZEtaPhiPerLayer(track);
      double deltaR=999;
      for(int i=0;i<11;i++){
	double dif_eta = cluster->etaSample((CaloSampling::CaloSample)i) - etrack[i][3];
	//double dif_phi = CaloPhiRange::diff(cluster->phiSample((CaloSampling::CaloSample)i) - etrack[i][4]);
	double dif_phi = cluster->phiSample((CaloSampling::CaloSample)i) - etrack[i][4];
	if(dif_phi<0) dif_phi=-dif_phi;
	if(dif_phi>M_PI){dif_phi=2*M_PI-dif_phi;}
	double tmp = sqrt(dif_eta*dif_eta+dif_phi*dif_phi);
	deltaR = (tmp<deltaR ? tmp : deltaR);
      }
      if(deltaR<=m_deltaR){
        CLUSTER* tmpCluster = new CLUSTER();
        tmpCluster->makePrivateStore( cluster );
        outputClusters->push_back(tmpCluster);
      }
    } 
  }
  CHECK( evtStore()->record(outputClusters, m_outputContainerName ) );
  CHECK( evtStore()->record(outputAuxClusters, m_outputContainerName+"Aux.") );

  return StatusCode::SUCCESS;
}

