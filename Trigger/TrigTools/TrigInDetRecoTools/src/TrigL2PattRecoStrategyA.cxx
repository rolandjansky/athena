/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

////////////////////////////////////////////////////////////////////////////////
// TrigL2PattRecoStrategyA
// -------------------------------
// ATLAS Collaboration
//
// 15.04.2011 Package created
//
// Author: Ilektra A. Christidi, UCL
// e-mail: Electra.Christidi@cern.ch
//
// implements IDSCAN algorithm
//
////////////////////////////////////////////////////////////////////////////////

#include <cmath>
//#include <iostream>
//#include <list>
//#include "GaudiKernel/ToolFactory.h"
//#include "StoreGate/DataHandle.h"
#include "AthenaKernel/Timeout.h"

#include "TrigInDetEvent/TrigInDetTrack.h"
#include "TrigInDetEvent/TrigInDetTrackCollection.h"
#include "TrigSteeringEvent/TrigRoiDescriptor.h"

#include "TrigInDetRecoTools/ITrigL2PattRecoStrategy.h"

#include "TrigTimeAlgs/TrigTimerSvc.h"

#include "IRegionSelector/IRegSelSvc.h"
#include "TrigInDetRecoTools/TrigL2PattRecoStrategyA.h"

#include "EventInfo/EventInfo.h"
#include "EventInfo/EventID.h"
#include "eformat/DetectorMask.h"
#include "TrigInDetToolInterfaces/ITrigRun1ZFinder.h"
#include "InDetBeamSpotService/IBeamCondSvc.h"
#include "TrigInDetToolInterfaces/ITrigHitFilter.h"
#include "TrigInDetRecoTools/ITrigL2DupTrackRemovalTool.h"

TrigL2PattRecoStrategyA::TrigL2PattRecoStrategyA(const std::string& t, 
						 const std::string& n,
						 const IInterface*  p ): 
  AthAlgTool(t,n,p),
  m_regionSelector("RegSelSvc", n),
  m_zFinder("IDScanZFinder",this),
  m_findMultipleZ(false),
  m_nZvertices(0),
  m_hitFilter("IDScanHitFilter",this),
  m_dupTrkRemoval("DupTrackRemovalTool",this)
{
  declareInterface< ITrigL2PattRecoStrategy >( this ); 

  declareProperty( "AdjustLayerThreshold",  m_adjustLayerThreshold = false );
  declareProperty( "DetMaskCheck",          m_check_detector_mask = false);
  declareProperty( "DetMaskCheckOnEvent",   m_detector_mask_on_event = false);
  declareProperty( "TrigZFinder",           m_zFinder);
  declareProperty( "ZFinderMode",           m_zFinderMode = 0 );
  declareProperty( "FindMultipleZ",         m_findMultipleZ = false );
  declareProperty( "TrueVertexLocation",    m_TrueVertexLocation = "TrueTrigVertexColl" );
  declareProperty( "TrigHitFilter",         m_hitFilter);
  declareProperty( "DupTrackRemovalTool",   m_dupTrkRemoval);
  declareProperty( "UseBeamSpot",           m_useBeamSpot = true);
  declareProperty( "DoZF_Only",             m_doZF_Only = false);
}


StatusCode TrigL2PattRecoStrategyA::initialize()
{
  StatusCode sc = AthAlgTool::initialize();
  MsgStream athenaLog(msgSvc(), name());

  if ( m_adjustLayerThreshold ) { 
    athenaLog << MSG::INFO << "will adjust layer threshold depending on disabled modules" << endreq;
    sc = m_regionSelector.retrieve();
    if ( sc.isFailure() ) {
      athenaLog << MSG::FATAL<< "Unable to retrieve RegionSelector tool "<< m_regionSelector.type() << endreq;
      return sc;
    }
  }
  
  m_detector_mask_not_checked = m_check_detector_mask;
  
  m_hasPixelBLayer = true;
  m_hasPixelBarrel = true; m_hasPixelDisk  = true;
  m_hasSCTBarrelA  = true; m_hasSCTEndcapA = true;
  m_hasSCTBarrelC  = true; m_hasSCTEndcapC = true;

  /*  
  if(m_detector_mask_not_checked) {
    const EventInfo* pEventInfo(0);
    if ( evtStore()->retrieve(pEventInfo).isFailure() ) {
      athenaLog << MSG::FATAL << "Failed to get EventInfo for detector mask info in BeginRun()" << endreq;
      return StatusCode::FAILURE;
    }
    setup_detector_mask(pEventInfo->event_ID());
  }
  */
  if( m_zFinderMode ==1 ){
    athenaLog << MSG::WARNING << " You have chosen to use the MC z0 position!" << endreq;
  }
  else {
    sc = m_zFinder.retrieve();
    if ( sc.isFailure() ) {
      athenaLog << MSG::FATAL << "Unable to retrieve ZFinder " << m_zFinder  << endreq;
      return StatusCode::FAILURE;
    }
  }

  if(m_doZF_Only) {
    athenaLog << MSG::INFO << "ZFinder-only mode, HitFilter will not be run !"<< endreq;
  }
  else {
    athenaLog << MSG::INFO << "Normal mode, HitFilter will be run"<< endreq;
  }

  if (m_useBeamSpot) {
    sc = service("BeamCondSvc", m_iBeamCondSvc);
    if (sc.isFailure() || m_iBeamCondSvc == 0) {
      m_iBeamCondSvc = 0;
      athenaLog << MSG::WARNING << "Could not retrieve Beam Conditions Service. " << endreq;
    }
  }

  sc = m_hitFilter.retrieve();
  if ( sc.isFailure() ){
    athenaLog << MSG::FATAL << "Unable to retrieve HitFilter " << m_hitFilter <<endreq;
    return StatusCode::FAILURE;
  }

  sc = m_dupTrkRemoval.retrieve();
  if ( sc.isFailure() ){
    athenaLog << MSG::FATAL << "Unable to retrieve DuplicateTrackRemovalTool " << m_dupTrkRemoval <<endreq;
    return StatusCode::FAILURE;
  }

  ITrigTimerSvc* timerSvc;
  sc = service( "TrigTimerSvc", timerSvc);
  if( sc.isFailure() ) {
    athenaLog << MSG::INFO<< "Unable to locate Service TrigTimerSvc " << endreq;
    m_timers = false;
  } 
  else{
    m_timers = true;  
  }
  if ( m_timers ) {
    m_totalTimer     = timerSvc->addItem("StrA_PattReco");
    m_ZFinderTimer   = timerSvc->addItem("ZFinder");
    m_ZFinderTimer->propName("nSP");
    m_HitFilterTimer = timerSvc->addItem("HitFilter");
    m_HitFilterTimer->propName("nTracks");
  }

  athenaLog << MSG::INFO << "TrigL2PattRecoStrategyA initialized "<< endreq;
  return sc;
}


StatusCode TrigL2PattRecoStrategyA::finalize() {

  StatusCode sc = AthAlgTool::finalize(); 
  return sc;
}


TrigL2PattRecoStrategyA::~TrigL2PattRecoStrategyA() {

}


HLT::ErrorCode TrigL2PattRecoStrategyA::findTracks(const std::vector<const TrigSiSpacePoint*>& vsp, 
						   const IRoiDescriptor& roi,
						   TrigInDetTrackCollection*  trackColl) {

  if ( m_timers ) m_totalTimer->start();

  m_pattRecoTiming=0.0;
  m_zVertices.clear();
  m_nZvertices=0;
  int nHits = vsp.size();

  MsgStream athenaLog(msgSvc(), name());
  int outputLevel = msgSvc()->outputLevel( name() );
  if (outputLevel <= MSG::DEBUG) {
    if ( roi.isFullscan() ) // || roi==nullptr)
      athenaLog<< MSG::DEBUG<<"TrigL2PattRecoStrategyA called in FullScan mode "<<endreq;
    else
      athenaLog<< MSG::DEBUG<<"TrigL2PattRecoStrategyA called in RoI-based mode "<<endreq;
  }

  // 1. z-finder
  /*
  double zfPhiWidth = m_zFinder->RoI_PhiWidth();
  if(fabs(zfPhiWidth - 2.0*(roi.phiHalfWidth()))>0.0001)
    {
      athenaLog << MSG::WARNING << "ZFinder ROIPhiWidth does not match input RoI phiHalfWidth"<<endreq;
      athenaLog << MSG::WARNING << "ZFinder ROIPhiWidth = "<<zfPhiWidth<<endreq;
      athenaLog << MSG::WARNING << "Input RoI phiHalfWidth x 2 = "<<2.0*(roi.phiHalfWidth())<<endreq;
      athenaLog << MSG::WARNING << "Wrong instance configuration - quitting"<<endreq;
      return HLT::BAD_JOB_SETUP;
      }*/
  //unsigned int zVertexFailOrSucceed=0;
  TrigVertexCollection* zTrueVertexColl=NULL;

  if ( m_timers ) m_ZFinderTimer->start();
  if ( m_zFinderMode == 1 ){ // true Z
    if ( evtStore()->retrieve(zTrueVertexColl, m_TrueVertexLocation).isFailure() ) {
      if (outputLevel <= MSG::DEBUG) athenaLog << MSG::DEBUG << "Failed to get trueZvCollection" << endreq; 
      m_nZvertices = 0;
    } 
    else {
      TrigVertex* trueVertex = zTrueVertexColl->front();
      if (outputLevel <= MSG::DEBUG) athenaLog << MSG::DEBUG << " True z position " << (trueVertex->position()).z() << endreq;
      m_nZvertices = 1;
      m_zVertices.push_back((trueVertex->position()).z());
    }
  } 
  else {
    /*if(m_doShift)
      //        zVertexColl = m_zFinder->findZ( spacePoints->spVec(), roi );
      zVertexColl = m_zFinder->findZ( m_shifterTool->spVec(), roi );
    else
    zVertexColl = m_zFinder->findZ( commonSPs,  roi );*/
    TrigVertexCollection* zVertexColl = m_zFinder->findZ( vsp,roi ); 
    if ( zVertexColl ) {
      for(TrigVertexCollection::iterator ivx=zVertexColl->begin(); ivx != zVertexColl->end(); ivx++)
	m_zVertices.push_back( (*ivx)->position().z() );
      m_nZvertices = m_zVertices.size();
      delete zVertexColl;
    }
  }

  if ( m_timers ) { 
    m_ZFinderTimer->propVal( nHits );
    m_ZFinderTimer->stop();
  }
  
  if (m_nZvertices==0) {
    m_zVertices.push_back(-9999.0);
    if ( m_timers ) {
      m_totalTimer->stop();
      m_pattRecoTiming=m_totalTimer->elapsed();
    }
    return HLT::OK;
  }

  if(m_doZF_Only) {
    trackColl->clear();
    if ( m_timers ) {
      m_totalTimer->stop();
      m_pattRecoTiming=m_totalTimer->elapsed();
    }
    return HLT::OK;
  }

  // 2. Event information, masking update  
  // get the run and event no.
  const EventInfo* pEventInfo;
  unsigned int IdRun=0;
  unsigned int IdEvent=0;
  if ( evtStore()->retrieve(pEventInfo).isFailure() ) {
    if(m_detector_mask_not_checked && m_detector_mask_on_event) {
      athenaLog << MSG::ERROR << "Could not find EventInfo object for detector mask info" << endreq;
      return HLT::SG_ERROR;
    }
    // if we are not interested in the detector mask, this is a minor problem.
    if (outputLevel <= MSG::DEBUG) athenaLog  << MSG::DEBUG << "Failed to get EventInfo " << endreq;
  } 
  else {
    IdRun   = pEventInfo->event_ID()->run_number();
    IdEvent = pEventInfo->event_ID()->event_number();
    if (outputLevel <= MSG::DEBUG) athenaLog << MSG::DEBUG << " Run " << IdRun << " Event " << IdEvent << endreq;    
    if(m_detector_mask_not_checked && m_detector_mask_on_event)
      setup_detector_mask(pEventInfo->event_ID());
  }

 // Extract the beamspot shift
  double shiftx(0), shifty(0);
  //if (!m_doShift && m_useBeamSpot && m_iBeamCondSvc) {
  if (m_useBeamSpot && m_iBeamCondSvc) {
    Amg::Vector3D vertex = m_iBeamCondSvc->beamPos();
    if (outputLevel <= MSG::DEBUG) athenaLog << MSG::DEBUG << "Beam spot position " << vertex << endreq;
    double xVTX = vertex.x();
    double yVTX = vertex.y();
    double zVTX = vertex.z();
    double tiltXZ = m_iBeamCondSvc->beamTilt(0);
    double tiltYZ = m_iBeamCondSvc->beamTilt(1);
    shiftx = xVTX - tiltXZ*zVTX;//correction for tilt
    shifty = yVTX - tiltYZ*zVTX;//correction for tilt
    if (outputLevel <= MSG::DEBUG) athenaLog << MSG::DEBUG << "Center position:  " << shiftx <<"  "<< shifty << endreq;
  }

  // 4. HitFilter
  if ( m_timers ) m_HitFilterTimer->start();
  std::vector<unsigned int> placeHolder;
  for ( int iz=0; iz<m_nZvertices; ++iz )  {
    double zPosition = m_zVertices[iz];
    if (outputLevel <= MSG::DEBUG) athenaLog << MSG::DEBUG << "REGTEST / z found = " << zPosition << endreq;
    /*if(m_doShift) {
    //m_hitFilter->findTracks( spacePoints->spVec(), *m_recoTracks, roi, zPosition, spacePoints->get_xshift());
    m_hitFilter->findTracks( m_shifterTool->spVec(), *m_recoTracks, roi, zPosition, 
    m_shifterTool->get_xshift(), 0, missing_layers );
    }
    else{ */
    //m_hitFilter->findTracks(vsp, *m_recoTracks, &roi, zPosition, shiftx, shifty, missing_layers );
    m_hitFilter->findTracks(vsp, *trackColl, &roi, zPosition, shiftx, shifty, 0);
    placeHolder.push_back(trackColl->size());
    if ( !trackColl->empty() && m_findMultipleZ == false ) break;
  }
  if ( m_timers ) {
    m_HitFilterTimer->propVal( trackColl->size() );
    m_HitFilterTimer->stop();
  }

  // protection against timeouts
  if (Athena::Timeout::instance().reached()) {
    if(outputLevel<=MSG::DEBUG)
      athenaLog << MSG::DEBUG << "Timeout reached. Aborting sequence." << endreq;
    return HLT::ErrorCode(HLT::Action::ABORT_CHAIN, HLT::Reason::TIMEOUT);	        
  }
  
  if ( trackColl->empty() ) 
    {
      if (outputLevel <= MSG::DEBUG) athenaLog << MSG::DEBUG << "REGTEST / No groups found - No tracks reconstructed" << endreq;
    }
  else
    { 
      //m_currentStage = 5;       
      //monitor number of tracks
      //m_ntracks=m_recoTracks->size();
      if (outputLevel <= MSG::DEBUG) athenaLog << MSG::DEBUG << "REGTEST / Made " << trackColl->size() << " IdScanGroups" << endreq;
      //m_countRoIwithGroups[zVertexFailOrSucceed]++;
      
      //Deleting duplicate tracks from multiple z vertices 
      m_dupTrkRemoval->removeDupTracks(trackColl,placeHolder);
      for(TrigInDetTrackCollection::iterator it = trackColl->begin(); it!=trackColl->end();++it) {
	(*it)->algorithmId(TrigInDetTrack::STRATEGY_A_ID);
      }
    }

  if ( m_timers ) {
    m_totalTimer->stop();
    m_pattRecoTiming=m_totalTimer->elapsed();
  }

  return HLT::OK;
}


HLT::ErrorCode TrigL2PattRecoStrategyA::findTracks(const std::vector<const TrigSiSpacePoint*>& vsp,
						   TrigInDetTrackCollection* trackColl) {

  findTracks(vsp, TrigRoiDescriptor(true), trackColl);

  return HLT::OK;
 
}



void TrigL2PattRecoStrategyA::setup_detector_mask(const EventID* eventId) { 
  
  MsgStream athenaLog(msgSvc(), name());
  int outputLevel = msgSvc()->outputLevel( name() );

  uint64_t dmask = eventId->detector_mask();

  if(outputLevel <= MSG::DEBUG) {
    athenaLog << MSG::DEBUG << "Retrieved the detector mask = 0x"
	  << MSG::hex << dmask << MSG::dec << endreq;
  }

  if( dmask==0 ) {
    if(outputLevel <= MSG::DEBUG) {
      athenaLog << MSG::DEBUG << "detector mask == 0: do nothing!" << endreq;
    }
    return;
  }

  eformat::helper::DetectorMask dm(dmask);
  m_hasPixelBLayer  = dm.is_set(eformat::PIXEL_B_LAYER);
  m_hasPixelBarrel  = dm.is_set(eformat::PIXEL_BARREL);
  m_hasPixelDisk    = dm.is_set(eformat::PIXEL_DISK);
  m_hasSCTBarrelA   = dm.is_set(eformat::SCT_BARREL_A_SIDE);
  m_hasSCTBarrelC   = dm.is_set(eformat::SCT_BARREL_C_SIDE);
  m_hasSCTEndcapA   = dm.is_set(eformat::SCT_ENDCAP_A_SIDE);
  m_hasSCTEndcapC   = dm.is_set(eformat::SCT_ENDCAP_C_SIDE);

  if(outputLevel <= MSG::DEBUG) {
    athenaLog << MSG::DEBUG << " ==== Detector Mask settings ==== " << endreq;           
    athenaLog << MSG::DEBUG << " ---> Pixel B-Layer: " << ((m_hasPixelBLayer)? "ON" : "OFF") << endreq;
    athenaLog << MSG::DEBUG << " ---> Pixel Barrel: "  << ((m_hasPixelBarrel)? "ON" : "OFF") << endreq;
    athenaLog << MSG::DEBUG << " ---> Pixel Disk: "    << ((m_hasPixelDisk)  ? "ON" : "OFF") << endreq;
    athenaLog << MSG::DEBUG << " ---> SCT Barrel side A: " << ((m_hasSCTBarrelA)? "ON" : "OFF") << endreq;
    athenaLog << MSG::DEBUG << " ---> SCT Barrel side C: " << ((m_hasSCTBarrelC)? "ON" : "OFF") << endreq;
    athenaLog << MSG::DEBUG << " ---> SCT Endcap side A: " << ((m_hasSCTEndcapA)? "ON" : "OFF") << endreq;
    athenaLog << MSG::DEBUG << " ---> SCT Endcap side C: " << ((m_hasSCTEndcapC)? "ON" : "OFF") << endreq;
  }

  if (!m_detector_mask_on_event) m_detector_mask_not_checked = false;
}


