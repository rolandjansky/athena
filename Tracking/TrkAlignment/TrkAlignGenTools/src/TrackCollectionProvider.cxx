/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TrkAlignGenTools/TrackCollectionProvider.h"

namespace Trk {
  
  //________________________________________________________________________
  TrackCollectionProvider::TrackCollectionProvider(const std::string& type, 
						   const std::string& name,
						   const IInterface* parent)
    
    : AthAlgTool(type,name,parent)
    , m_nRetrievalErrors(0)
  {

    declareInterface<ITrackCollectionProvider>(this);

    declareProperty("InputTrkCol",        m_inputCol="MooreTracks");
    
    declareProperty("MaxRetrievalErrors", m_maxRetrievalErrors = 10);

  }

  //________________________________________________________________________
  TrackCollectionProvider::~TrackCollectionProvider()
  {

  }

  //________________________________________________________________________
  StatusCode TrackCollectionProvider::initialize()
  {
    return StatusCode::SUCCESS;
  }

  //________________________________________________________________________
  StatusCode TrackCollectionProvider::finalize()
  {
    return StatusCode::SUCCESS;
  }

  
  //________________________________________________________________________
  StatusCode TrackCollectionProvider::trackCollection(const TrackCollection*& originalTracks)
  {
    ATH_MSG_DEBUG("in TrackCollectionProvider::trackCollection");

    originalTracks=nullptr;
    
    // check if m_inputCol is a TrackCollection and exists for this event
    // perhaps this can be done in a much nicer way on the python side ala PyUtils/checkFile.py
    // do it here for now
    if( !evtStore()->contains<TrackCollection>(m_inputCol) ||
	evtStore()->retrieve(originalTracks,m_inputCol).isFailure() ) {
      
      originalTracks=nullptr;
      
      ATH_MSG_DEBUG("input track collection \'"<<m_inputCol<<"\' not found for this event");
      
      if(m_maxRetrievalErrors>=0) {
	++m_nRetrievalErrors;
	if(m_nRetrievalErrors>=m_maxRetrievalErrors) {
	  msg(MSG::FATAL)<<"Input track collection \'"<<m_inputCol<<"\' not found in first "
			 <<m_nRetrievalErrors<<" events. Aborting."<<endmsg;
	  msg(MSG::FATAL)<<"Maximum allowed number of events without the track collection "
			 <<"can be changed using the \'MaxRetrievalErrors\' option (-1 for infinite)."
			 <<endmsg;
	  return StatusCode::FAILURE;
	}
      }
      return StatusCode::SUCCESS;
    }
    ATH_MSG_DEBUG("found "<<originalTracks->size()<<" tracks");

    // if we're here it means that the track collection m_inputCol exists in the file
    // (it can be empty) so we change the error cutoff to process the whole file
    if(m_maxRetrievalErrors>=0)
      m_maxRetrievalErrors=-1;
    
    return StatusCode::SUCCESS;
  }

} // end namespace
