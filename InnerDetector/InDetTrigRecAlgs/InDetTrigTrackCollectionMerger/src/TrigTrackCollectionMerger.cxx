/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
//   Implementation file for class Trk::TrackCollectionMerger
///////////////////////////////////////////////////////////////////
// (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////
// 20.06.2008 Trigger implementation Iwona Grabowska-Bold
///////////////////////////////////////////////////////////////////

#include "InDetTrigTrackCollectionMerger/TrigTrackCollectionMerger.h"
#include "TrkTrack/Track.h"


#include "TrkToolInterfaces/ITrackSummaryTool.h"
#include "TrkToolInterfaces/IPRD_AssociationTool.h"

///////////////////////////////////////////////////////////////////
// Constructor
///////////////////////////////////////////////////////////////////

Trk::TrigTrackCollectionMerger::TrigTrackCollectionMerger(const std::string& name, ISvcLocator* pSvcLocator  ) :
  HLT::FexAlgo(name, pSvcLocator )
{
  declareProperty("AssoTool",                       m_assoTool               );
  declareProperty("SummaryTool" ,                   m_trkSummaryTool         );
}

///////////////////////////////////////////////////////////////////
// Initialisation
///////////////////////////////////////////////////////////////////

HLT::ErrorCode Trk::TrigTrackCollectionMerger::hltInitialize()
{
  msg() << MSG::INFO << "Initializing TrigTrackCollectionMerger" << endreq;

  if ( m_assoTool.retrieve().isFailure() ) {
    msg() << MSG::FATAL << "Failed to retrieve tool " << m_assoTool << endreq;
    return HLT::ErrorCode(HLT::Action::ABORT_JOB, HLT::Reason::BAD_JOB_SETUP);
  } else
    msg() << MSG::INFO << "Retrieved tool " << m_assoTool << endreq;
  
  if (m_trkSummaryTool.retrieve().isFailure()) {
    msg() << MSG::FATAL << "Failed to retrieve tool " << m_trkSummaryTool << endreq;
    return HLT::ErrorCode(HLT::Action::ABORT_JOB, HLT::Reason::BAD_JOB_SETUP);
  } else 
    msg() << MSG::INFO << "Retrieved tool " << m_trkSummaryTool << endreq;

  return HLT::OK;
}


///////////////////////////////////////////////////////////////////
// Execute
///////////////////////////////////////////////////////////////////
HLT::ErrorCode Trk::TrigTrackCollectionMerger::hltExecute(const HLT::TriggerElement*, HLT::TriggerElement* outputTE){

  // clean up association tool
  m_assoTool->reset();

  int outputLevel = msgLvl();
  
  
  const DataVector<Trk::Track>* trackCol;

  //get forward tracks with TRT extension
  if ( HLT::OK != getFeature(outputTE, trackCol, "ExtProcTracks") ) {
    msg() << MSG::ERROR << " Input track collection could not be found " << endreq;
    return HLT::NAV_ERROR;
  }
  
  // out output track collection
  TrackCollection* outputCol = new TrackCollection;

  if(!trackCol){
    if(outputLevel <= MSG::DEBUG)
      msg() << MSG::DEBUG << "REGTEST: Input forward-track collection with TRT exetension was not attached. Trying collection without TRT extension." << endreq;
    //return HLT::OK; 

    //get forward tracks without TRT extension
    if ( HLT::OK != getFeature(outputTE, trackCol, "AmbigSolv") ) {
      msg() << MSG::ERROR << " Input track collection could not be found " << endreq;

      delete outputCol;
      return HLT::NAV_ERROR;
    }

    if(!trackCol){
      if(outputLevel <= MSG::DEBUG)
	msg() << MSG::DEBUG << "REGTEST: Input forward-track collection without TRT exetension was not attached. Trying another collection." << endreq;
      //return HLT::OK;
    }
    else{
      if(outputLevel <= MSG::DEBUG)
	msg() << MSG::DEBUG << "REGTEST: Input forward-track collection without TRT extension has size " << trackCol->size() << endreq;
      if ( trackCol->size() == 0 ) {
	if(outputLevel <= MSG::DEBUG)
	  msg() << MSG::DEBUG << "REGTEST: Input forward-track collection without TRT extension has 0 size. Trying another collection." << endreq;
	//return HLT::OK;
      }   
      else{
	if( HLT::OK != mergeTrack(trackCol,outputCol) ){
	  msg() << MSG::ERROR << "Failed to merge tracks! " << endreq;
	  
	  delete outputCol;
	  return HLT::NAV_ERROR;
	}
      }
    }
  } 
  else {
    if(outputLevel <= MSG::DEBUG)
      msg() << MSG::DEBUG << "REGTEST: Input forward-track collection with TRT extension has size " << trackCol->size() << endreq;
    if ( trackCol->size() == 0 ) {
      if(outputLevel <= MSG::DEBUG)
	msg() << MSG::DEBUG << "REGTEST: Input forward-track collection with TRT extension has 0 size. Trying another collection." << endreq;
      //return HLT::OK; 
    }
    else{
      if( HLT::OK != mergeTrack(trackCol,outputCol) ){
	msg() << MSG::ERROR << "Failed to merge tracks! " << endreq;
	
	delete outputCol;
	return HLT::NAV_ERROR;
      }
    }
  }
  
  //get back tracks
  if ( HLT::OK != getFeature(outputTE, trackCol, "TRTSeededAmbigSolv") ) {
    msg() << MSG::ERROR << " Input track collection could not be found " << endreq;
    
    delete outputCol;
    return HLT::NAV_ERROR;
  }
  
  if(!trackCol){
    if(outputLevel <= MSG::DEBUG)
      msg() << MSG::DEBUG << " Input back-track collection was not attached. Trying another collection." << endreq;
    //return HLT::OK; 
  } 
  else {
    if(outputLevel <= MSG::DEBUG)
      msg() << MSG::DEBUG << " Input back-track collection has size " << trackCol->size() << endreq;
    if ( trackCol->size() == 0 ) {
      if(outputLevel <= MSG::DEBUG)
	msg() << MSG::DEBUG << " Input back-track collection has 0 size. Trying another collection." << endreq;
      //return HLT::OK; 
    }
    else{
      if( HLT::OK != mergeTrack(trackCol,outputCol) ){
	msg() << MSG::ERROR << "Failed to merge tracks! " << endreq;
	
	delete outputCol;
	return HLT::NAV_ERROR;
      }
    }
  }

  //get TRT standalone tracks
  if ( HLT::OK != getFeature(outputTE, trackCol, "TRTStandaloneTracks") ) {
    msg() << MSG::ERROR << " Input track collection could not be found " << endreq;
    
    delete outputCol;
    return HLT::NAV_ERROR;
  }
  
  if(!trackCol){
    if(outputLevel <= MSG::DEBUG)
      msg() << MSG::DEBUG << " Input TRT-standalone track collection was not attached." << endreq;
    //return HLT::OK; 
  } 
  else {
    if(outputLevel <= MSG::DEBUG)
      msg() << MSG::DEBUG << " Input TRT-standalone track collection has size " << trackCol->size() << endreq;
    if ( trackCol->size() == 0 ) {
      if(outputLevel <= MSG::DEBUG)
	msg() << MSG::DEBUG << " Input TRT-standalone track collection has 0 size." << endreq;
      //return HLT::OK; 
    }
    else{
      if( HLT::OK != mergeTrack(trackCol,outputCol) ){
	msg() << MSG::ERROR << "Failed to merge tracks! " << endreq;
	
	delete outputCol;
	return HLT::NAV_ERROR;
      }
    }
  }


  if(outputCol->size()==0) {
    if(outputLevel <= MSG::DEBUG)
      msg() << MSG::DEBUG << " Output-container is empty - not storing it." << endreq;
   
    delete outputCol;
    return HLT::OK;
  }

  //  Attach all tracks to the trigger element.
  if ( HLT::OK !=  attachFeature(outputTE, outputCol, "CombinedTracks") ) {
    msg() << MSG::ERROR << "Could not attach feature to the TE" << endreq;
    
    delete outputCol;
    return HLT::NAV_ERROR;
  }

  if(outputLevel <= MSG::DEBUG){
    msg() << MSG::DEBUG << "Container recorded in StoreGate." << endreq;
    msg() << MSG::DEBUG << "REGTEST: Output container size :" << outputCol->size() << endreq;
  } 
  
  if(outputLevel <= MSG::DEBUG) msg() << MSG::DEBUG << "Update summaries" << endreq;  
  // now loop over all track and update summaries with new shared hit counts
  TrackCollection::const_iterator rf,rfe=outputCol->end();
  for(rf=outputCol->begin();rf!=rfe; ++rf) m_trkSummaryTool->updateTrack(**rf);
  
  if(outputLevel <= MSG::DEBUG) msg() << MSG::DEBUG << "Done !" << endreq;  
  
  return HLT::OK;
}

///////////////////////////////////////////////////////////////////
// Finalize
///////////////////////////////////////////////////////////////////
HLT::ErrorCode Trk::TrigTrackCollectionMerger::hltFinalize()
{
  return HLT::OK;
}

///////////////////////////////////////////////////////////////////
// Dumps relevant information into the MsgStream
///////////////////////////////////////////////////////////////////
MsgStream&  Trk::TrigTrackCollectionMerger::dump( MsgStream& out ) const
{
  //MsgStream msg()(msgSvc(), name());
  out<<std::endl;
  if((msg().level()-MSG::DEBUG)<=0)  return dumpevent(out);
  return dumptools(out);
}

///////////////////////////////////////////////////////////////////
// Dumps conditions information into the MsgStream
///////////////////////////////////////////////////////////////////
MsgStream& Trk::TrigTrackCollectionMerger::dumptools( MsgStream& out ) const
{
  return out;
}

///////////////////////////////////////////////////////////////////
// Dumps event information into the MsgStream
///////////////////////////////////////////////////////////////////
MsgStream& Trk::TrigTrackCollectionMerger::dumpevent( MsgStream& out ) const
{
  return out;
}

///////////////////////////////////////////////////////////////////
// Dumps relevant information into the ostream
///////////////////////////////////////////////////////////////////
std::ostream& Trk::TrigTrackCollectionMerger::dump( std::ostream& out ) const
{
  return out;
}

///////////////////////////////////////////////////////////////////
// Overload of << operator MsgStream
///////////////////////////////////////////////////////////////////
MsgStream& Trk::operator    << 
  (MsgStream& sl,const Trk::TrigTrackCollectionMerger& se)
{ 
  return se.dump(sl); 
}

///////////////////////////////////////////////////////////////////
// Overload of << operator std::ostream
///////////////////////////////////////////////////////////////////
std::ostream& Trk::operator << 
  (std::ostream& sl,const Trk::TrigTrackCollectionMerger& se)
{
  return se.dump(sl); 
}   


///////////////////////////////////////////////////////////////////
// Merge track collections and remove duplicates
///////////////////////////////////////////////////////////////////
HLT::ErrorCode Trk::TrigTrackCollectionMerger::mergeTrack(const TrackCollection* trackCol, TrackCollection* outputCol)
{

  int outputLevel = msgLvl();

  // loop over forward track, accept them and add them imto association tool
  if(trackCol && trackCol->size()) {
    if(outputLevel <= MSG::DEBUG) msg() << MSG::DEBUG << "Size of track collection " << trackCol->size() << endreq;

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
  }

  return HLT::OK;
}

