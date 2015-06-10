/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

////////////////////////////////////////////////////////////////////////////////
// TrigL2PattRecoStrategyC
// -------------------------------
// ATLAS Collaboration
//
// 16.01.2011 Package created
//
// Author: Dmitry Emeliyanov, RAL
// e-mail: Dmitry.Emeliyanov@cern.ch
//
// implements offline track finding algorithm based on I.Gavrilenko's 
// InnerDetector/InDetTrigRecAlgs/SiTrigSPSeededFinder_Lv2
//
////////////////////////////////////////////////////////////////////////////////

#include <cmath>
#include <iostream>
#include <list>
//#include "GaudiKernel/ToolFactory.h"
//#include "StoreGate/DataHandle.h"

#include "TrigInDetEvent/TrigInDetTrack.h"
#include "TrigInDetEvent/TrigInDetTrackCollection.h"

#include "TrigInDetRecoTools/ITrigL2PattRecoStrategy.h"

#include "IRegionSelector/IRoiDescriptor.h"

#include "InDetRecToolInterfaces/ISiSpacePointsSeedMaker.h"
#include "InDetRecToolInterfaces/ISiZvertexMaker.h" 
#include "InDetRecToolInterfaces/ISiTrackMaker.h" 
#include "SiSpacePoint/SCT_SpacePoint.h"
#include "SiSpacePoint/PixelSpacePoint.h"

#include "TrkRIO_OnTrack/RIO_OnTrack.h"
#include "TrkSpacePoint/SpacePoint.h"  
#include "TrkSpacePoint/SpacePointCollection.h"  
#include "TrkSpacePoint/SpacePointContainer.h"  

#include "InDetIdentifier/PixelID.h" 
#include "InDetIdentifier/SCT_ID.h" 

#include "TrkSurfaces/PerigeeSurface.h" 
#include "TrkParameters/TrackParameters.h" 
#include "InDetPrepRawData/SiClusterContainer.h" 
#include "InDetPrepRawData/PixelClusterCollection.h" 
#include "InDetPrepRawData/SCT_ClusterCollection.h" 
//#include "DataModel/tools/IdentContIndex.h"

#include "TrkEventPrimitives/FitQuality.h" 
#include "TrkEventPrimitives/ParamDefs.h"
 
#include "TrigTimeAlgs/TrigTimerSvc.h"

#include "IRegionSelector/IRegSelSvc.h"
#include "TrigInDetRecoTools/TrigL2PattRecoStrategyC.h"
#include "TrigInDetRecoTools/TrigL2DataConversionUtils.h"

TrigL2PattRecoStrategyC::TrigL2PattRecoStrategyC(const std::string& t, 
						 const std::string& n,
						 const IInterface*  p ): 
  AthAlgTool(t,n,p),

  m_useZvertexTool(true),
  m_nfreeCut(5)         ,
  m_seedsmaker("InDet::SiSpacePointsSeedMaker_ATLxk/InDetTrigSiSpacePointsSeedMaker"),
  m_zvertexmaker("InDet::SiZvertexMaker_xk/InDetTrigZvertexMaker"),
  m_trackmaker("InDet::SiTrackMaker_xk/InDetTrigSiTrackMaker"),
  m_regionSelector("RegSelSvc", n),
  m_sctSpacePointsName ("SCT_TempSpacePoints"), 
  m_pixelSpacePointsName ("PixelTempSpacePoints"),
  m_sctOnlineSpacePointsName("TrigSCT_SpacePoints"), 
  m_pixelOnlineSpacePointsName("TrigPixelSpacePoints")
{
  declareInterface< ITrigL2PattRecoStrategy >( this ); 

  declareProperty("SeedsTool"         ,m_seedsmaker    );
  declareProperty("ZvertexTool"       ,m_zvertexmaker  );
  declareProperty("TrackTool"         ,m_trackmaker    );
  declareProperty("useZvertexTool"    ,m_useZvertexTool);
  declareProperty("SCT_SpacePointsName",       m_sctSpacePointsName); 
  declareProperty("PixelSpacePointsName",     m_pixelSpacePointsName); 
  declareProperty("FreeClustersCut"   ,m_nfreeCut      );
}

StatusCode TrigL2PattRecoStrategyC::initialize()
{
  StatusCode sc = AthAlgTool::initialize();
  MsgStream athenaLog(msgSvc(), name());

  // Get tool for space points seed maker

  sc=m_seedsmaker.retrieve();

  if ( sc.isFailure() ) {
    athenaLog << MSG::FATAL << "Failed to retrieve tool " << m_seedsmaker << endreq;
    return sc;
  } 
  else {
    athenaLog << MSG::INFO << "Retrieved tool " << m_seedsmaker << endreq;
  }
  
  if(m_useZvertexTool) {
    
    // Get tool for z-coordinates primary vertices search
    sc=m_zvertexmaker.retrieve();

    if ( sc.isFailure() ) {
      athenaLog << MSG::FATAL << "Failed to retrieve tool " << m_zvertexmaker << endreq;
      return sc;
    } 
    else {
      athenaLog << MSG::INFO << "Retrieved tool " << m_zvertexmaker << endreq; 
    }
  }

  // Get track-finding tool
  sc = m_trackmaker.retrieve();
  if ( sc.isFailure() ) {
    athenaLog << MSG::FATAL << "Failed to retrieve tool " << m_trackmaker << endreq;
    return sc;
  } 
  else {
    athenaLog << MSG::INFO << "Retrieved tool " << m_trackmaker << endreq;
  }

  sc = m_regionSelector.retrieve();
  if ( sc.isFailure() ) {
    athenaLog << MSG::FATAL<< "Unable to retrieve RegionSelector tool "<< m_regionSelector.type() << endreq;
    return sc;
  }

  // Get SCT & pixel Identifier helpers

  if (detStore()->retrieve(m_pixelId, "PixelID").isFailure()) { 
     athenaLog << MSG::FATAL << "Could not get Pixel ID helper" << endreq;
     return StatusCode::FAILURE;
  }  

  if (detStore()->retrieve(m_sctId, "SCT_ID").isFailure()) {
    athenaLog << MSG::FATAL << "Could not get SCT ID helper" << endreq;
    return StatusCode::FAILURE;  
  }
  // register the IdentifiableContainers into StoreGate 
  // ------------------------------------------------------ 
  
  if( !evtStore()->transientContains<SpacePointContainer>(m_pixelSpacePointsName)){ 
  
    m_pixelSpacePointsContainer = new SpacePointContainer(m_pixelId->wafer_hash_max());  
    m_pixelSpacePointsContainer->addRef();  //preventing SG from deleting object: 

     if (evtStore()->record(m_pixelSpacePointsContainer, m_pixelSpacePointsName).isFailure()) { 
       athenaLog << MSG::WARNING << " Container " << m_pixelSpacePointsName
             << " could not be recorded in StoreGate !" << endreq; 
     }  
     else { 
       athenaLog << MSG::INFO << "Container " << m_pixelSpacePointsName 
             << " registered  in StoreGate" << endreq;   
     } 
  }
  else {     
    sc = evtStore()->retrieve(m_pixelSpacePointsContainer, m_pixelSpacePointsName); 
    if (sc.isFailure()) { 
      athenaLog << MSG::ERROR << "Failed to get Pixel space point Container" << endreq; 
      return sc; 
    } 
    else {  
      m_pixelSpacePointsContainer->addRef();  //increase ref count: 
      athenaLog << MSG::INFO << "Got Pixel space point Container from TDS: " 
		<< m_pixelSpacePointsName << endreq; 
    } 
  }
  
  if( !evtStore()->transientContains<SpacePointContainer>(m_sctSpacePointsName)){ 
    m_sctSpacePointsContainer = new SpacePointContainer(m_sctId->wafer_hash_max());  
    m_sctSpacePointsContainer->addRef();  //preventing SG from deleting object: 

    if (evtStore()->record(m_sctSpacePointsContainer, m_sctSpacePointsName).isFailure()) { 
       athenaLog << MSG::WARNING << " Container " << m_sctSpacePointsName
		 << " could not be recorded in StoreGate !" << endreq; 
    }  
    else { 
      athenaLog << MSG::INFO << "Container " << m_sctSpacePointsName 
		<< " registered  in StoreGate" << endreq;   
    } 
  }
  else {     
    sc = evtStore()->retrieve(m_sctSpacePointsContainer, m_sctSpacePointsName); 
    if (sc.isFailure()) { 
      athenaLog << MSG::ERROR << "Failed to get SCT space point Container" << endreq; 
      return sc; 
    } 
    else {  
      m_sctSpacePointsContainer->addRef();  //increase ref count: 
      athenaLog << MSG::INFO << "Got SCT space point Container from TDS: " 
		<< m_sctSpacePointsName << endreq; 
    } 
  }

  ITrigTimerSvc* timerSvc;
  StatusCode scTime = service( "TrigTimerSvc", timerSvc);
  if( scTime.isFailure() ) {
    athenaLog << MSG::INFO<< "Unable to locate Service TrigTimerSvc " << endreq;
    m_timers = false;
  } 
  else{
    m_timers = true;  
  }
  if ( m_timers ) {
    m_timer[0] = timerSvc->addItem("StrC_PattReco");
    m_timer[1] = timerSvc->addItem("Seed_NE");
    m_timer[2] = timerSvc->addItem("Seed_Prod");
    m_timer[3] = timerSvc->addItem("Seed_Ret");
    m_timer[3]->propName("Seed_Ret.nSeed");
    m_timer[4] = timerSvc->addItem("TMak_NE"); 
    m_timer[5] = timerSvc->addItem("TMak_Get"); 
    m_timer[5]->propName("TMak_Get.nSeed");
    m_timer[6] = timerSvc->addItem("TMak_Qm");
    m_timer[6]->propName("TMak_Qm.nTracks");
    m_timer[7] = timerSvc->addItem("TMak_EE");
    m_timer[8] = timerSvc->addItem("TrackFilt");
    m_timer[8]->propName("TrackFlt.nTracks");
    m_timer[9] = timerSvc->addItem("ZVMaker");

  }

  athenaLog << MSG::INFO << "TrigL2PattRecoStrategyC initialized "<< endreq;
  return sc;
}

StatusCode TrigL2PattRecoStrategyC::finalize() {

  m_sctSpacePointsContainer->cleanup();
  m_sctSpacePointsContainer->release();
  m_pixelSpacePointsContainer->cleanup();
  m_pixelSpacePointsContainer->release();

  StatusCode sc = AthAlgTool::finalize(); 
  return sc;
}

TrigL2PattRecoStrategyC::~TrigL2PattRecoStrategyC() {

}

HLT::ErrorCode TrigL2PattRecoStrategyC::findTracks(const std::vector<const TrigSiSpacePoint*>& /*vsp*/,
						   const IRoiDescriptor& roi,
						   TrigInDetTrackCollection*  trackColl) {

  m_pattRecoTiming=0.0;
  HLT::ErrorCode rc=HLT::OK;

  m_zVertices.clear();

  MsgStream athenaLog(msgSvc(), name());
  int outputLevel = msgSvc()->outputLevel( name() );

  if (outputLevel <= MSG::DEBUG) 
    athenaLog<< MSG::DEBUG<<"TrigL2PattRecoStrategyC called in RoI-based mode "<<endreq;

  std::vector<IdentifierHash>  listOfSCTIds; 
  std::vector<IdentifierHash>  listOfPixIds; 

  m_regionSelector->DetHashIDList(SCT, roi, listOfSCTIds );

  m_regionSelector->DetHashIDList(PIXEL,roi, listOfPixIds);

  StatusCode sc=convertInputData(listOfPixIds,listOfSCTIds);
  
  if(sc.isFailure()) {
    athenaLog<< MSG::WARNING<<"Failed to convert input data into offline spacepoints "<<endreq;
    return HLT::ErrorCode(HLT::Action::ABORT_JOB, HLT::Reason::BAD_JOB_SETUP);
  }

  if(m_timers) m_timer[0]->start();

  std::list<Trk::Vertex> VZ;

  if( m_useZvertexTool ) { 

    if(m_timers) m_timer[9]->start();

    m_zvertexmaker->newRegion(listOfPixIds,listOfSCTIds,roi);
    VZ = m_zvertexmaker->getVertices();
    std::for_each(VZ.begin(),VZ.end(),ZVertexCopyFunctor(m_zVertices));
    if(m_timers) m_timer[9]->stop();
    if (outputLevel <= MSG::DEBUG) 
      athenaLog << MSG::DEBUG << "REGTEST: Number of zvertex found = " << VZ.size() << endreq;
  }
  else{ 
    if(m_timers) m_timer[1]->start();
    m_seedsmaker->newRegion(listOfPixIds,listOfSCTIds,roi);
    if(m_timers) m_timer[1]->stop(); 
  }

  if(m_timers) m_timer[2]->start();
  m_seedsmaker->find3Sp(VZ);
  if(m_timers) m_timer[2]->stop();

  bool PIX = true;
  bool SCT = true;

  if(m_timers) m_timer[4]->start();
  m_trackmaker->newTrigEvent(PIX,SCT);
  if(m_timers) m_timer[4]->stop();

  // Loop through all seeds and reconsrtucted tracks collection preparation
  //

  TrackCollection* foundTracks = new TrackCollection;
  std::multimap<double,Trk::Track*>    qualityTrack;

  m_nseeds  = 0;
  m_ntracks = 0;

  const InDet::SiSpacePointsSeed* seed = 0;

  if(m_timers) { 
    m_timer[3]->start();m_timer[3]->pause();
    m_timer[5]->start();m_timer[5]->pause();
    m_timer[6]->start();m_timer[6]->pause();
  }

  while(true) {
    if(m_timers) m_timer[3]->resume();
    seed = m_seedsmaker->next();
    if(m_timers) m_timer[3]->pause();

    if(seed==0) break;

    ++m_nseeds;
    if(m_timers) m_timer[5]->resume();
    const std::list<Trk::Track*>& T = m_trackmaker->getTracks(seed->spacePoints()); 
    if(m_timers) m_timer[5]->pause();
    if(m_timers) m_timer[6]->resume();
    for(std::list<Trk::Track*>::const_iterator t=T.begin(); t!=T.end(); ++t) {
      if((*t)) 
	{
	  ++m_ntracks;
	  qualityTrack.insert(std::make_pair(-trackQuality((*t)),(*t)));
	}
    }
    if(m_timers) m_timer[6]->pause();
  }
  if(m_timers) { 
    m_timer[3]->propVal(m_nseeds);
    m_timer[3]->stop();
    m_timer[5]->propVal(m_nseeds);
    m_timer[5]->stop();
    m_timer[6]->propVal(m_ntracks);
    m_timer[6]->stop();
  }
  

  if(m_timers) m_timer[7]->start();
  m_trackmaker->endEvent();
  if(m_timers) m_timer[7]->stop();

  // Remove shared tracks with worse quality
  //

  if(m_timers) m_timer[8]->start();
  filterSharedTracks(qualityTrack);
  m_timer[8]->propVal(m_ntracks);
  if(m_timers) m_timer[8]->stop();

  std::multimap<double,Trk::Track*>::iterator 
    q = qualityTrack.begin(), qe =qualityTrack.end(); 

  m_ntracks = 0;

  for(; q!=qe; ++q) {
    foundTracks->push_back((*q).second); ++m_ntracks; 
  }

  if(m_timers) {
    m_timer[0]->stop();
    m_pattRecoTiming=m_timer[0]->elapsed();
  }

  if (outputLevel <= MSG::DEBUG) {
    athenaLog << MSG::DEBUG << "REGTEST: Number of seeds found = " << m_nseeds << endreq;
    athenaLog << MSG::DEBUG << "REGTEST: Investigated "
	      << std::setw(7) <<  m_nseeds 
	      << " seeds and found "
	      << std::setw(5) << foundTracks->size() <<" tracks"
	      << endreq;
  }

  for (size_t i=0; i<foundTracks->size() ; i++){
    TrigInDetTrack* p = convertOutputTracks(foundTracks->at(i));
    if(p!=NULL) {
      trackColl->push_back(p);
    }
  }

  delete foundTracks;

  return rc;
}


HLT::ErrorCode TrigL2PattRecoStrategyC::findTracks(const std::vector<const TrigSiSpacePoint*>& //vsp
						   ,
						   TrigInDetTrackCollection* trackColl) {

  m_pattRecoTiming=0.0;
  m_zVertices.clear();

  MsgStream athenaLog(msgSvc(), name());
  int outputLevel = msgSvc()->outputLevel( name() );

  HLT::ErrorCode rc=HLT::OK;
  if (outputLevel <= MSG::DEBUG) 
    athenaLog<< MSG::DEBUG<<"TrigL2PattRecoStrategyC called in full-scan mode "<<endreq;

  StatusCode sc=convertInputData();

  if(sc.isFailure()) {
    athenaLog<< MSG::WARNING<<"Failed to convert input data into offline spacepoints "<<endreq;
    return HLT::ErrorCode(HLT::Action::ABORT_JOB, HLT::Reason::BAD_JOB_SETUP);
  }

  //initialize monitoring variables
  m_nseeds = 0;
  m_ntracks= 0;

  // Test is sct clusters collection for given event
  //
  bool PIX = true;
  bool SCT = true;

  std::list<Trk::Vertex> VZ; 

  if(m_timers) m_timer[0]->start();
  if( m_useZvertexTool ) {    

    if(m_timers) m_timer[9]->start();
    m_zvertexmaker->newEvent(); 
    VZ = m_zvertexmaker->getVertices();
    std::for_each(VZ.begin(),VZ.end(),ZVertexCopyFunctor(m_zVertices));
    if(m_timers) m_timer[9]->stop();   
    if (outputLevel <= MSG::DEBUG) 
      athenaLog << MSG::DEBUG << "REGTEST: Number of zvertex found = " << VZ.size() << endreq;
  }
  else{
    if(m_timers) m_timer[1]->start();
    m_seedsmaker->newEvent();    
    if(m_timers) m_timer[1]->stop();
  }

  if(m_timers) m_timer[2]->start();
  m_seedsmaker->find3Sp(VZ);
  if(m_timers) m_timer[2]->stop();

  if(m_timers) m_timer[4]->start();
  m_trackmaker->newTrigEvent(PIX,SCT);
  if(m_timers) m_timer[4]->stop();

  // Loop through all seeds and reconsrtucted tracks collection preparation
  //

  TrackCollection* foundTracks = new TrackCollection;
  std::multimap<double,Trk::Track*>    qualityTrack;

  m_nseeds  = 0;
  m_ntracks = 0;

  const InDet::SiSpacePointsSeed* seed = 0;

  if(m_timers) { 
    m_timer[3]->start();m_timer[3]->pause();
    m_timer[5]->start();m_timer[5]->pause();
    m_timer[6]->start();m_timer[6]->pause();
  }

  while(true) {
    if(m_timers) m_timer[3]->resume();
    seed = m_seedsmaker->next();
    if(m_timers) m_timer[3]->pause();

    if(seed==0) break;

    ++m_nseeds;
    if(m_timers) m_timer[5]->resume();
    const std::list<Trk::Track*>& T = m_trackmaker->getTracks(seed->spacePoints()); 
    if(m_timers) m_timer[5]->pause();
    if(m_timers) m_timer[6]->resume();
    for(std::list<Trk::Track*>::const_iterator t=T.begin(); t!=T.end(); ++t) {
      if((*t)) 
	{
	  ++m_ntracks;
	  qualityTrack.insert(std::make_pair(-trackQuality((*t)),(*t)));
	}
    }
    if(m_timers) m_timer[6]->pause();
  }
  if(m_timers) { 
    m_timer[3]->propVal(m_nseeds);
    m_timer[3]->stop();
    m_timer[5]->propVal(m_nseeds);
    m_timer[5]->stop();
    m_timer[6]->propVal(m_ntracks);
    m_timer[6]->stop();
  }
  

  if(m_timers) m_timer[7]->start();
  m_trackmaker->endEvent();
  if(m_timers) m_timer[7]->stop();

  // Remove shared tracks with worse quality
  //

  if(m_timers) m_timer[8]->start();
  filterSharedTracks(qualityTrack);
  m_timer[8]->propVal(m_ntracks);
  if(m_timers) m_timer[8]->stop();

  std::multimap<double,Trk::Track*>::iterator 
    q = qualityTrack.begin(), qe =qualityTrack.end(); 

  m_ntracks = 0;

  for(; q!=qe; ++q) {
    foundTracks->push_back((*q).second); ++m_ntracks; 
  }

  if(m_timers) {
    m_timer[0]->stop();
    m_pattRecoTiming=m_timer[0]->elapsed();
  }

  if (outputLevel <= MSG::DEBUG) {
    athenaLog << MSG::DEBUG << "REGTEST: Number of seeds found = " << m_nseeds << endreq;
    athenaLog << MSG::DEBUG << "REGTEST: Investigated "
	      << std::setw(7) <<  m_nseeds 
	      << " seeds and found "
	      << std::setw(5) << foundTracks->size() <<" tracks"
	      << endreq;
  }

  for (size_t i=0; i<foundTracks->size() ; i++){
    TrigInDetTrack* p = convertOutputTracks(foundTracks->at(i));
    if(p!=NULL) {
      trackColl->push_back(p);
    }
  }

  delete foundTracks;

  return rc;
}


///////////////////////////////////////////////////////////////////
// Track quality calculation
///////////////////////////////////////////////////////////////////

double TrigL2PattRecoStrategyC::trackQuality(const Trk::Track* Tr)
{
 
 DataVector<const Trk::TrackStateOnSurface>::const_iterator  
    m  = Tr->trackStateOnSurfaces()->begin(), 
    me = Tr->trackStateOnSurfaces()->end  ();

 double quality = 0. ;
 double W       = 17.;

 for(; m!=me; ++m) {
   const Trk::FitQualityOnSurface* fq =  (*m)->fitQualityOnSurface();
   if(!fq) continue;

   double x2 = fq->chiSquared();
   double q;
   if(fq->numberDoF() == 2) q = (1.2*(W-x2*.5)); 
   else                     q =      (W-x2    );
   if(q < 0.) q = 0.; quality+=q;
 }
 return quality;
}

///////////////////////////////////////////////////////////////////
// Filter shared tracks
///////////////////////////////////////////////////////////////////

void TrigL2PattRecoStrategyC::filterSharedTracks(std::multimap<double,Trk::Track*>& QT) {

  std::set<const Trk::PrepRawData*> clusters;
  std::multimap<double,Trk::Track*>::iterator q = QT.begin();
  
  const Trk::PrepRawData* prd[100]; 
  
  while(q!=QT.end()) {
    
    std::set<const Trk::PrepRawData*>::iterator fe = clusters.end();

    DataVector<const Trk::MeasurementBase>::const_iterator 
      m  = (*q).second->measurementsOnTrack()->begin(), 
      me = (*q).second->measurementsOnTrack()->end  ();
 
    int nf = 0, nc = 0; 
    for(; m!=me; ++m) {

      const Trk::PrepRawData* pr = ((const Trk::RIO_OnTrack*)(*m))->prepRawData();
      if(pr) {
	++nc; if(clusters.find(pr)==fe) {prd[nf++]=pr; if(nf==100) break;}
      }
    }

    if(nf >= m_nfreeCut|| nf == nc) {
      for(int n=0; n!=nf; ++n) clusters.insert(prd[n]); ++q;
    } 
    else  {
      delete (*q).second; QT.erase(q++);
    }
  }
}

StatusCode TrigL2PattRecoStrategyC::convertInputData(const std::vector<IdentifierHash>& pixelIds, const std::vector<IdentifierHash>& sctIds) {

  //MsgStream athenaLog(msgSvc(), name());
  StatusCode sc = getIDCs();

  if(sc.isFailure()) return sc;

  TrigL2SpacePointCollectionConverter<TrigL2SCT_SpacePointConverter> sctSpCnv(m_sctSpacePointsContainer, m_sctOnlineSpacePointsContainer);
  std::for_each(sctIds.begin(),sctIds.end(),sctSpCnv);
  TrigL2SpacePointCollectionConverter<TrigL2PixelSpacePointConverter> pixSpCnv(m_pixelSpacePointsContainer, m_pixelOnlineSpacePointsContainer);
  std::for_each(pixelIds.begin(),pixelIds.end(),pixSpCnv);
  
  return sc;
}

StatusCode TrigL2PattRecoStrategyC::convertInputData() {

  // MsgStream athenaLog(msgSvc(), name());
  
  StatusCode sc = getIDCs();
  if(sc.isFailure()) return sc;

  TrigL2SpacePointCollectionConverter<TrigL2SCT_SpacePointConverter> sctSpCnv(m_sctSpacePointsContainer, m_sctOnlineSpacePointsContainer);
  std::for_each( m_sctOnlineSpacePointsContainer->begin(),m_sctOnlineSpacePointsContainer->end(),sctSpCnv);
  TrigL2SpacePointCollectionConverter<TrigL2PixelSpacePointConverter> pixSpCnv(m_pixelSpacePointsContainer, m_pixelOnlineSpacePointsContainer);
  std::for_each( m_pixelOnlineSpacePointsContainer->begin(), m_pixelOnlineSpacePointsContainer->end(),pixSpCnv);

  return sc;
}

StatusCode TrigL2PattRecoStrategyC::getIDCs() {
MsgStream athenaLog(msgSvc(), name());
  
  StatusCode sc(StatusCode::SUCCESS);

  if( !evtStore()->transientContains<SpacePointContainer>(m_pixelSpacePointsName)){   
    m_pixelSpacePointsContainer->cleanup();

     if (evtStore()->record(m_pixelSpacePointsContainer, m_pixelSpacePointsName).isFailure()) { 
       athenaLog << MSG::WARNING << " Container " << m_pixelSpacePointsName
             << " could not be recorded in StoreGate !" << endreq; 
     }  
     else { 
       /*
       athenaLog << MSG::DEBUG << "Container " << m_pixelSpacePointsName 
             << " registered  in StoreGate" << endreq;  
       */ 
     } 
  }
  else {     
    sc = evtStore()->retrieve(m_pixelSpacePointsContainer, m_pixelSpacePointsName); 
    if (sc.isFailure()) { 
      athenaLog << MSG::ERROR << "Failed to get Pixel space point Container" << endreq; 
      return sc; 
    } 
    else {  
      /*
      athenaLog << MSG::DEBUG << "Got Pixel space point Container from TDS: " 
		<< m_pixelSpacePointsName << endreq; 
      */
    } 
  }
  
  if( !evtStore()->transientContains<SpacePointContainer>(m_sctSpacePointsName)){ 

    m_sctSpacePointsContainer->cleanup();

    if (evtStore()->record(m_sctSpacePointsContainer, m_sctSpacePointsName).isFailure()) { 
       athenaLog << MSG::WARNING << " Container " << m_sctSpacePointsName
		 << " could not be recorded in StoreGate !" << endreq; 
    }  
    else { 
      /*
      athenaLog << MSG::DEBUG << "Container " << m_sctSpacePointsName 
		<< " registered  in StoreGate" << endreq;   
      */
    } 
  }
  else {     
    sc = evtStore()->retrieve(m_sctSpacePointsContainer, m_sctSpacePointsName); 
    if (sc.isFailure()) { 
      athenaLog << MSG::ERROR << "Failed to get SCT space point Container" << endreq; 
      return sc; 
    } 
    else {  
      /*
      athenaLog << MSG::DEBUG << "Got SCT space point Container from TDS: " 
		<< m_sctSpacePointsName << endreq; 
      */
    } 
  }
 
  sc = evtStore()->retrieve(m_pixelOnlineSpacePointsContainer, m_pixelOnlineSpacePointsName); 
  if (sc.isFailure()) { 
    athenaLog << MSG::ERROR << "Failed to get Pixel SpacePoints Container " <<m_pixelOnlineSpacePointsName<< endreq; 
    return sc; 
  } 
  else {
    /*  
    athenaLog << MSG::DEBUG << "Got Pixel SpacePoints Container from TDS: " 
	      << m_pixelOnlineClustersName << endreq; 
    */
  } 

  sc = evtStore()->retrieve(m_sctOnlineSpacePointsContainer, m_sctOnlineSpacePointsName); 
  if (sc.isFailure()) { 
    athenaLog << MSG::ERROR << "Failed to get SCT SpacePoints Container " <<m_sctOnlineSpacePointsName<< endreq; 
    return sc; 
  } 
  else {
    /*  
    athenaLog << MSG::DEBUG << "Got SCT SpacePoints Container from TDS: " 
	      << m_pixelOnlineClustersName << endreq; 
    */
  }
  return sc;
}
 
TrigInDetTrack* TrigL2PattRecoStrategyC::convertOutputTracks(Trk::Track* t) {

  MsgStream athenaLog(msgSvc(), name());
  int outputLevel = msgSvc()->outputLevel( name() );

  TrigInDetTrack* pTrack=NULL;

  std::vector<const TrigSiSpacePoint*>* pvsp = new std::vector<const TrigSiSpacePoint*>;
  pvsp->clear();

  std::vector<const TrigSiSpacePoint*> vpsctSP;
  vpsctSP.clear();
  std::vector<const InDet::SCT_Cluster*> vpClust;
  vpClust.clear();

  DataVector<const Trk::MeasurementBase>::const_iterator mIt = t->measurementsOnTrack()->begin();
  for(; mIt!=t->measurementsOnTrack()->end();++mIt) {
    const Trk::PrepRawData* pPRD = ((const Trk::RIO_OnTrack*)(*mIt))->prepRawData();
    if(pPRD) {
      const InDet::SCT_Cluster* pSCT = dynamic_cast<const InDet::SCT_Cluster*>(pPRD);
      if(pSCT) {
	const IdentContIndex ici = pSCT->getHashAndIndex();
	//	std::cout<<"SCT cluster hashId="<<ici.collHash()<<std::endl; 
	TrigSiSpacePointContainer::const_iterator spCollIt = m_sctOnlineSpacePointsContainer->indexFind(ici.collHash());
	if(spCollIt!=m_sctOnlineSpacePointsContainer->end()) {
	  //  std::cout<<"Found ! iterating now looking for "<<pSCT->identify()<<std::endl;
	  for(TrigSiSpacePointCollection::const_iterator spIt=(*spCollIt)->begin();spIt!=(*spCollIt)->end();++spIt) {
	    if(pSCT->identify()==(*spIt)->clusters().first->identify()) {
	      //   std::cout<<"Phi cluster detected, corresponding stereo id="<<(*spIt)->clusters().second->identify()<<std::endl;
	      vpsctSP.push_back((*spIt));
	    }
	  }
	}
	else {
	  //	  std::cout<<"Not found, could be a stereo cluster, id="<<pSCT->identify()<<std::endl;
	  vpClust.push_back(pSCT);
	}
      }
      else {
	const InDet::PixelCluster* pPIX = dynamic_cast<const InDet::PixelCluster*>(pPRD);
	if(pPIX) {
	  const IdentContIndex ici = pPIX->getHashAndIndex();
	  //  std::cout<<"Pixel cluster hashId="<<ici.collHash()<<std::endl; 
	  TrigSiSpacePointContainer::const_iterator spCollIt = m_pixelOnlineSpacePointsContainer->indexFind(ici.collHash());
	  if(spCollIt!=m_pixelOnlineSpacePointsContainer->end()) {
	    //   std::cout<<"Found ! iterating now"<<std::endl;
	    for(TrigSiSpacePointCollection::const_iterator spIt=(*spCollIt)->begin();spIt!=(*spCollIt)->end();++spIt) {
	      if(pPIX->identify()==(*spIt)->clusters().first->identify()) {
		//		std::cout<<"Pixel cluster detected"<<std::endl;
		pvsp->push_back((*spIt));
		//	std::cout<<"sp stored"<<std::endl;
	      }
	    }
	  }
	  else {
	    std::cout<<"ERROR PIXEL CL NOT FOUND IN SP id="<<pPIX->identify()<<std::endl;
	  }
	}
      }
    }
  }

  // resolving stereo clusters
  int nClust = vpClust.size();
  for(std::vector<const TrigSiSpacePoint*>::const_iterator spIt=vpsctSP.begin();spIt!=vpsctSP.end();++spIt) {
    for(std::vector<const InDet::SCT_Cluster*>::const_iterator clIt=vpClust.begin();clIt!=vpClust.end();++clIt) {
      if((*spIt)->clusters().second->identify()==(*clIt)->identify()) {
	pvsp->push_back(*spIt);
	--nClust;
      }
    }
  }
  if (outputLevel <= MSG::DEBUG) {
    athenaLog << MSG::DEBUG <<nClust<<" SCT clusters left unassigned "<<endreq;
  }

  if(pvsp->empty() || (pvsp->size()<4)) {
    delete pvsp;
    return pTrack;
  }

  if (outputLevel <= MSG::DEBUG) {
    athenaLog << MSG::DEBUG <<"created vector with "<<pvsp->size()<<" spacepoints"<<endreq;
  }

  //  std::cout<<(*t)<<std::endl;

  const Trk::TrackParameters* trackPars;
  trackPars = t->perigeeParameters();

  if(trackPars==NULL) {
    delete pvsp;
    return pTrack;
  }

  double D0 = trackPars->parameters()[Trk::d0]; 
  double Z0 = trackPars->parameters()[Trk::z0]; 
  double Phi0 = trackPars->parameters()[Trk::phi0]; 
  double Theta = trackPars->parameters()[Trk::theta]; 
  if(Phi0>M_PI) Phi0-=2*M_PI; 
  if(Phi0<-M_PI) Phi0+=2*M_PI; 
  double Eta = -log(sin(0.5*Theta)/cos(0.5*Theta)); 
    
  double qOverP = trackPars->parameters()[Trk::qOverP]; 
  double Pt=sin(Theta)/qOverP; 

  // delete trackPars;

  const TrigInDetTrackFitPar* tidtfp = new TrigInDetTrackFitPar(D0,Phi0,Z0,Eta,Pt,NULL); 
  
  pTrack = new TrigInDetTrack(pvsp,tidtfp);
  pTrack->algorithmId(TrigInDetTrack::STRATEGY_C_ID);

  return pTrack;

}
