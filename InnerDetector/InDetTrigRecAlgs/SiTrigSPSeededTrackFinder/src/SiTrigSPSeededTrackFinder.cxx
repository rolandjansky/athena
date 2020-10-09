/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

//***************************************************************************
// 
// Implementation for SiTrigSPSeededTrackFinder
// (see class definition)
//
//****************************************************************************
#include "GaudiKernel/MsgStream.h"
#include "TrigTimeAlgs/TrigTimer.h"
#include "SiTrigSPSeededTrackFinder/SiTrigSPSeededTrackFinder.h"
#include "TrkTrack/Track.h"
#include "TrkTrack/TrackCollection.h"
#include "AthenaKernel/Timeout.h"

//Trigger stuff
#include "TrigSteeringEvent/TrigRoiDescriptor.h"
#include "IRegionSelector/IRegSelTool.h"
#include "IRegionSelector/IRoiDescriptor.h"

//tools
#include "InDetRecToolInterfaces/ISiSpacePointsSeedMaker.h"
#include "InDetRecToolInterfaces/ISiZvertexMaker.h" 
#include "InDetRecToolInterfaces/ISiTrackMaker.h" 

#include "SiSPSeededTrackFinderData/SiSpacePointsSeedMakerEventData.h"
#include "SiSPSeededTrackFinderData/SiTrackMakerEventData_xk.h"

//
#include "TrigInDetEvent/TrigInDetTrackCollection.h"
#include "TrkParameters/TrackParameters.h"
#include "TrkMeasurementBase/MeasurementBase.h"
#include "TrkRIO_OnTrack/RIO_OnTrack.h"

//Eigen
#include "EventPrimitives/EventPrimitives.h"
#include "GeoPrimitives/GeoPrimitives.h"

#include "TrigNavigation/NavigationCore.icc"
#include "GaudiKernel/ThreadLocalContext.h"

///////////////////////////////////////////////////////////////////
// Constructor
///////////////////////////////////////////////////////////////////

InDet::SiTrigSPSeededTrackFinder::SiTrigSPSeededTrackFinder
(const std::string& name,ISvcLocator* pSvcLocator): 
  HLT::FexAlgo(name, pSvcLocator),
  m_useZvertexTool(true),
  m_useSeedMaker(true),
  m_outputlevel(0),
  m_nprint(0),
  m_nseeds(0),
  m_ntracks(0),
  m_nseedsTotal(0),
  m_ntracksTotal(0),
  m_nInvoked(0),
  m_nfreeCut(5),
  m_seedsmaker("InDet::SiSpacePointsSeedMaker_ATLxk/InDetTrigSiSpacePointsSeedMaker"),
  m_zvertexmaker("InDet::SiZvertexMaker_xk/InDetTrigZvertexMaker"),
  m_trackmaker("InDet::SiTrackMaker_xk/InDetTrigSiTrackMaker"),
  m_etaHalfWidth(0.1),
  m_phiHalfWidth(0.1),
  m_doFullScan(false),
  m_fastTracking(false),
  m_doTimeOutChecks(true)
{
  // SiTrigSPSeededTrackFinder steering parameters
  declareProperty("SeedsTool"     ,m_seedsmaker    );
  declareProperty("ZvertexTool"   ,m_zvertexmaker  );
  declareProperty("TrackTool"     ,m_trackmaker    );
  
  declareProperty("useZvertexTool",m_useZvertexTool);
  declareProperty("useSeedMaker",m_useSeedMaker);
  declareProperty("EtaHalfWidth",  m_etaHalfWidth);
  declareProperty("PhiHalfWidth",  m_phiHalfWidth);
  declareProperty("doFullScan",    m_doFullScan);
  declareProperty("FastTracking",  m_fastTracking);
  declareProperty("FreeClustersCut"   ,m_nfreeCut      );
  declareProperty("doTimeOutChecks",         m_doTimeOutChecks);
  
  //declare properties to be monitored
  declareMonitoredVariable("numSeeds",m_nseeds);
  declareMonitoredVariable("numtracks",m_ntracks);
  declareMonitoredVariable("numZvtx",m_nZvtx);
  declareMonitoredVariable("numDetElPixel",m_nDetElPixel);
  declareMonitoredVariable("numDetElSCT",m_nDetElSCT);
  declareMonitoredVariable("RoIEta",m_RoIEta);
  declareMonitoredVariable("RoIPhi",m_RoIPhi);
  declareMonitoredVariable("nGoodSeedsTotal",m_nGoodSeedsTotal);
  declareMonitoredVariable("nBadSeedsTotal",m_nBadSeedsTotal);
  declareMonitoredVariable("time_GoodSeeds",m_timeGoodSeeds);
  declareMonitoredVariable("time_BadSeeds",m_timeBadSeeds);
  declareMonitoredVariable("time_AveGoodSeeds",m_AvtimeGoodSeeds);
  declareMonitoredVariable("time_AveBadSeeds",m_AvtimeBadSeeds);
 
  m_timerRegSel = 0;
  m_timerZVertexTool = 0;
  m_timerSeedsMaker = 0;
  m_timerTrackMaker = 0;
  m_timerSeedProcessing = 0;

  m_AvtimeGoodSeeds = 0.;
  m_AvtimeBadSeeds = 0.;

  m_nseedsTotal  = 0;
  m_nGoodSeedsTotal =0;
  m_nBadSeedsTotal =0;
  m_ntracksTotal = 0;

}

///////////////////////////////////////////////////////////////////
// Initialisation
///////////////////////////////////////////////////////////////////

HLT::ErrorCode InDet::SiTrigSPSeededTrackFinder::hltInitialize() {
  // Get tool for space points seed maker
  if ( m_seedsmaker.retrieve().isFailure() ) {
    msg() << MSG::FATAL << "Failed to retrieve tool " << m_seedsmaker << endmsg;
    return HLT::ErrorCode(HLT::Action::ABORT_JOB, HLT::Reason::BAD_JOB_SETUP);
  } 
  else {
    msg() << MSG::INFO << "Retrieved tool " << m_seedsmaker << endmsg;
  }
  
  if(m_useZvertexTool) {
    
    // Get tool for z-coordinates primary vertices search
    if ( m_zvertexmaker.retrieve().isFailure() ) {
      msg() << MSG::FATAL << "Failed to retrieve tool " << m_zvertexmaker << endmsg;
      return HLT::ErrorCode(HLT::Action::ABORT_JOB, HLT::Reason::BAD_JOB_SETUP);
    } 
    else {
      msg() << MSG::INFO << "Retrieved tool " << m_zvertexmaker << endmsg; 
    }
  } else {
    m_zvertexmaker.disable();
  }

  // Get track-finding tool
  if ( m_trackmaker.retrieve().isFailure() ) {
    msg() << MSG::FATAL << "Failed to retrieve tool " << m_trackmaker << endmsg;
    return HLT::ErrorCode(HLT::Action::ABORT_JOB, HLT::Reason::BAD_JOB_SETUP);
  } 
  else {
    msg() << MSG::INFO << "Retrieved tool " << m_trackmaker << endmsg;
  }

  msg() << MSG::INFO << "Using internal seedMaker: " << m_useSeedMaker << endmsg;

  // Get output print level
  //
  m_outputlevel = msg().level()-MSG::DEBUG;
  if(m_outputlevel<=0) {
    m_nprint=0; msg()<<MSG::DEBUG<<(*this)<<endmsg;
  }
  
  m_nseedsTotal  = 0;
  m_nGoodSeedsTotal =0;
  m_nBadSeedsTotal =0;
  m_ntracksTotal = 0;

  if(!m_doFullScan){
    // Retrieving Region Selector Tool
    if ( m_regionSelector_pixel.retrieve().isFailure() ) {
      msg() << MSG::FATAL
	    << "Unable to retrieve RegionSelector tool "
	    << m_regionSelector_pixel.type() << endmsg;
      return HLT::ErrorCode(HLT::Action::ABORT_JOB, HLT::Reason::BAD_JOB_SETUP);
    }
    if ( m_regionSelector_sct.retrieve().isFailure() ) {
      msg() << MSG::FATAL
	    << "Unable to retrieve RegionSelector tool "
	    << m_regionSelector_sct.type() << endmsg;
      return HLT::ErrorCode(HLT::Action::ABORT_JOB, HLT::Reason::BAD_JOB_SETUP);
    }
  }
  else{
    msg() << MSG::INFO
	  << "RegionSelector not initialized due to FullScan mode. " << endmsg;
  }

  // Initialize timers:
  m_timerRegSel = addTimer("RegSel");
  m_timerZVertexTool = addTimer("ZVertexTool");
  m_timerSeedsMaker = addTimer("SeedsMaker");
  m_timerTrackMaker = addTimer("TrackMaker");
  m_timerSeedProcessing = addTimer("SeedProcessing");

  return HLT::OK;
}

namespace InDet {
  class FeatureAccessor : public HLT::FexAlgo 
  {
  public:
    //make the getFeature method public
    template<class T> HLT::ErrorCode getFeature(const HLT::TriggerElement* te, const T*&  feature, 
                                                 const std::string& label = "") {
      return HLT::Algo::getFeature(te,feature,label);
    }
  };

  class ExtendedSiTrackMakerEventData_xk : public InDet::SiTrackMakerEventData_xk
  {
  public:
    ExtendedSiTrackMakerEventData_xk(HLT::FexAlgo &algo, const HLT::TriggerElement*outputTE, const std::string &key) {
      if (!key.empty()) {
        const Trk::PRDtoTrackMap *prd_to_track_map_cptr;
        HLT::ErrorCode stat = reinterpret_cast<FeatureAccessor &>(algo).getFeature(outputTE, prd_to_track_map_cptr, key);
        if(stat!= HLT::OK){
          throw std::runtime_error(std::string("Failed to get PRD to track map:") + key);
        }
        setPRDtoTrackMap(prd_to_track_map_cptr);
      }
    }
  private:
    void dummy() {}
  };
}

///////////////////////////////////////////////////////////////////
// Execute HLT Algorithm
///////////////////////////////////////////////////////////////////
HLT::ErrorCode InDet::SiTrigSPSeededTrackFinder::hltExecute(const HLT::TriggerElement*,
							    HLT::TriggerElement* outputTE) {
  
  //----------------------------------------------------------------------
  // Trigger specific part: navigate throw the trigger element to get the
  // ROI center. Use the region selector in order to retrieve the list of
  // detector elements that describe the region of the detector to be 
  // reconstructed.
  //----------------------------------------------------------------------
  
  //initialize monitoring variables
  m_nseeds = 0;
  m_ntracks= 0;  m_nZvtx  = 0;
  m_nDetElPixel = m_nDetElSCT = 0;
  m_RoIEta = m_RoIPhi =0;
  ++m_nInvoked;

  // Get RoiDescriptor
  const TrigRoiDescriptor* roi;
  if ( ( HLT::OK != getFeature(outputTE, roi) ) || !roi ) {
    msg() << MSG::WARNING << "Can't get RoI" << endmsg;
    return HLT::NAV_ERROR;
  }
  
  if (msgLvl() <= MSG::DEBUG) {
    msg() << MSG::DEBUG << "REGTEST:" << *roi << endmsg;
    msg() << MSG::DEBUG << "PhiHalfWidth: " << m_phiHalfWidth << " EtaHalfWidth: "<< m_etaHalfWidth <<endmsg;
  }
  m_RoIEta = roi->eta();
  m_RoIPhi = roi->phi();


  
  std::vector<IdentifierHash> listOfSCTIds; 
  std::vector<IdentifierHash> listOfPixIds; 


  
  if(!m_doFullScan){
    if(doTiming()) 
      m_timerRegSel->start();
    
    // SCT hash id's:
    m_regionSelector_sct->HashIDList( *roi, listOfSCTIds );
    
    m_nDetElSCT = listOfSCTIds.size();
    
    // pixels hash id's:
    m_regionSelector_pixel->HashIDList( *roi, listOfPixIds);
    m_nDetElPixel = listOfPixIds.size();

    if(doTiming()) 
      m_timerRegSel->stop();
  }
  
  const InDet::SiSpacePointsSeed* seed = 0;

  // Test is sct clusters collection for given event
  //
  bool PIX = true;
  bool SCT = true;

  SiSpacePointsSeedMakerEventData seedEventData;

  if (m_useSeedMaker){
    if( m_useZvertexTool ) {
      
      if(doTiming()) m_timerZVertexTool->start();

      std::list<Trk::Vertex> vertices;
      
      if(!m_doFullScan){
	if (m_fastTracking){
          vertices = m_zvertexmaker->newRegion(Gaudi::Hive::currentContext(), seedEventData, listOfPixIds, listOfSCTIds, *roi);
	}
	else {
	  vertices = m_zvertexmaker->newRegion(Gaudi::Hive::currentContext(), seedEventData, listOfPixIds, listOfSCTIds);
	}
      }
      else{
	vertices = m_zvertexmaker->newEvent(Gaudi::Hive::currentContext(), seedEventData);
      }
      
      if(doTiming()) m_timerZVertexTool->stop();
      
      if(doTiming()) m_timerSeedsMaker->start();
      
      m_seedsmaker->find3Sp(Gaudi::Hive::currentContext(), seedEventData, vertices);
      
      if(doTiming()) m_timerSeedsMaker->stop();
      
      m_nZvtx = vertices.size();
      if(m_outputlevel <= 0){ 
	ATH_MSG_DEBUG("REGTEST: Number of zvertex found = " << m_nZvtx);
      }
    }
    else{
      if(doTiming()) m_timerSeedsMaker->start();
      if(!m_doFullScan){
	if (m_fastTracking){
	  m_seedsmaker->newRegion(Gaudi::Hive::currentContext(), seedEventData, listOfPixIds, listOfSCTIds, *roi);
	} else {
	  m_seedsmaker->newRegion(Gaudi::Hive::currentContext(), seedEventData, listOfPixIds, listOfSCTIds);
	}

      }
      else{
	m_seedsmaker->newEvent(Gaudi::Hive::currentContext(), seedEventData);
      }
      
      std::list<Trk::Vertex> VZ;
      m_seedsmaker->find3Sp(Gaudi::Hive::currentContext(), seedEventData, VZ);
      if(doTiming()) m_timerSeedsMaker->stop();
    }
  }

  else   {     //!m_useSeedMaker
    //?
  }
 
  InDet::ExtendedSiTrackMakerEventData_xk trackEventData(*this, outputTE, m_prdToTrackMap);
  if(doTiming()) m_timerTrackMaker->start();
  if (m_fastTracking){
    m_trackmaker->newTrigEvent(Gaudi::Hive::currentContext(), trackEventData, PIX, SCT);
    //m_trackmaker->newEvent(trackEventData, PIX, SCT);
  } else {
    m_trackmaker->newEvent(Gaudi::Hive::currentContext(), trackEventData, PIX, SCT);
  }

  // Loop through all seeds and reconsrtucted tracks collection preparation
  

  TrackCollection* foundTracks = new TrackCollection;
  std::multimap<double,Trk::Track*>    qualityTrack;
  
  m_nseeds  = 0;
  m_ntracks = 0;
  m_nGoodSeeds  = 0;
  m_nBadSeeds = 0;
  
  m_timeGoodSeeds=0.0;
  m_timeBadSeeds=0.0;
  m_nGoodSeedsTotal=0;
  m_nBadSeedsTotal=0;
  m_timeGoodSeedsTotal=0;
  m_timeBadSeedsTotal=0;
  
  if (m_useSeedMaker){
    
    //++m_nseeds;
        
    if (msgLvl() <= MSG::DEBUG) 
      msg() << MSG::DEBUG << "Using internal seeding::::  " << endmsg;

    int nseedwithtrack(0);
    ///////////////////////////////////////
    
    while((seed = m_seedsmaker->next(Gaudi::Hive::currentContext(), seedEventData))) {
      if (m_doTimeOutChecks && Athena::Timeout::instance().reached() ) {
	      ATH_MSG_WARNING( "Timeout reached. Aborting sequence." );
	      delete foundTracks;
	      return HLT::ErrorCode(HLT::Action::ABORT_CHAIN, HLT::Reason::TIMEOUT);
      }
      if(doTiming()) m_timerSeedProcessing->start();
      ++m_nseeds;
      std::list<Trk::Track*> T = m_trackmaker->getTracks(Gaudi::Hive::currentContext(), trackEventData, seed->spacePoints());
      
      if (m_fastTracking){
	      for(std::list<Trk::Track*>::const_iterator t=T.begin(); t!=T.end(); ++t) {
	        if((*t)) qualityTrack.insert(std::make_pair(-trackQuality((*t)),(*t)));
	      }
      }
      
      if (msgLvl() <= MSG::VERBOSE) {
	msg() << MSG::VERBOSE << "Using 3SPs seed with ====> " << endmsg; 
	for(std::vector<const Trk::SpacePoint*>::const_iterator it=seed->spacePoints().begin(); it != seed->spacePoints().end(); it++) {
	  msg() << MSG::VERBOSE << "Using SP with :::: " << endmsg; 
	  msg() << " :: r :: " << (*it)->globalPosition().mag() 
		<< " :: perp :: " << (*it)->globalPosition().perp() 
		<< " :: phi :: " << (*it)->globalPosition().phi() 
		<< " :: theta :: " << (*it)->globalPosition().theta() 
		<< " :: eta :: " << (*it)->globalPosition().eta() 
		<< " :: x :: " << (*it)->globalPosition().x() 
		<< " :: y :: " << (*it)->globalPosition().y() 
		<< " :: z :: " << (*it)->globalPosition().z() <<endmsg;
	  (*it)->dump(msg());
	}
      }
      
      if(doTiming()){
	m_timerSeedProcessing->stop();
	if(T.empty()) {
	  m_nBadSeeds++;
	  m_timeBadSeeds+=m_timerSeedProcessing->elapsed();
	} 
	else {
	  m_nGoodSeeds++;
	  m_timeGoodSeeds+=m_timerSeedProcessing->elapsed();
	}
      }


      int TRACK_COUNTER(0);
      if (!m_fastTracking){
	for(std::list<Trk::Track*>::const_iterator t=T.begin(); t!=T.end(); ++t) {
	  ++m_ntracks; foundTracks->push_back((*t));
	  
	  TRACK_COUNTER++;
	  
	  if (msgLvl() <= MSG::VERBOSE) {
	    msg() << MSG::VERBOSE << "Found track with ====> " << endmsg; 
	    for(unsigned int i(0); i<((*t)->trackParameters())->size(); i++) {
	      double trkx = ((*t)->trackParameters()->at(i))->parameters()[Trk::x];
	      double trky = ((*t)->trackParameters()->at(i))->parameters()[Trk::y];
	      double trkz = ((*t)->trackParameters()->at(i))->parameters()[Trk::z];
	      double trktheta = ((*t)->trackParameters()->at(i))->parameters()[Trk::theta];
	      double trkphi = ((*t)->trackParameters()->at(i))->parameters()[Trk::phi];
	      msg() << MSG::VERBOSE << "Track Parameters :::: " << endmsg;
	      msg() << " :: r :: " << sqrt( trkx*trkx + trky*trky + trkz*trkz )
		    << " :: perp :: " << sqrt( trkx*trkx + trky*trky )
		    << " :: phi :: " << trkphi
		    << " :: theta :: " << trktheta
		    << " :: eta :: " << -log( tan( trktheta/2 ) )
		    << " :: d0 :: " << ((*t)->trackParameters()->at(i))->parameters()[Trk::d0]
		    << " :: z0 :: " << ((*t)->trackParameters()->at(i))->parameters()[Trk::z0]
		    << " :: x :: " << trkx
		    << " :: y :: " << trky
		    << " :: z :: " << trkz <<endmsg;
	    }
	    msg() << MSG::VERBOSE << endmsg 
		  << "========================== back ========================== " << endmsg << endmsg; 
	    ((*t)->trackParameters()->back())->dump(msg());
	    msg() << MSG::VERBOSE << endmsg 
		  << "========================================================== " << endmsg << endmsg; 
	    msg() << MSG::VERBOSE << endmsg 
		  << "========================== front ========================== " << endmsg << endmsg; 
	    ((*t)->trackParameters()->front())->dump(msg());
	    msg() << MSG::VERBOSE << endmsg 
		  << "=========================================================== " << endmsg << endmsg; 
	  }
	}
      }

      if(TRACK_COUNTER!=0)
	{
	  nseedwithtrack++;
	  
	  if (msgLvl() <= MSG::VERBOSE) {
	    msg() << MSG::VERBOSE << "Using seed :::: " << m_nseeds << " :::: " << endmsg;
	    
	    msg() << MSG::VERBOSE << endmsg 
		  << "========================== DUMP SEED ========================== " << endmsg << endmsg; 
	    seed->dump(msg());
	    msg() << MSG::VERBOSE << endmsg 
		  << "====================== END DUMP SEED ========================== " << endmsg << endmsg; 
	    
	    
	    msg() <<MSG::VERBOSE
		  << "TRACK_COUNTER = " << TRACK_COUNTER 
		  << " ====> Found " << TRACK_COUNTER << " track(s) with this seed. " << endmsg;
	  }
	}
    }
    
    if (msgLvl() <= MSG::DEBUG) {
      msg() << MSG::DEBUG << "Processed number of seeds = " << m_nseeds << ". " << endmsg; 
      msg() << MSG::DEBUG << "Number of seeds with at least 1 track = " << nseedwithtrack << ". " << endmsg; 
    }
    
  } else {      // external seeding
    

    const std::list<Amg::Vector3D> dummyp;

    const TrackCollection *tx;

    if (getFeature(outputTE, tx) != HLT::OK || !tx) {
      msg() << MSG::ERROR << "Can't get InDetTrackCollection " << endmsg;
      return HLT::NAV_ERROR;
    }
       
    TrackCollection::const_iterator iter    = tx->begin();
    TrackCollection::const_iterator iterEnd = tx->end();
      
    while (iter != iterEnd){

      if(doTiming()) 
	m_timerSeedProcessing->start();

      ++m_nseeds;
      
      const Trk::Perigee* perig = (*iter)->perigeeParameters();
      /////////////////////////////////////////////
      if (msgLvl() <= MSG::VERBOSE) {
	msg() << MSG::VERBOSE << "Using seed with ====> " << endmsg; 
	msg() << MSG::VERBOSE 
	      << "========================== DUMP Measured Perigee ========================== " << endmsg << endmsg; 
	double MPphi = perig->parameters()[Trk::phi]; 
	double MPtheta = perig->parameters()[Trk::theta]; 
	double MPd0 = perig->parameters()[Trk::d0]; 
	double MPz0 = perig->parameters()[Trk::z0]; 
	double MPx = perig->parameters()[Trk::x]; 
	double MPy = perig->parameters()[Trk::y]; 
	double MPz = perig->parameters()[Trk::z]; 
	msg() << " :: r :: " << sqrt( MPx*MPx + MPy*MPy + MPz*MPz )
	      << " :: perp :: " << sqrt( MPx*MPx + MPy*MPy )
	      << " :: phi :: " << MPphi
	      << " :: theta :: " << MPtheta
	      << " :: eta :: " << -log( tan( MPtheta/2 ) )
	      << " :: d0 :: " << MPd0
	      << " :: z0 :: " << MPz0
	      << " :: x :: " << MPx
	      << " :: y :: " << MPy
	      << " :: z :: " << MPz << endmsg;
	perig->dump(msg());
	msg() << MSG::VERBOSE 
	      << "========================== DUMP Measured Perigee ========================== " << endmsg << endmsg; 
      }
      
      std::list<Amg::Vector3D> gpList;
      
      gpList.clear();
      

      DataVector<const Trk::TrackStateOnSurface>::const_iterator it  = (*iter)->trackStateOnSurfaces()->begin();
      DataVector<const Trk::TrackStateOnSurface>::const_iterator ite = (*iter)->trackStateOnSurfaces()->end();

      //TODO selection
      while (it != ite) {
	if ((*it)->type(Trk::TrackStateOnSurface::Measurement)){
	  const Trk::MeasurementBase* mesh = (*it)->measurementOnTrack();
	  gpList.push_back(mesh->globalPosition());
	}
	++it;
      }
      
      if (msgLvl() <= MSG::VERBOSE) {
	
	msg() << MSG::VERBOSE << "Using gpList with ====> " << endmsg; 
	for(std::list<Amg::Vector3D>::iterator itt = gpList.begin(); itt != gpList.end(); ++itt) {
	  msg() << MSG::VERBOSE << "Global Position :::: " << endmsg; 
	  msg() << " :: r :: " << (*itt).mag() 
		<< " :: perp :: " << (*itt).perp() 
		<< " :: phi :: " << (*itt).phi() 
		<< " :: theta :: " << (*itt).theta() 
		<< " :: eta :: " << (*itt).eta() 
		<< " :: x :: " << (*itt).x() 
		<< " :: y :: " << (*itt).y() 
		<< " :: z :: " << (*itt).z() <<endmsg;
	}
      }
      
	  
      std::list<Trk::Track*> T = m_trackmaker->getTracks(Gaudi::Hive::currentContext(), trackEventData, *perig, gpList);//dummyp); //
	  
	  
	if(doTiming()){
	  m_timerSeedProcessing->stop();
	  if(T.empty()) {
	    m_nBadSeeds++;
	    m_timeBadSeeds+=m_timerSeedProcessing->elapsed();
	  } 
	  else {
	    m_nGoodSeeds++;
	    m_timeGoodSeeds+=m_timerSeedProcessing->elapsed();
	  }
	}
	  
	///////////////////////////////////////
	int TRACK_COUNTER(0);
	///////////////////////////////////////
	for(std::list<Trk::Track*>::const_iterator t=T.begin(); t!=T.end(); ++t) {
	  ++m_ntracks; foundTracks->push_back((*t));
	    
	  ///////////////////////////////////////
	  TRACK_COUNTER++;
	  if (msgLvl() <= MSG::VERBOSE) {

	    msg() << MSG::VERBOSE << "Found track with ====> " << endmsg; 
	    for(unsigned int i(0); i<((*t)->trackParameters())->size(); i++) {
	      double trkx = ((*t)->trackParameters()->at(i))->parameters()[Trk::x];
	      double trky = ((*t)->trackParameters()->at(i))->parameters()[Trk::y];
	      double trkz = ((*t)->trackParameters()->at(i))->parameters()[Trk::z];
	      double trktheta = ((*t)->trackParameters()->at(i))->parameters()[Trk::theta];
	      double trkphi = ((*t)->trackParameters()->at(i))->parameters()[Trk::phi];
	      msg() << MSG::VERBOSE << "Track Parameters :::: " << endmsg;
	      msg() << " :: r :: " << sqrt( trkx*trkx + trky*trky + trkz*trkz )
		    << " :: perp :: " << sqrt( trkx*trkx + trky*trky )
		    << " :: phi :: " << trkphi
		    << " :: theta :: " << trktheta
		    << " :: eta :: " << -log( tan( trktheta/2 ) )
		    << " :: d0 :: " << ((*t)->trackParameters()->at(i))->parameters()[Trk::d0]
		    << " :: z0 :: " << ((*t)->trackParameters()->at(i))->parameters()[Trk::z0]
		    << " :: x :: " << trkx
		    << " :: y :: " << trky
		    << " :: z :: " << trkz <<endmsg;
	    }
	    msg() << MSG::VERBOSE << endmsg 
		  << "========================== back ========================== " << endmsg << endmsg; 
	    ((*t)->trackParameters()->back())->dump(msg());
	    msg() << MSG::VERBOSE << endmsg 
		  << "========================================================== " << endmsg << endmsg; 
	    msg() << MSG::VERBOSE << endmsg 
		  << "========================== front ========================== " << endmsg << endmsg; 
	    ((*t)->trackParameters()->front())->dump(msg());
	    msg() << MSG::VERBOSE << endmsg 
		  << "=========================================================== " << endmsg << endmsg; 
	    ///////////////////////////////////////
	  }
	}
	///////////////////////////////////////
	if (msgLvl() <= MSG::DEBUG) {
	  msg() <<MSG::DEBUG 
		<< "TRACK_COUNTER = " << TRACK_COUNTER 
		<< " ====> Found " << TRACK_COUNTER << " track(s) with this seed. " << endmsg;
	}
	///////////////////////////////////////
	  
	  
      ++iter;
	
    }
      
  } //external seeding
    /*
      delete newTracks;
    */
    //    
    
    
    
  if(doTiming()){
    if (m_nGoodSeeds) m_AvtimeGoodSeeds=m_timeGoodSeeds/m_nGoodSeeds;
    if (m_nBadSeeds)  m_AvtimeBadSeeds=m_timeBadSeeds/m_nBadSeeds;
  }
    
    
  
  
  m_trackmaker->endEvent(trackEventData);

  if (m_fastTracking){
    // Remove shared tracks with worse quality
    //
    filterSharedTracks(qualityTrack);
    std::multimap<double,Trk::Track*>::iterator 
      q = qualityTrack.begin(), qe =qualityTrack.end(); 
    for(; q!=qe; ++q) {
      foundTracks->push_back((*q).second); ++m_ntracks; 
    }

  }
    
  if(doTiming()) 
    m_timerTrackMaker->stop();
    
  m_nGoodSeedsTotal = m_nGoodSeeds;
  m_nBadSeedsTotal = m_nBadSeeds;
  m_nseedsTotal += m_nseeds;
  m_ntracksTotal += m_ntracks;
    
  if(m_outputlevel<=0) 
    msg() << MSG::DEBUG << "REGTEST: Number of seeds found = " << m_nseeds << endmsg;
    
    
  // Save reconstructed tracks
  const std::string sgkey = "SPSeeded";
  HLT::ErrorCode ec = attachFeature(outputTE, foundTracks, sgkey);
  if ( ec != HLT::OK ) {
    msg() << MSG::ERROR << "Could not record feature " << sgkey << endmsg;
  } 
    
  // Print common event information
  //
  if(m_outputlevel<=0) {
    msg() << MSG::DEBUG << "REGTEST: Investigated "
	  << std::setw(7) <<  m_nseeds 
	  << " and found "
	  << std::setw(5) << foundTracks->size() 
	  << " tracks stored to SG/" << sgkey
	  << endmsg;
    // m_nprint=1; msg()<<MSG::DEBUG<<(*this)<<endmsg;
  }
    
  if (msgLvl() <= MSG::VERBOSE){
    for (size_t i=0; i<foundTracks->size() ; i++){
      msg() << MSG::VERBOSE << "REGTEST track " << i << endmsg;
      msg() << MSG::VERBOSE << *(foundTracks->at(i)) << endmsg;
    }
  }
    
  return HLT::OK;
}

///////////////////////////////////////////////////////////////////
// Finalize
///////////////////////////////////////////////////////////////////
HLT::ErrorCode InDet::SiTrigSPSeededTrackFinder::hltFinalize() {
  
  m_nprint=2; msg()<<MSG::INFO<<(*this)<<endmsg;
  msg() << MSG::INFO << "REGTEST Investigated " << m_nseedsTotal
	<< " seeds and found " << m_ntracksTotal 
	<< ". Invoked " << m_nInvoked << " times." << endmsg;
  return HLT::OK;
}

///////////////////////////////////////////////////////////////////
// Overload of << operator MsgStream
///////////////////////////////////////////////////////////////////
MsgStream& InDet::operator    << 
  (MsgStream& sl,const InDet::SiTrigSPSeededTrackFinder& se)
{ 
  return se.dump(sl);
}

///////////////////////////////////////////////////////////////////
// Overload of << operator std::ostream
///////////////////////////////////////////////////////////////////
std::ostream& InDet::operator << 
  (std::ostream& sl,const InDet::SiTrigSPSeededTrackFinder& se)
{
  return se.dump(sl);
}   

///////////////////////////////////////////////////////////////////
// Dumps relevant information into the MsgStream
///////////////////////////////////////////////////////////////////
MsgStream& InDet::SiTrigSPSeededTrackFinder::dump( MsgStream& out ) const
{
  out<<std::endl;
  if(m_nprint)  return dumpevent(out);
  return dumptools(out);
}

///////////////////////////////////////////////////////////////////
// Dumps conditions information into the MsgStream
///////////////////////////////////////////////////////////////////
MsgStream& InDet::SiTrigSPSeededTrackFinder::dumptools( MsgStream& out ) const
{
  int n = 65-m_zvertexmaker.type().size();
  std::string s1; for(int i=0; i<n; ++i) s1.append(" "); s1.append("|");
  n     = 65-m_seedsmaker.type().size();
  std::string s2; for(int i=0; i<n; ++i) s2.append(" "); s2.append("|");
  n     = 65-m_trackmaker.type().size();
  std::string s3; for(int i=0; i<n; ++i) s3.append(" "); s3.append("|");
  std::string s4; for(int i=0; i<n; ++i) s4.append(" "); s4.append("|");
  
  out<<"|----------------------------------------------------------------"
     <<"--------------------------------|"
     <<std::endl;
  if(m_useZvertexTool)
    out<<"| Tool for primary vertices z-coordinates finding | "<<m_zvertexmaker.type()<<s1
       <<std::endl;
  out<<"| Tool for space points seeds             finding | "<<m_seedsmaker.type()<<s2
     <<std::endl;
  out<<"| Tool for space points seeded track      finding | "<<m_trackmaker.type()<<s3
     <<std::endl;
  out<<"|----------------------------------------------------------------"
     <<"--------------------------------|"
     <<std::endl;
  return out;
}
///////////////////////////////////////////////////////////////////
// Dumps event information into the ostream
///////////////////////////////////////////////////////////////////
MsgStream& InDet::SiTrigSPSeededTrackFinder::dumpevent( MsgStream& out ) const
{
  int ns = m_nseeds;
  int nt = m_ntracks;
  if(m_nprint > 1) {ns = m_nseedsTotal; nt = m_ntracksTotal;}

  out<<"|-------------------------------------------------------------------";
  out<<"-----------------------------|"
     <<std::endl;
  out<<"|  Investigated "
     <<std::setw(7)<<ns<<" space points seeds and found "
     <<std::setw(7)<<nt<<" tracks                              |"
     <<std::endl;
  out<<"|-------------------------------------------------------------------";
  out<<"-----------------------------|"
     <<std::endl;
  return out;
}
///////////////////////////////////////////////////////////////////
// Dumps relevant information into the ostream
///////////////////////////////////////////////////////////////////
std::ostream& InDet::SiTrigSPSeededTrackFinder::dump( std::ostream& out ) const
{
  return out;
}


///////////////////////////////////////////////////////////////////
// Track quality calculation
///////////////////////////////////////////////////////////////////

double InDet::SiTrigSPSeededTrackFinder::trackQuality(const Trk::Track* Tr)
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
   if(q < 0.) q = 0.;
   quality+=q;
 }
 return quality;
}

///////////////////////////////////////////////////////////////////
// Filter shared tracks
///////////////////////////////////////////////////////////////////

void InDet::SiTrigSPSeededTrackFinder::filterSharedTracks
(std::multimap<double,Trk::Track*>& QT)
{
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
      for(int n=0; n!=nf; ++n) clusters.insert(prd[n]);
      ++q;
    } 
    else  {
      delete (*q).second; QT.erase(q++);
    }
  }
}

