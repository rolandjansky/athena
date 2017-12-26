/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "InDetTrigTrackSlimmer/TrigTrackSlimmer.h"
#include "TrkTrack/Track.h"
#include "GaudiKernel/MsgStream.h"
#include "TrkToolInterfaces/ITrackSlimmingTool.h"
#include "TrkToolInterfaces/ITrackSummaryTool.h"

//#include "TrkTrack/TrackCollection.h"
//#include "GaudiKernel/ITHistSvc.h"
//#include <TH1F.h>

namespace InDet
{

  TrigTrackSlimmer::TrigTrackSlimmer(const std::string &name, 
					   ISvcLocator *pSvcLocator)
    : HLT::FexAlgo (name, pSvcLocator),
      m_trackSlimmingTool("Trk::TrkTrackSlimmingTool/TrkTrackSlimmingTool"),
      m_sumTool("Trk::TrackSummaryTool/InDetTrigTrackSummaryTool"),
      m_allTracksFromStoreGate(0),
      m_monitoring(0),
      m_path("/EXPERT")  
  {
    declareProperty("TrackSlimmingTool",   m_trackSlimmingTool);
    declareProperty("SummaryTool",         m_sumTool);
    declareProperty("doMonitoring",        m_monitoring);
    declareProperty("histoPath",           m_path);

  }

  TrigTrackSlimmer::~TrigTrackSlimmer()
  {}

  //----------------------------------
  //          beginRun method:
  //----------------------------------------------------------------------------
  HLT::ErrorCode TrigTrackSlimmer::hltBeginRun() {
    msg() << MSG::INFO << "TrigTrackSlimmer::beginRun()" << endmsg;

    return HLT::OK;
  }
  //----------------------------------------------------------------------------

  ///////////////////////////////////////////////////////////////////
  // Initialisation
  ///////////////////////////////////////////////////////////////////
  HLT::ErrorCode TrigTrackSlimmer::hltInitialize() {
    
    msg() << MSG::DEBUG << "initialize() success" << endmsg;
    
    if ( m_trackSlimmingTool.retrieve().isFailure() ) {
      msg() << MSG::FATAL << "Failed to retrieve tool " << m_trackSlimmingTool << endmsg;
      return HLT::ErrorCode(HLT::Action::ABORT_JOB, HLT::Reason::BAD_JOB_SETUP);
    }
    else{
      msg() << MSG::INFO << "Retrieved tool " << m_trackSlimmingTool << endmsg;
    }

    if ( m_sumTool.retrieve().isFailure()){
      msg() << MSG::FATAL << "Failed to retrieve trk::summary tool " << m_sumTool << endmsg;
      return HLT::ErrorCode(HLT::Action::ABORT_JOB, HLT::Reason::BAD_JOB_SETUP);
    } else {
      msg() << MSG::INFO << "Retrieved tool " << m_sumTool << endmsg;
    }

    
    return HLT::OK;
  }

  ///////////////////////////////////////////////////////////////////
  // Execute HLT Algorithm
  ///////////////////////////////////////////////////////////////////
  HLT::ErrorCode TrigTrackSlimmer::hltExecute(const HLT::TriggerElement*, HLT::TriggerElement* outputTE) {
 

    int outputLevel = msgLvl();
    int inputTracks(-1);

    if(outputLevel <= MSG::DEBUG)
      msg() << MSG::DEBUG << "execHLTAlgorithm()" << endmsg;

    //----------------------------------------------------------------------
    //  Navigate throw the trigger element to retrieve the last TrackCollection
    //----------------------------------------------------------------------

    //initialize monitored objects
    m_allTracksFromStoreGate = 0;
    if ( HLT::OK != getFeature(outputTE, m_allTracksFromStoreGate) ) {
      msg() << MSG::ERROR << " Input track collection could not be found " << endmsg;
     
      return HLT::NAV_ERROR;
    }

    TrackCollection* slimmedTracks = new TrackCollection;

    if(!m_allTracksFromStoreGate){
      msg() << MSG::INFO
	    << " Input track collection was not attached. Algorithm not executed!" << endmsg;
    } else {
      inputTracks = m_allTracksFromStoreGate->size();
      if(outputLevel <= MSG::VERBOSE)
	msg() << MSG::VERBOSE << " Input track collection has size " << inputTracks << endmsg;

      for (TrackCollection::const_iterator it = m_allTracksFromStoreGate->begin() ;
	   it != m_allTracksFromStoreGate->end()   ; ++it) {

	if (!m_sumTool.empty()) {
          // FIXME: const_cast
	  m_sumTool->updateTrack(*const_cast<Trk::Track*>(*it));
	}
	Trk::Track* slimmed = m_trackSlimmingTool->slim(**it);
	slimmedTracks->push_back(slimmed);
      }
      
    } 

    if ( HLT::OK !=  attachFeature(outputTE, slimmedTracks, name()) ) {
      msg() << MSG::ERROR << "Could not attach feature to the TE" << endmsg;
      
      return HLT::NAV_ERROR;
    }
    else{
      if(outputLevel <= MSG::DEBUG){
	msg() << MSG::DEBUG << "REGTEST: after TrackSlimmer  " << slimmedTracks->size()
	      << " of " <<  inputTracks << " tracks." << endmsg;
      }
    }



    return HLT::OK;
  }
  ///////////////////////////////////////////////////////////////////
  // Finalize
  ///////////////////////////////////////////////////////////////////
  
  HLT::ErrorCode TrigTrackSlimmer::hltFinalize() {

    msg() << MSG::DEBUG << "finalize() success" << endmsg;
    return HLT::OK;
  }

  //----------------------------------
  //          endRun method:
  //----------------------------------------------------------------------------
  HLT::ErrorCode TrigTrackSlimmer::hltEndRun() {
   
    msg() << MSG::INFO << "TrigTrackSlimmer::endRun()" << endmsg;
   
    return HLT::OK;
  }
  //---------------------------------------------------------------------------
} // end namespace


