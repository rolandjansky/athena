/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// **********************************************************************
// TrackSelectionTool.cxx
// AUTHORS: Ben Cooper
// **********************************************************************

#include "TrkToolInterfaces/ITrackSelectorTool.h"
#include "TrackSelectionTool.h"
#include "InDetTrackSelectionTool/IInDetTrackSelectionTool.h"


#include "TrkTrack/Track.h"
#include "TrkTrack/TrackCollection.h"
#include "TrkTrackSummary/TrackSummary.h"
#include "CommissionEvent/ComTime.h"
#include "VxVertex/VxContainer.h"
#include "VxVertex/RecVertex.h"
#include "AthContainers/ConstDataVector.h"


InDetAlignMon::TrackSelectionTool::TrackSelectionTool( const std::string & type, const std::string & name, const IInterface* parent )
  :AthAlgTool( type, name, parent )
{
  m_trackSelectorTool = ToolHandle<Trk::ITrackSelectorTool>("InDet::InDetDetailedTrackSelectorTool");
  m_idtrackSelectionTool = ToolHandle<InDet::IInDetTrackSelectionTool>("InDetTrackSelectionTool/InDetTrackSelectionTool");
  
  declareInterface<TrackSelectionTool>(this);
  declareProperty("PassAllTracks", m_passAllTracks = false);
  declareProperty("TrackSelectorTool", m_trackSelectorTool);
  declareProperty("IDTrackSelectionTool",m_idtrackSelectionTool);
  declareProperty("UseIDTrackSelectionTool",m_useIDTrackSelectionTool=false);
  declareProperty("DoEventPhaseCut", m_doEventPhaseCut=false);
  declareProperty("MinEventPhase", m_minEventPhase=5);
  declareProperty("MaxEventPhase", m_maxEventPhase=30);
  declareProperty("CommTimeName", m_commTimeName="TRT_Phase");
  declareProperty("UsePrimaryVertex", m_usePrimVtx=false);
  declareProperty("PrimVtxContainerName", m_VtxContainerName="VxPrimaryCandidate");
  declareProperty("MinTracksPerVtx", m_minTracksPerVtx=0);
  
}

//---------------------------------------------------------------------------------------

InDetAlignMon::TrackSelectionTool::~TrackSelectionTool() { }


//---------------------------------------------------------------------------------------

StatusCode InDetAlignMon::TrackSelectionTool::initialize()
{
  
  // get TrackSelectorTool

  
  if ( ! m_useIDTrackSelectionTool )
    {
      if ( m_trackSelectorTool.retrieve().isFailure() ) {
	msg(MSG::FATAL) << "Failed to retrieve tool " << m_trackSelectorTool << endmsg;
	return StatusCode::FAILURE;
      } else {
	if(msgLvl(MSG::INFO)) msg(MSG::INFO) << "Retrieved tool " << m_trackSelectorTool << endmsg;
      }
    }
  else
    {
      if (   m_idtrackSelectionTool.retrieve().isFailure() ) {
	msg(MSG::FATAL) << "Failed to retrieve tool " << m_idtrackSelectionTool << endmsg;
	return StatusCode::FAILURE;
      } else {
	if(msgLvl(MSG::INFO)) msg(MSG::INFO) << "Retrieved tool " << m_idtrackSelectionTool << endmsg;
      }
    }
  
  

  return StatusCode::SUCCESS;

}


//---------------------------------------------------------------------------------------

StatusCode InDetAlignMon::TrackSelectionTool::finalize()
{

  return StatusCode::SUCCESS;
}


//---------------------------------------------------------------------------------------

DataVector<xAOD::TrackParticle>* InDetAlignMon::TrackSelectionTool::selectTracksParticle(const std::string &trackColName)
{

  //if this method is used the decision on which trackcollection
  //is made by the calling method
  //returns a view to a new track collection object which contains the selected tracks

  //const TrackCollection* tracks; //pointer to original track collection in StoreGate
  const xAOD::TrackParticleContainer* tracks;
  DataVector<xAOD::TrackParticle>* return_tracks = new DataVector<xAOD::TrackParticle>(SG::VIEW_ELEMENTS); //new track collection view

  
  const xAOD::Vertex* pVtx = NULL;
  //const Trk::RecVertex* pVtx = NULL;
  if(m_usePrimVtx){

    //get primary vertex container
    //const VxContainer* vxContainer = 0;
    const xAOD::VertexContainer* vxContainer = NULL;
    StatusCode sc = evtStore()->retrieve (vxContainer,m_VtxContainerName);
    if (sc.isFailure()) {
      if (msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) << "No PrimVtxCollection with name  "<<m_VtxContainerName<<" found in StoreGate" << endmsg;
      return return_tracks; //return empty track collection (but not NULL)
    } else {
      if (msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) << "PrimVtxCollection with name  "<<m_VtxContainerName<< " with nvertices =  " << vxContainer->size() <<" found  in StoreGate" << endmsg;
    }

    //loop over vertices and look for good primary vertex
    for (xAOD::VertexContainer::const_iterator vxIter = vxContainer->begin(); vxIter != vxContainer->end(); ++vxIter) {

      // Select good primary vertex
      if ((*vxIter)->vertexType() != xAOD::VxType::PriVtx) continue;
      if ((*vxIter)->numberDoF() <= 0) continue;

      //Since I'm not sure if this decorator is constructed I prefer to not use it
      //std::vector<Trk::VxTrackAtVertex*>* vxTrackAtVertex = (*vxIter)->vxTrackAtVertex();
      //if (!(*vxIter)->vxTrackAtVertexAvailable() || vxTrackAtVertex->size() < m_minTracksPerVtx) continue;
      
      if ((*vxIter)->nTrackParticles()==0 || (*vxIter)->nTrackParticles() < m_minTracksPerVtx) continue;

      if (msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) << "Found a primary vertex built with " << (*vxIter)->nTrackParticles() << " tracks" << endmsg;
      pVtx = *vxIter;//set pointer to identified primary vertex
      break;//best pvtx is the first one, so can quit loop once find it
    }
  }

  //retrieve the track collection from StoreGate to which the selection will be applied
  //if track collection cannot be found an empty track collection is returned
  StatusCode sc = evtStore()->retrieve(tracks,trackColName);
  if (sc.isFailure()) {
    if(msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) << "No TrackCollection with name "<<trackColName<<" found in StoreGate" << endmsg;
    return return_tracks; //return empty track collection (but not NULL)
  } else {
    if(msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) << "TrackCollection with name "<<trackColName<<" found in StoreGate" << endmsg;
    if(msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) << "Retrieved "<< tracks->size() <<" reconstructed tracks from StoreGate" << endmsg;
  }
    
  //TrackCollection::const_iterator trksItr  = tracks->begin();
  //TrackCollection::const_iterator trksItrE = tracks->end();

  xAOD::TrackParticleContainer::const_iterator trksItr  = tracks->begin();
  xAOD::TrackParticleContainer::const_iterator trksItrE = tracks->end();
  for (; trksItr != trksItrE; ++trksItr) {
    
    // Trk::Track* track = *trksItr;
    const xAOD::TrackParticle* track =  *trksItr;     
    

    if(m_passAllTracks) {
      if(msgLvl(MSG::VERBOSE)) msg(MSG::VERBOSE) << "Track automatically passes TrackSelectionTool since passAllTracks=True" << endmsg;
      //selected_tracks->push_back(track);//allow all tracks into new collection, regardless of decision 
      return_tracks->push_back(const_cast<xAOD::TrackParticle*> (track) );
    }
    else {
      if(msgLvl(MSG::VERBOSE)) msg(MSG::VERBOSE) << "Testing track using trackSelectorTool..." << endmsg;
      
      bool trackPassed = false;
      if(pVtx){
	if (msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) << "Using primary vertex in track selection" << endmsg;

	
	if (m_useIDTrackSelectionTool)
	  {
	    if (m_idtrackSelectionTool->accept(*track,pVtx))
	      trackPassed = true;
	  }
	else
	  trackPassed = m_trackSelectorTool->decision(*track,pVtx);
      }
      else {
	if (msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) << "Not using primary vertex in track selection" << endmsg;

	
	if (m_useIDTrackSelectionTool)
	  {
	    if (m_idtrackSelectionTool->accept(*track))
	    trackPassed = true;
	  }
	else
	  trackPassed = m_trackSelectorTool->decision(*track);
      }
      
      if(m_doEventPhaseCut){
	// cut on the TRT_Phase (ie: the Event Phase)
	float eventPhase=-99.0;
	
	ComTime* theComTime(0);
	StatusCode sc = evtStore()->retrieve(theComTime, m_commTimeName);
	if(sc.isFailure()){
	  if(msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) << "ComTime object not found with name TRT_Phase !!!" << endmsg;
	  trackPassed = false;
	}
	
	// get the event phase (one for the entire event)
	if(theComTime){
	  eventPhase = theComTime->getTime();
	}
	
	if(msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) << "Event phase is " << eventPhase << endmsg;
	
	// Cut on event phase
	if(eventPhase == -99.0 || eventPhase <= m_minEventPhase || eventPhase >= m_maxEventPhase) {
	  trackPassed = false;
	}
	
      }
      
      if(trackPassed) {
	//selected_tracks->push_back(track);//allow only tracks that pass decision into the new collection
	return_tracks->push_back(const_cast<xAOD::TrackParticle*> (track));
	if(msgLvl(MSG::VERBOSE)) msg(MSG::VERBOSE) << "Track passed trackSelectorTool" << endmsg;
      }
      else if(msgLvl(MSG::VERBOSE)) msg(MSG::VERBOSE) << "Track failed trackSelectorTool" << endmsg;
    }
  }

  return return_tracks;
}

const DataVector<Trk::Track>* InDetAlignMon::TrackSelectionTool::selectTracks(const std::string &trackColName)
{

  //if this method is used the decision on which trackcollection
  //is made by the calling method
  //returns a view to a new track collection object which contains the selected tracks

  const TrackCollection* tracks; //pointer to original track collection in StoreGate
  auto selected_tracks = std::make_unique<ConstDataVector<DataVector<Trk::Track> > >(SG::VIEW_ELEMENTS); //new track collection view
  
  const Trk::RecVertex* pVtx = NULL;
  if(m_usePrimVtx){

    //get primary vertex container
    const VxContainer* vxContainer = 0;
    StatusCode sc = evtStore()->retrieve (vxContainer,m_VtxContainerName);
    if (sc.isFailure()) {
      if (msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) << "No PrimVtxCollection with name  "<<m_VtxContainerName<<" found in StoreGate" << endmsg;
      return selected_tracks.release()->asDataVector(); //return empty track collection (but not NULL)
    } else {
      if (msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) << "PrimVtxCollection with name  "<<m_VtxContainerName<< " with nvertices =  " << vxContainer->size() <<" found  in StoreGate" << endmsg;
    }

    //loop over vertices and look for good primary vertex
    for (VxContainer::const_iterator vxIter = vxContainer->begin(); vxIter != vxContainer->end(); ++vxIter) {

      // Select good primary vertex
      if ((*vxIter)->vertexType() != Trk::PriVtx) continue;
      if ((*vxIter)->recVertex().fitQuality().numberDoF() <= 0) continue;
      const std::vector<Trk::VxTrackAtVertex*>* vxTrackAtVertex = (*vxIter)->vxTrackAtVertex();
      if (vxTrackAtVertex==0 || vxTrackAtVertex->size() < m_minTracksPerVtx) continue;
      if (msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) << "Found a primary vertex built with " << vxTrackAtVertex->size() << " tracks" << endmsg;
      pVtx = &((*vxIter)->recVertex());//set pointer to identified primary vertex
      break;//best pvtx is the first one, so can quit loop once find it
    }
  }

  //retrieve the track collection from StoreGate to which the selection will be applied
  //if track collection cannot be found an empty track collection is returned
  StatusCode sc = evtStore()->retrieve(tracks,trackColName);
  if (sc.isFailure()) {
    if(msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) << "No TrackCollection with name "<<trackColName<<" found in StoreGate" << endmsg;
    return selected_tracks.release()->asDataVector(); //return empty track collection (but not NULL)
  } else {
    if(msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) << "TrackCollection with name "<<trackColName<<" found in StoreGate" << endmsg;
    if(msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) << "Retrieved "<< tracks->size() <<" reconstructed tracks from StoreGate" << endmsg;
  }
    
  TrackCollection::const_iterator trksItr  = tracks->begin();
  TrackCollection::const_iterator trksItrE = tracks->end();
  for (; trksItr != trksItrE; ++trksItr) {
    
    const Trk::Track* track = *trksItr;

    if(m_passAllTracks) {
      if(msgLvl(MSG::VERBOSE)) msg(MSG::VERBOSE) << "Track automatically passes TrackSelectionTool since passAllTracks=True" << endmsg;
      selected_tracks->push_back(track);//allow all tracks into new collection, regardless of decision 
    }
    else {
      if(msgLvl(MSG::VERBOSE)) msg(MSG::VERBOSE) << "Testing track using trackSelectorTool..." << endmsg;
      
      bool trackPassed = false;
      if(pVtx){
	if (msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) << "Using primary vertex in track selection" << endmsg;

	
	if (m_useIDTrackSelectionTool)
	  {
	    if (m_idtrackSelectionTool->accept(*track,pVtx))
	      trackPassed = true;
	  }
	else
	  trackPassed = m_trackSelectorTool->decision(*track,pVtx);
      }
      else {
	if (msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) << "Not using primary vertex in track selection" << endmsg;

	
	if (m_useIDTrackSelectionTool)
	  {
	    if (m_idtrackSelectionTool->accept(*track))
	    trackPassed = true;
	  }
	else
	  trackPassed = m_trackSelectorTool->decision(*track);
      }
      
      if(m_doEventPhaseCut){
	// cut on the TRT_Phase (ie: the Event Phase)
	float eventPhase=-99.0;
	
	ComTime* theComTime(0);
	StatusCode sc = evtStore()->retrieve(theComTime, m_commTimeName);
	if(sc.isFailure()){
	  if(msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) << "ComTime object not found with name TRT_Phase !!!" << endmsg;
	  trackPassed = false;
	}
	
	// get the event phase (one for the entire event)
	if(theComTime){
	  eventPhase = theComTime->getTime();
	}
	
	if(msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) << "Event phase is " << eventPhase << endmsg;
	
	// Cut on event phase
	if(eventPhase == -99.0 || eventPhase <= m_minEventPhase || eventPhase >= m_maxEventPhase) {
	  trackPassed = false;
	}
	
      }
      
      if(trackPassed) {
	selected_tracks->push_back(track);//allow only tracks that pass decision into the new collection    
	if(msgLvl(MSG::VERBOSE)) msg(MSG::VERBOSE) << "Track passed trackSelectorTool" << endmsg;
      }
      else if(msgLvl(MSG::VERBOSE)) msg(MSG::VERBOSE) << "Track failed trackSelectorTool" << endmsg;
    }
  }

  return selected_tracks.release()->asDataVector();
}


DataVector<Trk::Track>* InDetAlignMon::TrackSelectionTool::selectTracksNew(const std::string &trackColName)
{

  //if this method is used the decision on which trackcollection
  //is made by the calling method
  //returns a view to a new track collection object which contains the selected tracks

  //const TrackCollection* tracks; //pointer to original track collection in StoreGate
  const xAOD::TrackParticleContainer* tracks;
  DataVector<Trk::Track>* return_tracks = new DataVector<Trk::Track>(SG::VIEW_ELEMENTS); //new track collection view
  
  
  const xAOD::Vertex* pVtx = NULL;
  //const Trk::RecVertex* pVtx = NULL;
  if(m_usePrimVtx){

    //get primary vertex container
    //const VxContainer* vxContainer = 0;
    const xAOD::VertexContainer* vxContainer = NULL;
    StatusCode sc = evtStore()->retrieve (vxContainer,m_VtxContainerName);
    if (sc.isFailure()) {
      if (msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) << "No PrimVtxCollection with name  "<<m_VtxContainerName<<" found in StoreGate" << endmsg;
      return return_tracks; //return empty track collection (but not NULL)
    } else {
      if (msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) << "PrimVtxCollection with name  "<<m_VtxContainerName<< " with nvertices =  " << vxContainer->size() <<" found  in StoreGate" << endmsg;
    }

    //loop over vertices and look for good primary vertex
    for (xAOD::VertexContainer::const_iterator vxIter = vxContainer->begin(); vxIter != vxContainer->end(); ++vxIter) {

      // Select good primary vertex
      if ((*vxIter)->vertexType() != xAOD::VxType::PriVtx) continue;
      if ((*vxIter)->numberDoF() <= 0) continue;

      //Since I'm not sure if this decorator is constructed I prefer to not use it
      //std::vector<Trk::VxTrackAtVertex*>* vxTrackAtVertex = (*vxIter)->vxTrackAtVertex();
      //if (!(*vxIter)->vxTrackAtVertexAvailable() || vxTrackAtVertex->size() < m_minTracksPerVtx) continue;

      if ((*vxIter)->nTrackParticles()==0 || (*vxIter)->nTrackParticles() < m_minTracksPerVtx) continue;

      if (msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) << "Found a primary vertex built with " << (*vxIter)->nTrackParticles() << " tracks" << endmsg;
      pVtx = *vxIter;//set pointer to identified primary vertex
      break;//best pvtx is the first one, so can quit loop once find it
    }
  }

  //retrieve the track collection from StoreGate to which the selection will be applied
  //if track collection cannot be found an empty track collection is returned
  //StatusCode sc = evtStore()->retrieve(tracks,trackColName);   //Right one

  StatusCode sc = evtStore()->retrieve(tracks,"InDetTrackParticles");

  if (sc.isFailure()) {
    if(msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) << "No TrackCollection with name "<<trackColName<<" found in StoreGate" << endmsg;
    return return_tracks; //return empty track collection (but not NULL)
  } else {
    if(msgLvl(MSG::INFO)) msg(MSG::INFO) << "TrackCollection with name "<<trackColName<<" found in StoreGate" << endmsg;
    if(msgLvl(MSG::INFO)) msg(MSG::INFO) << "Retrieved "<< tracks->size() <<" reconstructed tracks from StoreGate" << endmsg;
  }
    
  //TrackCollection::const_iterator trksItr  = tracks->begin();
  //TrackCollection::const_iterator trksItrE = tracks->end();

  xAOD::TrackParticleContainer::const_iterator trksItr  = tracks->begin();
  xAOD::TrackParticleContainer::const_iterator trksItrE = tracks->end();
  for (; trksItr != trksItrE; ++trksItr) {
    
    // Trk::Track* track = *trksItr;
    const xAOD::TrackParticle* track = *trksItr;

    //Convert the const Trk::Track to Trk::Track. ATTENTION: writing to the casted object will create undefined behaviour. 
    Trk::Track* trk_p = const_cast<Trk::Track*>(track->track());
   
    if (!trk_p)
      {
	msg(MSG::DEBUG)<<" IDAlignMon::TrackSelectorTool  ---NULL TRACK POINTER--- skipping track"<<endmsg;
	continue;
      }

        
    if(m_passAllTracks) {
      if(msgLvl(MSG::VERBOSE)) msg(MSG::VERBOSE) << "Track automatically passes TrackSelectionTool since passAllTracks=True" << endmsg;
      //selected_tracks->push_back(track);//allow all tracks into new collection, regardless of decision 
      return_tracks->push_back(trk_p);
    }
    else {
      if(msgLvl(MSG::VERBOSE)) msg(MSG::VERBOSE) << "Testing track using trackSelectorTool..." << endmsg;
      
      bool trackPassed = false;
      if(pVtx){
	if (msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) << "Using primary vertex in track selection" << endmsg;

	
	if (m_useIDTrackSelectionTool)
	  {
	    if (m_idtrackSelectionTool->accept(*track,pVtx))
	      trackPassed = true;
	  }
	else
	  trackPassed = m_trackSelectorTool->decision(*track,pVtx);
      }
      else {
	if (msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) << "Not using primary vertex in track selection" << endmsg;

	
	if (m_useIDTrackSelectionTool)
	  {
	    if (m_idtrackSelectionTool->accept(*track))
	    trackPassed = true;
	  }
	else
	  trackPassed = m_trackSelectorTool->decision(*track);
      }
      
      if(m_doEventPhaseCut){
	// cut on the TRT_Phase (ie: the Event Phase)
	float eventPhase=-99.0;
	
	ComTime* theComTime(0);
	StatusCode sc = evtStore()->retrieve(theComTime, m_commTimeName);
	if(sc.isFailure()){
	  if(msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) << "ComTime object not found with name TRT_Phase !!!" << endmsg;
	  trackPassed = false;
	}
	
	// get the event phase (one for the entire event)
	if(theComTime){
	  eventPhase = theComTime->getTime();
	}
	
	if(msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) << "Event phase is " << eventPhase << endmsg;
	
	// Cut on event phase
	if(eventPhase == -99.0 || eventPhase <= m_minEventPhase || eventPhase >= m_maxEventPhase) {
	  trackPassed = false;
	}
	
      }
      
      if(trackPassed) {
	//selected_tracks->push_back(track);//allow only tracks that pass decision into the new collection
	return_tracks->push_back(trk_p);
	if(msgLvl(MSG::VERBOSE)) msg(MSG::VERBOSE) << "Track passed trackSelectorTool" << endmsg;
      }
      else if(msgLvl(MSG::VERBOSE)) msg(MSG::VERBOSE) << "Track failed trackSelectorTool" << endmsg;
    }
  }

  


  return return_tracks;
}

//---------------------------------------------------------------------------------------

const DataVector<Trk::Track>* InDetAlignMon::TrackSelectionTool::selectTracks()
{

  //if this method the decision on which trackcollection
  //is made from the configuration of the TrackSlectionTool (in jobOptions)
  //returns a view to a new track collection object which contains the selected tracks

  const TrackCollection* tracks;
  auto selected_tracks = std::make_unique<ConstDataVector<DataVector<Trk::Track> > >(SG::VIEW_ELEMENTS);

  StatusCode sc = evtStore()->retrieve(tracks,m_trackColName);
  if (sc.isFailure()) {
    if(msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) << "No TrackCollection with name "<<m_trackColName<<" found in StoreGate" << endmsg;
    return selected_tracks.release()->asDataVector(); //return empty track collection (but not NULL)
  } else {
    if(msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) << "TrackCollection with name "<<m_trackColName<<" found in StoreGate" << endmsg;
    if(msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) << "Retrieved "<< tracks->size() <<" reconstructed tracks from StoreGate" << endmsg;
  }

  TrackCollection::const_iterator trksItr  = tracks->begin();
  TrackCollection::const_iterator trksItrE = tracks->end();
  for (; trksItr != trksItrE; ++trksItr) {
    
    const Trk::Track* track = *trksItr;

    if(m_passAllTracks) {
      selected_tracks->push_back(track);//allow all tracks into new collection, regardless of decision 
    }
    else {
      bool trackPassed = m_trackSelectorTool->decision(*track);
      if(trackPassed) selected_tracks->push_back(track);//allow only tracks that pass decision into the new collection    
    }
  }

  return selected_tracks.release()->asDataVector();
}

