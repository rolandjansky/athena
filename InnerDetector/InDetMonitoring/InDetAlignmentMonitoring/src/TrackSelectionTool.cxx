/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// **********************************************************************
// TrackSelectionTool.cxx
// AUTHORS: Ben Cooper
// **********************************************************************

#include "TrkToolInterfaces/ITrackSelectorTool.h"
#include "InDetAlignmentMonitoring/TrackSelectionTool.h"



#include "TrkTrack/Track.h"
#include "TrkTrack/TrackCollection.h"
#include "TrkTrackSummary/TrackSummary.h"
#include "CommissionEvent/ComTime.h"
#include "VxVertex/VxContainer.h"
#include "VxVertex/RecVertex.h"

InDetAlignMon::TrackSelectionTool::TrackSelectionTool( const std::string & type, const std::string & name, const IInterface* parent )
  :AthAlgTool( type, name, parent )
{
  m_trackSelectorTool = ToolHandle<Trk::ITrackSelectorTool>("InDet::InDetDetailedTrackSelectorTool");

  declareInterface<TrackSelectionTool>(this);
  declareProperty("PassAllTracks", m_passAllTracks = false);
  declareProperty("TrackSelectorTool", m_trackSelectorTool);
  declareProperty("DoEventPhaseCut", m_doEventPhaseCut=false);
  declareProperty("MinEventPhase", m_minEventPhase=5);
  declareProperty("MaxEventPhase", m_maxEventPhase=30);
  declareProperty("CommTimeName", m_commTimeName="TRT_Phase");
  declareProperty("UsePrimaryVertex", m_usePrimVtx=false);
  declareProperty("PrimVtxContainerName", m_VtxContainerName="VxPrimaryCandidate");
  declareProperty("MinTracksPerVtx", m_minTracksPerVtx=3);
  
}

//---------------------------------------------------------------------------------------

InDetAlignMon::TrackSelectionTool::~TrackSelectionTool() { }


//---------------------------------------------------------------------------------------

StatusCode InDetAlignMon::TrackSelectionTool::initialize()
{
  
  // get TrackSelectorTool
  if ( m_trackSelectorTool.retrieve().isFailure() ) {
    msg(MSG::FATAL) << "Failed to retrieve tool " << m_trackSelectorTool << endreq;
    return StatusCode::FAILURE;
  } else {
    if(msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) << "Retrieved tool " << m_trackSelectorTool << endreq;
  }

  msg(MSG::INFO) << "TrackSelectionTool initialized" << endreq;

  return StatusCode::SUCCESS;

}


//---------------------------------------------------------------------------------------

StatusCode InDetAlignMon::TrackSelectionTool::finalize()
{

  return StatusCode::SUCCESS;
}


//---------------------------------------------------------------------------------------

DataVector<Trk::Track>* InDetAlignMon::TrackSelectionTool::selectTracks(const std::string &trackColName)
{

  //if this method is used the decision on which trackcollection
  //is made by the calling method
  //returns a view to a new track collection object which contains the selected tracks

  const TrackCollection* tracks; //pointer to original track collection in StoreGate
  DataVector<Trk::Track>* selected_tracks = new DataVector<Trk::Track>(SG::VIEW_ELEMENTS); //new track collection view
  
  const Trk::RecVertex* pVtx = NULL;
  if(m_usePrimVtx){

    //get primary vertex container
    const VxContainer* vxContainer = 0;
    StatusCode sc = evtStore()->retrieve (vxContainer,m_VtxContainerName);
    if (sc.isFailure()) {
      if (msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) << "No PrimVtxCollection with name  "<<m_VtxContainerName<<" found in StoreGate" << endreq;
      return selected_tracks; //return empty track collection (but not NULL)
    } else {
      if (msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) << "PrimVtxCollection with name  "<<m_VtxContainerName<< " with nvertices =  " << vxContainer->size() <<" found  in StoreGate" << endreq;
    }

    //loop over vertices and look for good primary vertex
    for (VxContainer::const_iterator vxIter = vxContainer->begin(); vxIter != vxContainer->end(); ++vxIter) {

      // Select good primary vertex
      if ((*vxIter)->vertexType() != Trk::PriVtx) continue;
      if ((*vxIter)->recVertex().fitQuality().numberDoF() <= 0) continue;
      std::vector<Trk::VxTrackAtVertex*>* vxTrackAtVertex = (*vxIter)->vxTrackAtVertex();
      if (vxTrackAtVertex==0 || vxTrackAtVertex->size() < m_minTracksPerVtx) continue;
      if (msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) << "Found a primary vertex built with " << vxTrackAtVertex->size() << " tracks" << endreq;
      pVtx = &((*vxIter)->recVertex());//set pointer to identified primary vertex
      break;//best pvtx is the first one, so can quit loop once find it
    }
  }

  //retrieve the track collection from StoreGate to which the selection will be applied
  //if track collection cannot be found an empty track collection is returned
  StatusCode sc = evtStore()->retrieve(tracks,trackColName);
  if (sc.isFailure()) {
    if(msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) << "No TrackCollection with name "<<trackColName<<" found in StoreGate" << endreq;
    return selected_tracks; //return empty track collection (but not NULL)
  } else {
    if(msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) << "TrackCollection with name "<<trackColName<<" found in StoreGate" << endreq;
    if(msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) << "Retrieved "<< tracks->size() <<" reconstructed tracks from StoreGate" << endreq;
  }
    
  TrackCollection::const_iterator trksItr  = tracks->begin();
  TrackCollection::const_iterator trksItrE = tracks->end();
  for (; trksItr != trksItrE; ++trksItr) {
    
    Trk::Track* track = *trksItr;

    if(m_passAllTracks) {
      if(msgLvl(MSG::VERBOSE)) msg(MSG::VERBOSE) << "Track automatically passes TrackSelectionTool since passAllTracks=True" << endreq;
      selected_tracks->push_back(track);//allow all tracks into new collection, regardless of decision 
    }
    else {
      if(msgLvl(MSG::VERBOSE)) msg(MSG::VERBOSE) << "Testing track using trackSelectorTool..." << endreq;
      
      bool trackPassed = false;
      if(pVtx){
	if (msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) << "Using primary vertex in track selection" << endreq;
	trackPassed = m_trackSelectorTool->decision(*track,pVtx);
      }
      else {
	if (msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) << "Not using primary vertex in track selection" << endreq;
	trackPassed = m_trackSelectorTool->decision(*track);
      }

      if(m_doEventPhaseCut){
	// cut on the TRT_Phase (ie: the Event Phase)
	float eventPhase=-99.0;
	
	ComTime* theComTime(0);
	StatusCode sc = evtStore()->retrieve(theComTime, m_commTimeName);
	if(sc.isFailure()){
	  if(msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) << "ComTime object not found with name TRT_Phase !!!" << endreq;
	  trackPassed = false;
	}
	
	// get the event phase (one for the entire event)
	if(theComTime){
	  eventPhase = theComTime->getTime();
	}
	
	if(msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) << "Event phase is " << eventPhase << endreq;
	
	// Cut on event phase
	if(eventPhase == -99.0 || eventPhase <= m_minEventPhase || eventPhase >= m_maxEventPhase) {
	  trackPassed = false;
	}
	
      }
      
      if(trackPassed) {
	selected_tracks->push_back(track);//allow only tracks that pass decision into the new collection    
	if(msgLvl(MSG::VERBOSE)) msg(MSG::VERBOSE) << "Track passed trackSelectorTool" << endreq;
      }
      else if(msgLvl(MSG::VERBOSE)) msg(MSG::VERBOSE) << "Track failed trackSelectorTool" << endreq;
    }
  }

  return selected_tracks;
}

//---------------------------------------------------------------------------------------

DataVector<Trk::Track>* InDetAlignMon::TrackSelectionTool::selectTracks()
{

  //if this method the decision on which trackcollection
  //is made from the configuration of the TrackSlectionTool (in jobOptions)
  //returns a view to a new track collection object which contains the selected tracks

  const TrackCollection* tracks;
  DataVector<Trk::Track>* selected_tracks = new DataVector<Trk::Track>(SG::VIEW_ELEMENTS);

  StatusCode sc = evtStore()->retrieve(tracks,m_trackColName);
  if (sc.isFailure()) {
    if(msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) << "No TrackCollection with name "<<m_trackColName<<" found in StoreGate" << endreq;
    return selected_tracks; //return empty track collection (but not NULL)
  } else {
    if(msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) << "TrackCollection with name "<<m_trackColName<<" found in StoreGate" << endreq;
    if(msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) << "Retrieved "<< tracks->size() <<" reconstructed tracks from StoreGate" << endreq;
  }

  TrackCollection::const_iterator trksItr  = tracks->begin();
  TrackCollection::const_iterator trksItrE = tracks->end();
  for (; trksItr != trksItrE; ++trksItr) {
    
    Trk::Track* track = *trksItr;

    if(m_passAllTracks) {
      selected_tracks->push_back(track);//allow all tracks into new collection, regardless of decision 
    }
    else {
      bool trackPassed = m_trackSelectorTool->decision(*track);
      if(trackPassed) selected_tracks->push_back(track);//allow only tracks that pass decision into the new collection    
    }
  }

  return selected_tracks;
}

