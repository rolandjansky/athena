/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// **************************************************************************
#include "InDetTrigPriVxFinder/TrigVxPrimaryAllTE.h"
#include "StoreGate/StoreGateSvc.h"
#include "InDetRecToolInterfaces/IVertexFinder.h"
#include "TrkTrack/Track.h"
#include "TrkTrack/TrackCollection.h"
#include "TrkParticleBase/TrackParticleBase.h"
#include "TrkParameters/TrackParameters.h"
#include "xAODTracking/VertexContainer.h"
#include "xAODTracking/VertexAuxContainer.h"
#include "TrkEventPrimitives/ParamDefs.h"

#include "MagFieldInterfaces/IMagFieldSvc.h"
#include "InDetBeamSpotService/IBeamCondSvc.h"

namespace InDet
{
  
  TrigVxPrimaryAllTE::TrigVxPrimaryAllTE(const std::string &n, ISvcLocator *pSvcLoc)
    : HLT::AllTEAlgo(n, pSvcLoc),
      m_runWithoutField(false),
      m_VertexFinderTool("InDet::InDetPriVxFinderTool/InDetTrigPriVxFinderTool"),
      m_fieldSvc("AtlasFieldSvc", n),
      m_BeamCondSvc("BeamCondSvc", n),
      m_retrieve_tracks_from_SG(false),
      m_track_collection_from_SG("Unconfigured_TrigVxPrimaryAllTE_For_SG_Access")
  {
    declareProperty("VertexFinderTool",m_VertexFinderTool);
    declareProperty("RunWithoutField", m_runWithoutField, "It may be unsafe to run vertexing w/o field on");
    declareProperty("BeamCondSvc", m_BeamCondSvc);

    //
    declareProperty("RetrieveTracksFromSG", m_retrieve_tracks_from_SG,
		    "Non default mode of operation, mainly for testing");
    declareProperty("TrackCollectionFromSG", m_track_collection_from_SG,
		    "Non default mode of operation, mainly for testing");
    
    declareMonitoredVariable("numTracks", m_nTracks   );
    declareMonitoredVariable("numVertices", m_nVertices    );
    declareMonitoredStdContainer("zOfPriVtx", m_zOfPriVtx);
    declareMonitoredStdContainer("zOfPileUp", m_zOfPileUp);
    declareMonitoredStdContainer("zOfNoVtx", m_zOfNoVtx);
  }

  //-------------------------------------------------------------------------
  TrigVxPrimaryAllTE::~TrigVxPrimaryAllTE()
  {}
  //-------------------------------------------------------------------------
  HLT::ErrorCode TrigVxPrimaryAllTE::hltInitialize() {
    
    msg() << MSG::INFO << "TrigVxPrimaryAllTE::initialize(). "<< endreq;
    
    /* Get the VertexFinderTool */
    if ( m_VertexFinderTool.retrieve().isFailure() ) {
      msg() << MSG::FATAL << "Failed to retrieve tool " << m_VertexFinderTool << endreq;
      return HLT::ErrorCode(HLT::Action::ABORT_JOB, HLT::Reason::BAD_JOB_SETUP);
    }
    else{
      msg() << MSG::INFO << "Retrieved tool " << m_VertexFinderTool << endreq;
    }
    
    if (m_fieldSvc.retrieve().isFailure()){
      msg() << MSG::FATAL << "Failed to retrieve service " << m_fieldSvc << endreq;
      return HLT::ErrorCode(HLT::Action::ABORT_JOB, HLT::Reason::BAD_JOB_SETUP);
    } 
    else {
      msg() << MSG::INFO << "Retrieved tool " << m_fieldSvc << endreq;
    }

    if (m_BeamCondSvc.retrieve().isFailure()){
      msg() << MSG::FATAL << "Failed to retrieve tool " << m_BeamCondSvc << endreq;
      return HLT::ErrorCode(HLT::Action::ABORT_JOB, HLT::Reason::BAD_JOB_SETUP);
    }
    else {
      msg() << MSG::INFO << "Retrieved service " << m_fieldSvc << endreq;
    }

    return HLT::OK;
  }
  

  ///////////////////////////////////////////////////////////////////
  // Execute HLT algorithm
  ///////////////////////////////////////////////////////////////////
  HLT::ErrorCode TrigVxPrimaryAllTE::hltExecute(std::vector<std::vector<HLT::TriggerElement*> >& inputTE, unsigned int output) {
    
    
    m_nTracks = 0;
    m_nVertices = 0;
    m_zOfPriVtx.clear();
    m_zOfPileUp.clear();
    m_zOfNoVtx.clear();
    bool runVtx(true);
    HLT::TEVec allTEs;
    
    int outputLevel = msgLvl();
    
    if(outputLevel <= MSG::DEBUG)
      msg() << MSG::DEBUG << " In execHLTAlgorithm()" << endreq;
    

    /************************************/
    /** CREATE NEW TRACK COLLECTION *****/
    /************************************/
    TrackCollection* trackTES_All = new TrackCollection;

    const TrackCollection* pointerToTrackCollections = 0;

    if (!m_retrieve_tracks_from_SG){
      for (HLT::TEVec::const_iterator inner_it = inputTE[0].begin(); inner_it != inputTE[0].end(); ++inner_it) {
	
	if ( HLT::OK != getFeature(*inner_it, pointerToTrackCollections) || !pointerToTrackCollections) {
	  msg() << MSG::ERROR << "Input track collection could not be found " << endreq;
	  return HLT::NAV_ERROR;
	} 
	
	for (TrackCollection::const_iterator it = pointerToTrackCollections->begin() ;
	     it != pointerToTrackCollections->end()   ; ++it) {
	  
	  Trk::Track* track = *it;
	  trackTES_All->push_back(track);  
	}
      }
    } else {
      const std::string tracksName= m_track_collection_from_SG;
      //retrieve tracks from SG not from Navigation
      if (store()->transientContains<TrackCollection>(tracksName)) {
	msg() << MSG::DEBUG << "sg contains  "<< tracksName << endreq;
      }
      else {
	msg() << MSG::DEBUG << "sg does not contain  "<< tracksName << endreq;
	runVtx=false;
      }
      StatusCode sc = store()->retrieve(pointerToTrackCollections,tracksName);
      if (!sc){
	msg() << MSG::DEBUG << "sg does not contain  "<< tracksName << endreq;
	runVtx=false;
      }
      else {
	for (TrackCollection::const_iterator it = pointerToTrackCollections->begin() ;
	     it != pointerToTrackCollections->end()   ; ++it) {
	  
	  Trk::Track* track = *it;
	  trackTES_All->push_back(track);  
	}
      }

    }

    
    if (!trackTES_All){
	if(outputLevel <= MSG::DEBUG)
	  msg() << MSG::DEBUG
		<< "Input tracks by getFeature NULL, a previous algo attached nothing?" << endreq;
	runVtx = false;
      }
      else if(trackTES_All->size()==0){
	if(outputLevel <= MSG::DEBUG)
	  msg() << MSG::DEBUG << " Input track collection has 0 size. Algorithm not executed!" << endreq;
	runVtx = false;
      }
      
      if (!m_runWithoutField && !m_fieldSvc->solenoidOn()){
	if(outputLevel <= MSG::DEBUG)
	  msg() << MSG::DEBUG << "Solenoid Off and RunWithoutField=False. Algorithm not executed!" << endreq;
	runVtx = false;
      }
      
      if (runVtx){
	m_nTracks = trackTES_All->size();
	if (outputLevel <= MSG::DEBUG)
	  msg() << MSG::DEBUG << "REGTEST: Retrieved input track collection with "
		<< m_nTracks << " tracks. " << endreq;
	//check whether measured perigees are available, if not vertexing crashes
	for (int i=0; i<m_nTracks; i++){
	  const Trk::Perigee *mp = (trackTES_All->at(i)->perigeeParameters());
	  if (mp){
	    if (std::isnan(mp->parameters()[Trk::d0])){
	      if(outputLevel <= MSG::DEBUG)
		msg() << MSG::DEBUG
		      << "Algorithm not executed as measured perigees are not valid" << endreq;
	      runVtx = false;
	    }
	  }
	  else {
	    if(outputLevel <= MSG::DEBUG)
	      msg() << MSG::DEBUG
		    << "Algorithm not executed as measured perigees are empty" << endreq;
	    runVtx = false;
	  }
	}
      }
    
    // Create the xAOD container and its auxiliary store:
    xAOD::VertexContainer* theVxContainer = 0;
    xAOD::VertexAuxContainer* theVxAuxContainer = 0;
    std::pair<xAOD::VertexContainer*,xAOD::VertexAuxContainer*> theVxContainers
	= std::make_pair( theVxContainer, theVxAuxContainer );

    if (runVtx) {
      theVxContainers = m_VertexFinderTool->findVertex(trackTES_All);
      theVxContainer = theVxContainers.first;
      theVxAuxContainer = theVxContainers.second;  // the vertex finder has already called setStore
    }
    
    if (!runVtx || !theVxContainers.first){  // create and fill place-holder VertexContainer if necessary
      theVxContainer = new xAOD::VertexContainer();
      theVxAuxContainer = new xAOD::VertexAuxContainer(); 
      theVxContainer->setStore(theVxAuxContainer);
      theVxContainers.first = theVxContainer;
      theVxContainers.second = theVxAuxContainer;
      
      xAOD::Vertex *dummyvtx = new xAOD::Vertex();
      theVxContainer->push_back(dummyvtx);  // put vertex in container so it has Aux store
      dummyvtx->setVertexType(xAOD::VxType::NoVtx);  // now safe to set member variable
      dummyvtx->setPosition( m_BeamCondSvc->beamVtx().position() );
      dummyvtx->setCovariancePosition( m_BeamCondSvc->beamVtx().covariancePosition() );
      dummyvtx->vxTrackAtVertex() = std::vector<Trk::VxTrackAtVertex>();

    }
    

    
    //
    //  Attach resolved tracks to the trigger element.
    // create output TE:         
    // Create an output TE seeded by the inputs                                                                                                                
    HLT::TriggerElement* outputTE = config()->getNavigation()->addNode(allTEs, output);
    outputTE->setActiveState(true);


    HLT::ErrorCode stat = attachFeature(outputTE, theVxContainer,"xPrimVx");
    if (stat != HLT::OK) {
      if (msgLvl() <= MSG::WARNING)
	    msg() << MSG::ERROR << "Could not attach feature to the TE" << endreq;
      
      delete theVxAuxContainer; theVxAuxContainer=0;
      return HLT::NAV_ERROR;
    }

    
    
    m_nVertices = theVxContainer->size();
    if(outputLevel <= MSG::DEBUG){
      msg() << MSG::DEBUG << "Container recorded in StoreGate." << endreq;
      msg() << MSG::DEBUG << "REGTEST: Container size :" << m_nVertices << endreq;
    }    
    


    // since this is an AllTEAlgo, we have to call the monitoring ourselves:
    afterExecMonitors().ignore();

    delete theVxAuxContainer; theVxAuxContainer=0;
    
    return HLT::OK;
  }
  //---------------------------------------------------------------------------
  HLT::ErrorCode TrigVxPrimaryAllTE::hltFinalize() {

    msg() << MSG::INFO << "TrigVxPrimaryAllTE::finalize()" << endreq;

    return HLT::OK;
  }

} // end namespace InDet
