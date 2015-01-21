/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/* 
 * File:   TileEopFilterAlg.cxx
 * Author: Carlos.Solans@cern.ch
 */

#include "TileEopFilterAlg.h"
#include "CaloGeoHelpers/CaloSampling.h"
#include "AthContainers/ConstDataVector.h"

using namespace std;

//=======================================
TileEopFilterAlg::TileEopFilterAlg( const std::string& name, ISvcLocator* pSvcLocator ): 
  AthAlgorithm( name, pSvcLocator ), m_trackInCalo("TrackExtrapolator"){
//=======================================
  
  declareProperty("InputTracks",     m_inputTracks     = "InDetTrackParticles"); 
  declareProperty("OutputTracks",    m_outputTracks    = "SelectedTracks");
  declareProperty("InputClusters",   m_inputClusters   = "CaloTopoClusters");
  declareProperty("OutputClusters",  m_outputClusters  = "SelectedClusters");
  declareProperty("InputCells",      m_inputCells      = "AllCalo");
  declareProperty("OutputCells",     m_outputCells     = "SelectedCells");
  declareProperty("TrackClusters",   m_trackClusters   = "TrackClusters");
  declareProperty("TrackCells",      m_trackCells      = "TrackCells");
  declareProperty("ClusterCells",    m_clusterCells    = "ClusterCells");
  declareProperty("TrackMomCut",     m_trackMom        = 1000.);
  declareProperty("TrackEtaCut",     m_trackEta        = 2.4); 
  declareProperty("TrackIsoCut",     m_trackIso        = 0.8);  
  declareProperty("TrackPtrelCut",   m_trackPtrel      = 0.15);  
  declareProperty("TrackClusterR",   m_trackClusterR   = 0.2);  
  declareProperty("TrackCellR",      m_trackCellR      = 1.0);  
  declareProperty("DumpLArCells",    m_dumpLarCells    = false);
  declareProperty("TrackTools",   m_trackInCalo);
  m_storeGate =	0;
} 

//=======================================
StatusCode TileEopFilterAlg::initialize(){
//=======================================
      
  CHECK(service("StoreGateSvc",m_storeGate));
  CHECK(m_trackInCalo.retrieve());
  //CHECK(m_trackIsoTool.retrieve());
  
  return StatusCode::SUCCESS;
} 

//=======================================
StatusCode TileEopFilterAlg::execute(){
//=======================================

  //Get the input tracks
  const TRACKCONTAINER* inputTracks = 0;
  CHECK( m_storeGate->retrieve( inputTracks, m_inputTracks ) );
  
  //Allocate the output tracks container
  TRACKCONTAINER * outputTracks = new TRACKCONTAINER;
  TRACKAUXCONTAINER* outputAuxTracks = new TRACKAUXCONTAINER;
  outputTracks->setStore( outputAuxTracks );

  
  //Select tracks
  TRACKCONTAINER::const_iterator trackItr = inputTracks->begin();
  TRACKCONTAINER::const_iterator trackEnd = inputTracks->end();
  for(; trackItr != trackEnd; ++trackItr ){
    const TRACK* track = *trackItr;
    if(!track){ ATH_MSG_INFO("Not a valid track"); continue; }
    //Cut 1. Momentum
    if(track->p4().P() >= m_trackMom){     
      //Cut 2. Eta
      if(fabs(track->eta()) <= m_trackEta){
	/*
	//Cut 3. Isolation: Only one track in that cone
	//Extrapolate the current track and all other tracks and check they don't overlap in the range
	int tracksInCone=1;
	TRACKCONTAINER::const_iterator trackItr2 = inputTracks->begin();
	TRACKCONTAINER::const_iterator trackEnd2 = inputTracks->end();
	vector< vector<double> > etrack = m_trackInCalo->getXYZEtaPhiPerLayer(track);
	for(; trackItr2 != trackEnd2; ++trackItr2 ){
	  if(trackItr==trackItr2) continue;
	  const TRACK* track2 = *trackItr2;
	  if(track2->p() < m_trackMom) continue;
	  vector< vector<double> > etrack2 = m_trackInCalo->getXYZEtaPhiPerLayer(track2);
	  for(int i=0;i<11;i++){
	    double dif_eta = etrack2[i][3] - etrack[i][3];
	    double dif_phi = etrack2[i][4] - etrack[i][4];
	    if(dif_phi<0) dif_phi=-dif_phi;
	    if(dif_phi>M_PI){dif_phi=2*M_PI-dif_phi;}
	    double deltaR = sqrt(dif_eta*dif_eta+dif_phi*dif_phi);
	    if(deltaR<m_trackIso) tracksInCone++;
	  }
	}
	if(tracksInCone==1){ 
          TRACK* tmpTrack = new TRACK();
          tmpTrack->makePrivateStore( track );
          outputTracks->push_back(tmpTrack);
	}
	*/
	
	//Cut 3. Isolation: require ptcone{IsoCut}/track_pt < ptrelCut
	double ptcone=0.;
	TRACKCONTAINER::const_iterator trackItr2 = inputTracks->begin();
	TRACKCONTAINER::const_iterator trackEnd2 = inputTracks->end();
	for(; trackItr2 != trackEnd2; ++trackItr2 ){
	  if(trackItr==trackItr2) continue;
	  const TRACK* track2 = *trackItr2;
	  double dif_eta = track->eta() - track2->eta();
	  double dif_phi = track->phi() - track2->phi();
	  if(dif_phi<0) dif_phi=-dif_phi;
	  if(dif_phi>M_PI){dif_phi=2*M_PI-dif_phi;}
	  double deltaR = sqrt(dif_eta*dif_eta+dif_phi*dif_phi);
	  if(deltaR<m_trackIso) ptcone += track2->pt();
	}
	if(ptcone/track->pt() < m_trackPtrel ){ 
          TRACK* tmpTrack = new TRACK();
          tmpTrack->makePrivateStore( track );
          outputTracks->push_back(tmpTrack);
	}
      }
    }
  }
  
  ATH_MSG_DEBUG("Number of selected tracks: " << outputTracks->size());

  //Get input clusters
  const CLUSTERCONTAINER* inputClusters = 0;
  CHECK( evtStore()->retrieve( inputClusters, m_inputClusters ) );

  //Allocate output clusters container
  xAOD::CaloClusterContainer* outputClusters = new xAOD::CaloClusterContainer;
  xAOD::CaloClusterAuxContainer* outputAuxClusters = new xAOD::CaloClusterAuxContainer;
  outputClusters->setStore( outputAuxClusters );


  //Allocate output association between tracks and clusters  
  ASSOCCONTAINER* trackClusters = new ASSOCCONTAINER_CONSTRUCTOR(outputTracks->size());
  CHECK( m_storeGate->record( trackClusters, m_trackClusters ) );

  //Select clusters that match a track
  //For each cluster loop over all selected tracks
  //extrapolate the track to each calorimeter layer
  //compute distance of cluster center in given layer to track pointer in same layer
  //select cluster if distance is smaller than threshold
  CLUSTERCONTAINER::const_iterator clusterItr = inputClusters->begin();
  CLUSTERCONTAINER::const_iterator clusterEnd = inputClusters->end();
  for(;clusterItr != clusterEnd; ++clusterItr){
    const CLUSTER* cluster = *clusterItr;
    TRACKCONTAINER::const_iterator trackItr = outputTracks->begin();
    TRACKCONTAINER::const_iterator trackEnd = outputTracks->end();
    ASSOCCONTAINER::iterator assocItr = trackClusters->begin();
    for( ; trackItr != trackEnd; ++trackItr ){
      const TRACK* track = *trackItr;
      vector< vector<double> > etrack = m_trackInCalo->getXYZEtaPhiPerLayer(track);
      double deltaR=999;
      for(int cell_sid=0;cell_sid<CaloSampling::Unknown;cell_sid++){
	int lay=-1;
	if     (cell_sid==0  || cell_sid==4 ){lay=0;}
	else if(cell_sid==1  || cell_sid==5 ){lay=1;}
	else if(cell_sid==2  || cell_sid==6 ){lay=2;}
	else if(cell_sid==3  || cell_sid==7 ){lay=3;}
	else if(cell_sid==12 || cell_sid==18){lay=4;}
	else if(cell_sid==13 || cell_sid==19){lay=5;}
	else if(cell_sid==15 || cell_sid==17){lay=5;}
	else if(cell_sid==14 || cell_sid==20){lay=6;}
	else if(cell_sid==16)                {lay=6;}
	else if(cell_sid==8                 ){lay=7;}
	else if(cell_sid==9                 ){lay=8;}
	else if(cell_sid==10                ){lay=9;}
	else if(cell_sid==11                ){lay=10;}
	if(lay==-1) continue;
	double dif_eta = cluster->etaSample((CaloSampling::CaloSample)cell_sid) - etrack[lay][3];
	double dif_phi = cluster->phiSample((CaloSampling::CaloSample)cell_sid) - etrack[lay][4];
	if(dif_phi<0) dif_phi=-dif_phi;
	if(dif_phi>M_PI){dif_phi=2*M_PI-dif_phi;}
	double tmp = sqrt(dif_eta*dif_eta+dif_phi*dif_phi);
	deltaR = (tmp<deltaR ? tmp : deltaR);
      }
      if(deltaR<=m_trackClusterR){
	unsigned int i=0;
	while(i!=outputClusters->size()){if(outputClusters->at(i)==cluster){break;}i++;}
	if(i==outputClusters->size()){
          CLUSTER* tmpCluster = new CLUSTER();
          tmpCluster->makePrivateStore( cluster );
          outputClusters->push_back(tmpCluster);
	}
	//Fill association
	assocItr->push_back(i); //FIXME
      }
      ++assocItr;
    } 
  }

  ATH_MSG_DEBUG("Number of selected clusters: " << outputClusters->size());
  
  //Get input cells
  const CELLCONTAINER* inputCells = 0;
  CHECK( evtStore()->retrieve( inputCells, m_inputCells ) );

  //Allocate output cells container
  ConstDataVector<CELLCONTAINER>* outputCells = new ConstDataVector<CELLCONTAINER>( SG::VIEW_ELEMENTS );
  CHECK( m_storeGate->record( outputCells, m_outputCells ) );

  //Allocate output association between clusters and cells  
  ASSOCCONTAINER* clusterCells = new ASSOCCONTAINER_CONSTRUCTOR(outputClusters->size());
  CHECK( m_storeGate->record( clusterCells, m_clusterCells ) );

  //Select cells associated to clusters
  clusterItr = outputClusters->begin();
  clusterEnd = outputClusters->end();
  ASSOCCONTAINER::iterator assocItr = clusterCells->begin();
  //Loop over clusters
  for(;clusterItr != clusterEnd; ++clusterItr){
    const CLUSTER* cluster = *clusterItr;
    if(cluster->getCellLinks() != 0){
      CLUSTER::const_cell_iterator cellItr = cluster->cell_begin(); 
      CLUSTER::const_cell_iterator cellEnd = cluster->cell_end(); 
      for(;cellItr != cellEnd; ++cellItr){
	const CELL* cell = *cellItr;
   	unsigned int i=0;
	while(i!=outputCells->size()){if(outputCells->at(i)==cell){break;}i++;}
	if(i==outputCells->size()){
	  outputCells->push_back(cell);
	}
	//Fill association
	assocItr->push_back(i);//FIXME
      }
    }
    ++assocItr;
  }
  
  //Allocate output association between tracks and cells  
  ASSOCCONTAINER* trackCells = new ASSOCCONTAINER_CONSTRUCTOR(outputTracks->size());
  CHECK( m_storeGate->record( trackCells, m_trackCells ) );

  //Also select cells associated to tracks 
  //cell matches track in a cone of DeltaR and is not in ouputCells already
  trackItr = outputTracks->begin();
  trackEnd = outputTracks->end();
  assocItr = trackCells->begin();
  //Loop over tracks
  for( ; trackItr != trackEnd; ++trackItr ){
    const TRACK* track = *trackItr;
    vector< vector<double> > etrack = m_trackInCalo->getXYZEtaPhiPerLayer(track);
    //loop cells
    CELLCONTAINER::const_iterator cellItr = inputCells->begin(); 
    CELLCONTAINER::const_iterator cellEnd = inputCells->end(); 
    for(;cellItr != cellEnd; ++cellItr){
      const CELL* cell = *cellItr;
      CaloSampling::CaloSample cell_sid = (CaloSampling::CaloSample)cell->caloDDE()->getSampling();
      //http://acode-browser.usatlas.bnl.gov/lxr/source/atlas/Calorimeter/CaloGeoHelpers/CaloGeoHelpers/CaloSampling.def
      int lay=-1;
      if     (cell_sid==0  || cell_sid==4 ){lay=0;}
      else if(cell_sid==1  || cell_sid==5 ){lay=1;}
      else if(cell_sid==2  || cell_sid==6 ){lay=2;}
      else if(cell_sid==3  || cell_sid==7 ){lay=3;}
      else if(cell_sid==12 || cell_sid==18){lay=4;}
      else if(cell_sid==13 || cell_sid==19){lay=5;}
      else if(cell_sid==15 || cell_sid==17){lay=5;}
      else if(cell_sid==14 || cell_sid==20){lay=6;}
      else if(cell_sid==16)                {lay=6;}
      else if(cell_sid==8                 ){lay=7;}
      else if(cell_sid==9                 ){lay=8;}
      else if(cell_sid==10                ){lay=9;}
      else if(cell_sid==11                ){lay=10;}
      if(lay==-1) continue;
      double dif_eta = cell->eta() - etrack[lay][3];
      double dif_phi = cell->phi() - etrack[lay][4];
      if(dif_phi<0) dif_phi=-dif_phi;
      if(dif_phi>M_PI){dif_phi=2*M_PI-dif_phi;}
      double deltaR = sqrt(dif_eta*dif_eta+dif_phi*dif_phi);
      if(deltaR<=m_trackCellR){ 
	unsigned int i=0;
	while(i!=outputCells->size()){if(outputCells->at(i)==cell){break;}i++;}
	if(i==outputCells->size()){
	  outputCells->push_back(cell);
	}
	assocItr->push_back(i);//FIXME
      }
    }
    ++assocItr;
  }

  ATH_MSG_DEBUG("Number of selected cells: " << outputCells->size());

  CHECK( evtStore()->record(outputClusters, m_outputClusters ) );
  CHECK( evtStore()->record(outputAuxClusters, m_outputClusters+"Aux.") );
  CHECK( evtStore()->record(outputTracks, m_outputTracks) );
  CHECK( evtStore()->record(outputAuxTracks, m_outputTracks+"Aux.") );


  return StatusCode::SUCCESS;

}

