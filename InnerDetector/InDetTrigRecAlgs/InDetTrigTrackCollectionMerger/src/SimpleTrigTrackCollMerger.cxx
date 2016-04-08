/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "InDetTrigTrackCollectionMerger/SimpleTrigTrackCollMerger.h"

#include "TrkToolInterfaces/ITrackSummaryTool.h"
#include "TrkToolInterfaces/IPRD_AssociationTool.h"

namespace Trk {

  SimpleTrigTrackCollMerger::SimpleTrigTrackCollMerger(const std::string &name, 
						       ISvcLocator *pSvcLocator)
    : HLT::FexAlgo (name, pSvcLocator),
      m_assoTool("InDet::InDetPRD_AssociationToolGangedPixels/InDetTrigPrdAssociationTool"),
      m_sumTool("Trk::TrackSummaryTool/InDetTrigTrackSummaryTool")
  {
    declareProperty("AssociationTool",    m_assoTool);
    declareProperty("SummaryTool",        m_sumTool);
    declareProperty("InputCollections",   m_inputColl);
    declareProperty("OutputCollection",   m_outputColl="CombinedEFIDTracks");

  }

  SimpleTrigTrackCollMerger::~SimpleTrigTrackCollMerger()
  {}


  //----------------------------------
  //          beginRun method:
  //----------------------------------------------------------------------------
  HLT::ErrorCode SimpleTrigTrackCollMerger::hltBeginRun() {
    msg() << MSG::INFO << name() << "::beginRun()" << endreq;

    return HLT::OK;
  }
  //----------------------------------------------------------------------------

  //----------------------------------
  //          hltInitialize method:
  //----------------------------------------------------------------------------
  HLT::ErrorCode SimpleTrigTrackCollMerger::hltInitialize() {
    msg() << MSG::INFO << name() << "::hltInitialize()" << endreq;

    if (!m_assoTool.retrieve().isSuccess()){
      msg() << MSG::FATAL << "Failed to retrieve association tool " << m_assoTool << endreq;
      return HLT::ErrorCode(HLT::Action::ABORT_JOB, HLT::Reason::BAD_JOB_SETUP);
    } else {
      msg() << MSG::INFO << "Retrieved tool " << m_assoTool << endreq;
    }

    if (!m_sumTool.retrieve().isSuccess()){
      msg() << MSG::FATAL << "Failed to retrieve trk::summary tool " << m_sumTool << endreq;
      return HLT::ErrorCode(HLT::Action::ABORT_JOB, HLT::Reason::BAD_JOB_SETUP);
    } else {
      msg() << MSG::INFO << "Retrieved tool " << m_sumTool << endreq;
    }

    return HLT::OK;
  }
  //----------------------------------------------------------------------------


  //----------------------------------
  //          hltExecute method:
  //----------------------------------------------------------------------------
  
  HLT::ErrorCode SimpleTrigTrackCollMerger::hltExecute(const HLT::TriggerElement*, HLT::TriggerElement* outputTE){
    
    size_t msglvl = msgLvl();
    //    bool   merged(false);

    m_assoTool->reset();

    const TrackCollection *tracks;
    TrackCollection *mergedTracks = new TrackCollection;

    std::vector<std::string>::const_iterator collname, lastcoll;
    collname = m_inputColl.begin();
    lastcoll = m_inputColl.end();

    for (; collname!=lastcoll; ++collname){
      if (HLT::OK != getFeature(outputTE, tracks, *collname)){
	if (msglvl<=MSG::DEBUG){
	  msg() << MSG::DEBUG << "Input collection " << *collname
		<< " not available in the navigation" << endreq;
	}
      } 
      else {
	if (tracks && msglvl<=MSG::DEBUG)
	  msg() << MSG::DEBUG << "input track coll: " << *collname <<
	    " with size" << tracks->size() << endreq;
	if (HLT::OK != mergeTrack(tracks, mergedTracks)){
	  msg() << MSG::ERROR << "merging failed  for " << *collname << endreq; 
	  delete mergedTracks; mergedTracks=0;
	  return HLT::NAV_ERROR;
	} else {
	  if (msglvl<=MSG::DEBUG) msg() << MSG::DEBUG << "REGTEST after merging "
					<< mergedTracks->size() << " tracks" << endreq;
	}
      }
    }


    // now loop over all track and update summaries with new shared hit counts
    TrackCollection::const_iterator rf,rfe=mergedTracks->end();
    for(rf=mergedTracks->begin();rf!=rfe; ++rf) m_sumTool->updateTrack(**rf);
	
    if ( HLT::OK !=  attachFeature(outputTE, mergedTracks, m_outputColl) ) {
      msg() << MSG::ERROR << "Could not attach feature to the TE" << endreq;
    }

    return HLT::OK;
  }
  

  //----------------------------------
  //          mergeTrack 
  //----------------------------------------------------------------------------

  
  HLT::ErrorCode  SimpleTrigTrackCollMerger::mergeTrack(const TrackCollection* trackCol, TrackCollection* outputCol)
  {
    
    size_t msglvl = msgLvl();
    
    // loop over forward track, accept them and add them imto association tool
    if(trackCol && trackCol->size()) {
      if (msglvl <= MSG::DEBUG) msg() << MSG::DEBUG << "Size of track collection " << trackCol->size() << endreq;
      
      // loop over tracks
      TrackCollection::const_iterator rf,rfe=trackCol->end();
      for(rf=trackCol->begin();rf!=rfe; ++rf){
	
	// add track into output
	Trk::Track* newTrack = new Trk::Track(**rf);
	outputCol->push_back(newTrack);
	
	// add tracks into PRD tool
	if (m_assoTool->addPRDs(*newTrack).isFailure())
	  msg() << MSG::WARNING << "Failed to add PRDs to map" << endreq;
	
      }
      if (msglvl <= MSG::DEBUG) msg() << MSG::DEBUG << "Size of the output track coll " << outputCol->size() << endreq;
    }
	
    return HLT::OK;
  }

  //----------------------------------
  //          endRun method:
  //----------------------------------------------------------------------------
  HLT::ErrorCode SimpleTrigTrackCollMerger::hltEndRun() {
    msg() << MSG::INFO << name() << "::hltEndRun()" << endreq;

    return HLT::OK;
  }
  //----------------------------------------------------------------------------

  //----------------------------------
  //          finalize method:
  //----------------------------------------------------------------------------
  HLT::ErrorCode SimpleTrigTrackCollMerger::hltFinalize() {
    msg() << MSG::INFO << name() << "::hltFinalize()" << endreq;

    return HLT::OK;
  }
  //----------------------------------------------------------------------------

}
