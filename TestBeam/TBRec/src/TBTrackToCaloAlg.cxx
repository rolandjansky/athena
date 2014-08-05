/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// TBTrackToCaloAlg.cxx
//   Implementation file for class TBTrackToCaloAlg
///////////////////////////////////////////////////////////////////

#include "TBRec/TBTrackToCaloAlg.h"
#include "TBRec/TBExtrapolTrackToCaloTool.h"

// Gaudi includes
#include "GaudiKernel/MsgStream.h"
#include "StoreGate/StoreGate.h"
#include "GaudiKernel/AlgFactory.h"
#include "StoreGate/StoreGateSvc.h"
#include "GaudiKernel/IToolSvc.h"

// Tracking includes
#include "TrkParameters/TrackParameters.h"
//#include "TrkParameters/MeasuredPerigee.h"
//#include "TrkParameters/Perigee.h"
#include <vector>
#include "Particle/TrackParticleContainer.h"
#include "Particle/TrackParticle.h"

#include "TrackToCalo/ImpactInCalo.h"

#include "CaloEvent/CaloCluster.h"
#include "CaloEvent/CaloClusterContainer.h"
#include "CaloDetDescr/CaloDetDescrManager.h"
#include "CaloGeoHelpers/CaloPhiRange.h"
#include "CaloUtils/CaloCellList.h"
#include "CaloEvent/CaloCell.h"
#include "CaloEvent/CaloCellContainer.h"
#include "CaloDetDescr/ICaloCoordinateTool.h"
#include "CaloDetDescr/CaloDepthTool.h"

//#include "ITrackToCalo/IExtrapolTrackToCaloTool.h"
#include "RecoToolInterfaces/IExtrapolateToCaloTool.h"
#include "TrackToCalo/ImpactInCaloCollection.h"


using HepGeom::Point3D;
using HepGeom::Vector3D;


// Constructor with parameters:
TBTrackToCaloAlg::TBTrackToCaloAlg(const std::string &name, 
                                   ISvcLocator *pSvcLocator) :
  Algorithm(name,pSvcLocator),
  m_StoreGate(0),
  m_TrackName("Tracks"),
  m_tracks(0)
{  
  // Get parameter values from jobOptions file
  declareProperty("TrackName", m_TrackName);
  declareProperty("TrackParticleName", m_TrackParticleName);
  declareProperty("ClusterContainerName", m_cluster_container);
  declareProperty("CaloCellContainerName", m_cell_container);
  declareProperty("ImpactInCaloContainerName", m_ImpactInCalosOutputName);
  declareProperty("TrackInputType", m_trkinput);

  m_calo_dd = 0;
  m_calo_id = 0;
  m_calo_tb_coord = 0;
  m_calodepth = 0;
  m_particle = 0;
  m_toCalo = 0;
}

// Initialize method:
StatusCode TBTrackToCaloAlg::initialize()
{

  // Get the messaging service, print where you are
  MsgStream log(msgSvc(), name());
  log << MSG::DEBUG << "TBTrackToCaloAlg::initialize()" << endreq;

  StatusCode sc;

  // Get an Identifier helper object
  StoreGateSvc* detStore(0);
  sc = service("DetectorStore", detStore);
  if (sc.isFailure()) {
    log << MSG::ERROR << "Detector service not found !" << endreq;
    return StatusCode::FAILURE;
  } 

  sc=service("StoreGateSvc",m_StoreGate);
  if (sc.isFailure()) {
    log << MSG::FATAL << "StoreGate service not found !" << endreq;
    return StatusCode::FAILURE;
  } 

  m_calo_dd = CaloDetDescrManager::instance();
  m_calo_id = m_calo_dd->getCaloCell_ID();
  m_phiRange.print();

  // General access to Tools :
  IToolSvc* p_toolSvc = 0;
  sc = service("ToolSvc", p_toolSvc);
  if (sc.isFailure()) {
    log << MSG::ERROR << "Cannot find ToolSvc ??? " << endreq;
    return StatusCode::FAILURE; 
  }

  IAlgTool* algTool = 0;

  sc = p_toolSvc->retrieveTool("ExtrapolTrackToCaloTool", algTool,this);
  //m_toCalo=dynamic_cast<IExtrapolTrackToCaloTool*>(algTool); 
  //m_toCalo=dynamic_cast<IExtrapolateToCaloTool*>(algTool); 
  m_toCalo=dynamic_cast<TBExtrapolTrackToCaloTool*>(algTool); 
  if(sc.isFailure() || !m_toCalo) {
    log << MSG::ERROR << "Cannot get ExtrapolTrackToCaloTool" << endreq;
    return StatusCode::FAILURE;
  }
  
  IAlgTool* tool;
  sc = p_toolSvc->retrieveTool("TBCaloCoordinate", tool);
  if(sc.isFailure()) {
    log << MSG::ERROR << "Cannot get TBCaloCoordinate tool" << endreq;
    return StatusCode::FAILURE;
  }
  m_calo_tb_coord = dynamic_cast<ICaloCoordinateTool*>(tool);

  // retrived via the Extrapolator to make sure that jobOpt setting is consistent.
  m_calodepth = m_toCalo->getCaloDepth();
  if (!m_calodepth) {
    log << MSG::ERROR << "Cannot get CaloDepthTool" << endreq;
    return StatusCode::FAILURE; 
  }

  log << MSG::INFO << "TBTrackToCaloAlg initialisation OK" << endreq;
  return StatusCode::SUCCESS;
}

// Execute method:
StatusCode TBTrackToCaloAlg::execute() 
{
  // Get the messaging service, print where you are
  StatusCode sc;
  MsgStream log(msgSvc(), name());
  log << MSG::DEBUG << "TBTrackToCaloAlg::execute()" << endreq;

  // just a simple test ( trick to trigger position reading )
  // PrintBeamPosition();

  // Create the impacts :
  /*found_a_track =*/ CreateTrkImpactInCalo();
  
  // Example 1 : you want to read them and compare to clusters

  /*
    if (found_a_track) {
    PrintImpact();
    //CompareImpactWithCluster();
    } 
  */

  // Example 2 : you want to know the list of cells crossed by the track
  // bool found_cells = PrintCellsCrossed();

  return StatusCode::SUCCESS;
}

// Finalize method:
StatusCode TBTrackToCaloAlg::finalize() 
{
  MsgStream log(msgSvc(), name());
  log << MSG::DEBUG << "TBTrackToCaloAlg::finalize()" << endreq;
  
  return StatusCode::SUCCESS;
}


// Loop on Tracks/TrackParticles and create ImpactInCaloCollection
bool TBTrackToCaloAlg::CreateTrkImpactInCalo()
{
  bool got_a_track = false;
  
  StatusCode sc;
  MsgStream log(msgSvc(), name());
  log << MSG::DEBUG << "TBTrackToCaloAlg::CreateTrkImpactInCalo()" << endreq;

  //create and record new ImpactInCalo container
  ImpactInCaloCollection* outputContainer = new ImpactInCaloCollection();
  sc=m_StoreGate->record(outputContainer,m_ImpactInCalosOutputName,false); //"false" indicates it can't be modified by another algorithm
  if(sc != StatusCode::SUCCESS){
    log << MSG::ERROR << " Could not record ImpactInCaloCollection" 
	      << m_ImpactInCalosOutputName << endreq ;
    return got_a_track ;
  }

  m_particle = 0;
  m_tracks = 0;

  if( m_trkinput == "TrackParticleCandidates")
    {
      if (m_TrackParticleName == "") {
	log << MSG::ERROR <<"m_TrackParticleName not set" << endreq;
	return StatusCode::SUCCESS;
      }

      sc = m_StoreGate->retrieve(m_particle, m_TrackParticleName);

      if (sc.isFailure()) log << MSG::ERROR <<"TrackParticle not found: will only play with calo " << m_TrackParticleName << endreq;
      else {
	log <<MSG::DEBUG <<"TrackParticle found in StoreGate" <<endreq; 
	for (Rec::TrackParticleContainer::const_iterator itr = (*m_particle).begin(); itr < (*m_particle).end(); itr++) {
	  const Trk::Track* tr = (*itr)->originalTrack();

	  ImpactInCalo * imp = GetImpactsInCalo(tr, got_a_track);	  
	  if(imp) outputContainer->push_back(imp);
	  else log <<MSG::DEBUG <<" ImpactInCalo pointer not valid for this track"<<endreq;
	}
      }
    }

  else
    {
      if (m_TrackName == "") {
	log << MSG::ERROR <<"m_TrackName not set" << endreq;
	return StatusCode::SUCCESS;
      }

      sc = m_StoreGate->retrieve(m_tracks, m_TrackName);

      if (sc.isFailure()) log << MSG::ERROR <<"Tracks not found: will only play with calo " << m_TrackName << endreq;
      else {
	log <<MSG::DEBUG <<"Tracks found in StoreGate" <<endreq; 
	for (TrackCollection::const_iterator itr = (*m_tracks).begin(); itr < (*m_tracks).end(); itr++) {

	  ImpactInCalo * imp = GetImpactsInCalo(*itr, got_a_track);
	  if(imp) outputContainer->push_back(imp);
	  else log <<MSG::DEBUG <<" ImpactInCalo pointer not valid for this track"<<endreq;
	}
      }
    }

  return  got_a_track;
}

ImpactInCalo* TBTrackToCaloAlg::GetImpactsInCalo(const Trk::Track* track, bool& got_a_track)
{
  MsgStream log(msgSvc(), name());

  Amg::Vector3D* pt_calo_ctb = new Amg::Vector3D; 
  Amg::Vector3D* pt_calo_local = new Amg::Vector3D; 

  double trketa = 0.;
  // take the last measured point to find out if I am in barrel or endcap :
  const DataVector <const Trk::TrackParameters>* paramvec = track->trackParameters();
  if (paramvec) { 
    DataVector <const Trk::TrackParameters>::const_iterator it = paramvec->begin();
    DataVector <const Trk::TrackParameters>::const_iterator itEnd = paramvec->end();
    for (;it!=itEnd; it++) 
      trketa = (*it)->eta();
  } 
  else
    log << MSG::ERROR << "  No track parameters for this track ??? do nothing " << endreq;
  
  double distbar = 0.;
  double distec = 0.;
  double offset = 0.;   
  double trketa_at = 0.;
  double trkphi_at = 0.;
  bool result = false;
  CaloCell_ID::CaloSample sample;
  
  // PreSampler :
  double x_ctb_0 = 0;
  double y_ctb_0 = 0;
  double z_ctb_0 = 0;
  double etaCaloLocal_0 = 0; 
  double phiCaloLocal_0 = 0;
  double trketa_at_0 = 0.;
  double trkphi_at_0 = 0.;
  // Strip :
  double x_ctb_1 = 0;
  double y_ctb_1 = 0;
  double z_ctb_1 = 0;
  double etaCaloLocal_1 = 0; 
  double phiCaloLocal_1 = 0;
  double trketa_at_1 = 0.;
  double trkphi_at_1 = 0.;
  // Middle :
  double x_ctb_2 = 0;
  double y_ctb_2 = 0;
  double z_ctb_2 = 0;
  double etaCaloLocal_2 = 0; 
  double phiCaloLocal_2 = 0;
  double trketa_at_2 = 0.;
  double trkphi_at_2 = 0.;
  // Back :
  double x_ctb_3 = 0;
  double y_ctb_3 = 0;
  double z_ctb_3 = 0;
  double etaCaloLocal_3 = 0; 
  double phiCaloLocal_3 = 0;
  double trketa_at_3 = 0.;
  double trkphi_at_3 = 0.;
  // Tiles : 
  double x_ctb_tile = 0;
  double y_ctb_tile = 0;
  double z_ctb_tile = 0;
  double etaCaloLocal_tile = 0; 
  double phiCaloLocal_tile = 0;
  double trketa_at_tile = 0.;
  double trkphi_at_tile = 0.;
  
  if (paramvec) { 
    
    // PS :
    distbar = m_calodepth->deta(CaloCell_ID::PreSamplerB,trketa);
    distec = m_calodepth->deta(CaloCell_ID::PreSamplerE,trketa);
    
    log << MSG::DEBUG << " TrackTo ...PS : for eta= " << trketa << " dist to Barrel =" << distbar 
	<< " to endcap =" << distec << endreq;
    if (distbar < 0 )  sample = CaloCell_ID::PreSamplerB;
    else if (distec < 0 ) sample = CaloCell_ID::PreSamplerE;
    else if ( distbar < distec) sample = CaloCell_ID::PreSamplerB;
    else sample = CaloCell_ID::PreSamplerE;
    log << MSG::DEBUG << "  => will shoot in sample " << sample << endreq;
    
    result = m_toCalo->TrackSeenByCalo(track,sample,offset,
				       pt_calo_ctb,pt_calo_local,trketa_at,trkphi_at);
    
    if (result) {
      got_a_track=true;
      x_ctb_0 = pt_calo_ctb->x();
      y_ctb_0 = pt_calo_ctb->y();
      z_ctb_0 = pt_calo_ctb->z();
      etaCaloLocal_0 = pt_calo_local->eta(); 
      phiCaloLocal_0 = pt_calo_local->phi();
      trketa_at_0 = trketa_at;
      trkphi_at_0 = trkphi_at;
      
      log << MSG::DEBUG << "Extrapolation to PreSamplerB gives: " 
	  << " etaCaloLocal=" << pt_calo_local->eta()
	  << " phiCaloLocal=" << pt_calo_local->phi() <<  endreq;
    }
    
    // strip :
    distbar = m_calodepth->deta(CaloCell_ID::EMB1,trketa);
    distec = m_calodepth->deta(CaloCell_ID::EME1,trketa);
    
    log << MSG::DEBUG << " TrackTo ...Strip : for eta= " << trketa << " dist to Barrel =" << distbar 
	<< " to endcap =" << distec << endreq;
    if (distbar < 0 ) sample = CaloCell_ID::EMB1;
    else if (distec < 0 ) sample = CaloCell_ID::EME1;
    else if ( distbar < distec) sample = CaloCell_ID::EMB1;
    else sample = CaloCell_ID::EME1;
    log << MSG::DEBUG << "  => will shoot in sample " << sample << endreq;
    
    result = m_toCalo->TrackSeenByCalo(track,sample,offset,
				       pt_calo_ctb,pt_calo_local,trketa_at,trkphi_at);
    
    if (result) {
      got_a_track=true;
      x_ctb_1 = pt_calo_ctb->x();
      y_ctb_1 = pt_calo_ctb->y();
      z_ctb_1 = pt_calo_ctb->z();
      etaCaloLocal_1 = pt_calo_local->eta(); 
      phiCaloLocal_1 = pt_calo_local->phi();
      trketa_at_1 = trketa_at;
      trkphi_at_1 = trkphi_at;
      
      log << MSG::DEBUG << "Extrapolation to EMB1 gives: " 
	  << " etaCaloLocal=" << pt_calo_local->eta()
	  << " phiCaloLocal=" << pt_calo_local->phi() <<  endreq;
    }
    
    
    // middle :
    distbar = m_calodepth->deta(CaloCell_ID::EMB2,trketa);
    distec = m_calodepth->deta(CaloCell_ID::EME2,trketa);
    
    log << MSG::DEBUG << " TrackTo ...Middle : for eta= " << trketa << " dist to Barrel =" << distbar 
	<< " to endcap =" << distec << endreq;
    if (distbar < 0 ) sample = CaloCell_ID::EMB2;
    else if (distec < 0 ) sample = CaloCell_ID::EME2;
    else if ( distbar < distec) sample = CaloCell_ID::EMB2;
    else sample = CaloCell_ID::EME2;
    log << MSG::DEBUG << "  => will shoot in sample " << sample << endreq;
    
    result = m_toCalo->TrackSeenByCalo(track,sample,offset,
				       pt_calo_ctb,pt_calo_local,trketa_at,trkphi_at);
    
    if (result) {
      got_a_track=true;
      x_ctb_2 = pt_calo_ctb->x();
      y_ctb_2 = pt_calo_ctb->y();
      z_ctb_2 = pt_calo_ctb->z();
      etaCaloLocal_2 = pt_calo_local->eta(); 
      phiCaloLocal_2 = pt_calo_local->phi();
      trketa_at_2 = trketa_at;
      trkphi_at_2 = trkphi_at;
      
      log << MSG::DEBUG << "Extrapolation to EMB2 gives: " 
	  << " etaCaloLocal=" << pt_calo_local->eta()
	  << " phiCaloLocal=" << pt_calo_local->phi() <<  endreq;
    }
    
    // Back :
    //
    // there is a real gap between 1.35 (end of EMB3) and 1.5 (start of EMEC3)
    // => for TrackToCalo chooses the closest. One could choose to stay in 
    //    barrel as long as not in endcap
    // In both cases, m_calodepth will see that you are outside the accpetance and
    // will use the backup solution (egparametrisation right now, rel 10.0.2).
    //
    
    distbar = m_calodepth->deta(CaloCell_ID::EMB3,trketa);
    distec = m_calodepth->deta(CaloCell_ID::EME3,trketa);
    
    log << MSG::DEBUG << " TrackTo ...Back : for eta= " << trketa << " dist to Barrel =" << distbar 
	<< " to endcap =" << distec << endreq;
    if (distbar < 0 ) sample = CaloCell_ID::EMB3;
    else if (distec < 0 ) sample = CaloCell_ID::EME3;
    else if ( distbar < distec) sample = CaloCell_ID::EMB3;
    else sample = CaloCell_ID::EME3;
    log << MSG::DEBUG << "  => will shoot in sample " << sample << endreq;
    
    result = m_toCalo->TrackSeenByCalo(track,sample,offset,
				       pt_calo_ctb,pt_calo_local,trketa_at,trkphi_at);
    
    if (result) {
      got_a_track=true;
      x_ctb_3 = pt_calo_ctb->x();
      y_ctb_3 = pt_calo_ctb->y();
      z_ctb_3 = pt_calo_ctb->z();
      etaCaloLocal_3 = pt_calo_local->eta(); 
      phiCaloLocal_3 = pt_calo_local->phi();
      trketa_at_3 = trketa_at;
      trkphi_at_3 = trkphi_at;
      
      log << MSG::DEBUG << "Extrapolation to EMB3 gives: " 
	  << " etaCaloLocal=" << pt_calo_local->eta()
	  << " phiCaloLocal=" << pt_calo_local->phi() <<  endreq;
    }
    
    // Tile or HEC0 :
    distbar = m_calodepth->deta(CaloCell_ID::TileBar0,trketa);
    distec = m_calodepth->deta(CaloCell_ID::HEC0,trketa);
    
    log << MSG::DEBUG << " TrackTo ...Tile : for eta= " << trketa << " dist to Barrel =" << distbar 
	<< " to endcap =" << distec << endreq;
    if (distbar < 0 ) sample = CaloCell_ID::TileBar0;
    else if (distec < 0 ) sample = CaloCell_ID::HEC0;
    else if ( distbar > distec && distec < 10. ) sample = CaloCell_ID::HEC0;
    else sample = CaloCell_ID::TileBar0;
    log << MSG::DEBUG << "  => will shoot in sample " << sample << endreq;
    
    result = m_toCalo->TrackSeenByCalo(track,sample,offset,
				       pt_calo_ctb,pt_calo_local,trketa_at,trkphi_at);
    
    if (result) {
      got_a_track=true;
      x_ctb_tile = pt_calo_ctb->x();
      y_ctb_tile = pt_calo_ctb->y();
      z_ctb_tile = pt_calo_ctb->z();
      etaCaloLocal_tile = pt_calo_local->eta(); 
      phiCaloLocal_tile = pt_calo_local->phi();
      trketa_at_tile = trketa_at;
      trkphi_at_tile = trkphi_at;
      
      log << MSG::DEBUG << "Extrapolation to TileBar0 gives: " 
	  << " etaCaloLocal=" << pt_calo_local->eta()
	  << " phiCaloLocal=" << pt_calo_local->phi() <<  endreq;
    }
  }
  
  ImpactInCalo * imp= new ImpactInCalo(x_ctb_0, y_ctb_0, z_ctb_0, etaCaloLocal_0, phiCaloLocal_0, trketa_at_0, trkphi_at_0,
				       x_ctb_1, y_ctb_1, z_ctb_1, etaCaloLocal_1, phiCaloLocal_1, trketa_at_1, trkphi_at_1,
				       x_ctb_2, y_ctb_2, z_ctb_2, etaCaloLocal_2, phiCaloLocal_2, trketa_at_2, trkphi_at_2,
				       x_ctb_3, y_ctb_3, z_ctb_3, etaCaloLocal_3, phiCaloLocal_3, trketa_at_3, trkphi_at_3,
				       x_ctb_tile, y_ctb_tile, z_ctb_tile, etaCaloLocal_tile, phiCaloLocal_tile, trketa_at_tile, trkphi_at_tile);
  
  if(pt_calo_ctb) delete pt_calo_ctb;
  if(pt_calo_local) delete pt_calo_local;
  
  return imp;
}




// Retreive ImpactInCaloCollection and compare to CaloClusters
void TBTrackToCaloAlg::CompareImpactWithCluster()
{
  StatusCode sc1, sc2;

  MsgStream log(msgSvc(), name());
  log << MSG::DEBUG << "TBTrackToCaloAlg::CompareImpactWithCluster()" << endreq;

  // loop on clusters
  
  const CaloClusterContainer* cluster_container;
  sc1=m_StoreGate->retrieve(cluster_container,m_cluster_container);
  
  const ImpactInCaloCollection* impact_collection;
  sc2=m_StoreGate->retrieve(impact_collection,m_ImpactInCalosOutputName);
  
  if(sc1 == StatusCode::SUCCESS && sc2 == StatusCode::SUCCESS ){

    typedef CaloClusterContainer::const_iterator cluster_iterator;
    cluster_iterator f_clu =cluster_container->begin();
    cluster_iterator l_clu = cluster_container->end();

    typedef ImpactInCaloCollection::const_iterator impact_iterator;
    impact_iterator f_imp = impact_collection->begin();
    impact_iterator l_imp = impact_collection->end();

    for ( ; f_clu!=l_clu; f_clu++){ 
      const CaloCluster* cluster = (*f_clu);
      double hecluster = cluster->energy()/1000.;
      double heta = cluster->eta();
      double hphi = cluster->phi();

      log << MSG::INFO << "Found a cluster : E= " << hecluster 
	  << "(GeV), etaCaloLocal=" <<  heta 
	  << ", phiCaloLocal=" << hphi <<endreq;
      
      for ( ; f_imp!=l_imp; f_imp++){ 
	const ImpactInCalo* impact = (*f_imp);
		
	log << MSG::INFO 
	    << "==> Comparison between cluster and impact in Middle : deta=" 
	    << heta-impact->etaCaloLocal_2()
	    << " , dphi=" << hphi-impact->phiCaloLocal_2() <<endreq;
      }
    }
  }
  else {
    if (sc1 != StatusCode::SUCCESS)
      log << MSG::ERROR <<"Clusters not found" 
	  << m_cluster_container << endreq;
    if (sc2 != StatusCode::SUCCESS)
      log << MSG::ERROR <<"ImpactInCalos not found" 
	  << m_ImpactInCalosOutputName << endreq;
  }
}

// Retreive ImpactInCaloCollection and print content
void TBTrackToCaloAlg::PrintImpact()
{
  StatusCode sc;

  MsgStream log(msgSvc(), name());

  log << MSG::INFO << " Method PrintImpacts : " << endreq;

  log << MSG::INFO << " " << endreq;
  log << MSG::INFO << " Start with Impacts : " << endreq;
  
  const ImpactInCaloCollection* impact_collection;
  sc=m_StoreGate->retrieve(impact_collection,m_ImpactInCalosOutputName);
  
  if(sc == StatusCode::SUCCESS ){

    typedef ImpactInCaloCollection::const_iterator impact_iterator;
    impact_iterator f_imp = impact_collection->begin();
    impact_iterator l_imp = impact_collection->end();

    for ( ; f_imp!=l_imp; f_imp++){ 
      const ImpactInCalo* impact = (*f_imp);

      const double impcosPhi = std::cos(impact->phiCaloLocal_1());
      const double impeta = impact->etaCaloLocal_1();

      log << MSG::INFO << "Found an impact in strips : parameters are eta = " << impeta  
	  << " cosPhi = " << impcosPhi <<  endreq;      
 
      //impact->print();
    }
  }
  else {
    log << MSG::ERROR <<"ImpactInCalos not found" 
	<< m_ImpactInCalosOutputName << endreq;
  }

  if (m_tracks) {

    log << MSG::INFO << " " << endreq;      
    log <<MSG::INFO <<" Now loop on Trk::Track collection " <<endreq; 
    log << MSG::INFO << " " << endreq;      
    
    for (TrackCollection::const_iterator itr  = 
	   (*m_tracks).begin(); itr < (*m_tracks).end(); itr++){
      const Trk::Perigee *aMeasPer=
	dynamic_cast<const Trk::Perigee*>((*itr)->perigeeParameters());
      if (aMeasPer==0){
	log << MSG::ERROR << "Could not get Trk::MeasuredPerigee" << endreq;
      }
      else {	

	const double trkcosPhi = cos(aMeasPer->parameters()[Trk::phi]);
	const double trketa = aMeasPer->eta();
	
	log << MSG::INFO << "Found a Trk::Track : parameters are eta = " << trketa  
	    << " cosPhi = " << trkcosPhi <<  endreq;      	
      }
    }
  }

  if (m_particle) {
    
    log << MSG::INFO << " " << endreq;      
    log <<MSG::INFO <<" Now loop on Trk::TrackParticle collection " <<endreq; 
    log << MSG::INFO << " " << endreq;      
    
    for (Rec::TrackParticleContainer::const_iterator itr = m_particle->begin();
	 itr != m_particle->end(); ++itr )
      {
	const Trk::Perigee *aMeasPer=
	  dynamic_cast<const Trk::Perigee*>(((*itr)->originalTrack())->perigeeParameters());
	if (aMeasPer==0){
	  log << MSG::ERROR << "Could not get TrkParticle::MeasuredPerigee" << endreq;
	}
	else {	
	  
	  const double partcosPhi = cos(aMeasPer->parameters()[Trk::phi]);
	  const double parteta = aMeasPer->eta();
	  
	  log << MSG::INFO << "Found a trackparticle : parameters are eta = " << parteta  
	      << " cosPhi = " << partcosPhi <<  endreq;      
	}
      }
  }
}

// List of cells crossed by Trk::Tracks in CaloSample 
// ( neta and nphi are the total window width, e.g .5x5 )
CaloCellList* 
TBTrackToCaloAlg::CellsCrossedByTrack(const Trk::Track* trk, 
				    const CaloCell_ID::CaloSample sam, 
				    int neta, int nphi)
{
  CaloCellList* my_list = 0;

  // Retreive CaloCell's from StoreGate :  

  const CaloCellContainer* cell_container;
  StatusCode sc=m_StoreGate->retrieve(cell_container,m_cell_container);
  if ( sc != StatusCode::SUCCESS  ) return 0;

  // Where is the track shooting ?
  double offset = 0.;
  double trketa_at = 0.;
  double trkphi_at = 0.;
  Amg::Vector3D* pt_ctb = new Amg::Vector3D();
  Amg::Vector3D* pt_local = new Amg::Vector3D();
  bool result = m_toCalo->
    TrackSeenByCalo(trk,sam,offset,pt_ctb,pt_local,trketa_at,trkphi_at);

  if (!result) {
    delete pt_ctb;
    delete pt_local;
    return 0;
  }

  double eta = pt_local->eta();
  double phi = pt_local->phi();

  // CaloCellList needs both enums: subCalo and CaloSample
  CaloCell_ID::SUBCALO subcalo;
  bool barrel;
  int sampling_or_module;
  m_calo_dd->decode_sample (subcalo, barrel, sampling_or_module, sam);

  // Get the corresponding grannularities : needs to know where you are 
  //                  the easiest is to look for the CaloDetDescrElement
  const CaloDetDescrElement* dde =
     m_calo_dd->get_element(subcalo,sampling_or_module,barrel,eta,phi);

  double deta = int(neta/2)*dde->deta();
  double dphi = int(nphi/2)*dde->dphi();
  
  //std::cout << "zone is ..." << eta << " " << phi << " "
  //	      << deta << " " << dphi << " " << std::endl;
  
  // Construct the list : 
  my_list = new CaloCellList(cell_container,subcalo);
  my_list->select(eta,phi,deta,dphi, (int) sam);

  // cleanup 
  delete pt_ctb;
  delete pt_local;
  
  return my_list ;
}

bool TBTrackToCaloAlg::PrintCellsCrossed()
{

  // Get the messaging service, print where you are
  StatusCode sc;
  MsgStream log(msgSvc(), name());
  log << MSG::INFO << "TBTrackToCaloAlg::PrintCellsCrossed()" << endreq;
  
  // Here we are :
  CaloCell_ID::CaloSample sam = CaloCell_ID::EMB2;
  int neta = 5;
  int nphi = 5;
  
  // get tracks from TDS
  if (m_TrackName == "") {
    log << MSG::ERROR <<"m_TrackName not set" << endreq;
    return StatusCode::SUCCESS;
  }
  
  sc = m_StoreGate->retrieve(m_tracks, m_TrackName);
  if (sc.isFailure()){
    log << MSG::ERROR <<"Tracks not found: will only play with calo " 
	<< m_TrackName << endreq;
  }
  else{ 
    log <<MSG::DEBUG <<"Tracks found in StoreGate" <<endreq; 
    
    for (TrackCollection::const_iterator itr  = 
	   (*m_tracks).begin(); itr < (*m_tracks).end(); itr++){
      
      const Trk::Perigee *aMeasPer=
	dynamic_cast<const Trk::Perigee*>((*itr)->perigeeParameters());
      if (aMeasPer==0){
	log << MSG::ERROR << "Could not get Trk::MeasuredPerigee" << endreq;
      }
      else {
	double d0 = aMeasPer->parameters()[Trk::d0];
	double z0 = aMeasPer->parameters()[Trk::z0];
	double phi0 = aMeasPer->parameters()[Trk::phi0];
	double theta = aMeasPer->parameters()[Trk::theta];
	double qOverP = aMeasPer->parameters()[Trk::qOverP];
	log << MSG::INFO << " " << endreq;
	log << MSG::INFO << "Found a track: parameters are " << d0  << " " 
	    << z0  << " " << phi0 << " " << theta << " " << qOverP <<  endreq;
      }

      // Warning : if anything fails, CellsCrossedByTrack will
      //           simply return a null pointer
      //           if it works, it does a new CaloCellList
      //           ==> the client has to do the delete !!!!

      CaloCellList* my_list = CellsCrossedByTrack(*itr, sam, neta, nphi); 

      if (my_list) {

	for ( CaloCellList::list_iterator itr  = 
		my_list->begin(); itr < my_list->end(); itr++)
	  log << MSG::INFO << "found cell ! eta=" << (*itr)->eta() 
	      << " phi=" << (*itr)->phi() << " energy=" << (*itr)->energy() << endreq ;
	
	delete my_list;

	}
      else 
	std::cout << " :-(  extrapolation did not work" << std::endl;
    }
  }
  
  return true;
}

// A little check : where is the beam ?
void TBTrackToCaloAlg::PrintBeamPosition()
{
  // Get the messaging service, print where you are
  StatusCode sc;
  MsgStream log(msgSvc(), name());
  log << MSG::INFO << "TBTrackToCaloAlg::PrintBeamPosition()" << endreq;

  log << MSG::INFO << "Just a check... beam direction in calo : " << endreq;
  m_calo_tb_coord->read_table_position();  
  Amg::Vector3D beam (1.,0.,0.);
  beam = ( *(m_calo_tb_coord->transform_ctb_to_calo()) )* beam;
  log << MSG::INFO << " eta= "<< beam.eta() << " phi="<< beam.phi()<< endreq;
}

