/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#include "MuPatTrackBuilder.h"
#include "MuonRecHelperTools/IMuonEDMHelperSvc.h"

#include "StoreGate/DataHandle.h"
#include "TrkSegment/SegmentCollection.h"
#include "TrkTrack/Track.h"
#include "TrkTrack/TrackCollection.h"
#include "MuonSegment/MuonSegment.h"

#include "TrkTrack/TrackStateOnSurface.h"
#include "Particle/TrackParticleContainer.h"
#include <vector>

using namespace Muon;

StatusCode MuPatTrackBuilder::initialize()
{
  if (m_trackMaker.retrieve().isFailure()){
    msg(MSG::FATAL) <<"Could not get " << m_trackMaker <<endmsg; 
    return StatusCode::FAILURE;
  }
  if (m_edmHelperSvc.retrieve().isFailure()){
    msg(MSG::FATAL) <<"Could not get " << m_edmHelperSvc <<endmsg; 
    return StatusCode::FAILURE;
  }
  if( msgLvl(MSG::DEBUG) ) msg(MSG::DEBUG) << "Retrieved " << m_trackMaker << endmsg;
  
 ATH_CHECK( m_segmentKey.initialize() );
 ATH_CHECK( m_spectroTrackKey.initialize() );

  return StatusCode::SUCCESS; 
}

StatusCode MuPatTrackBuilder::execute()
{
  typedef std::vector<const Muon::MuonSegment*> MuonSegmentCollection;

  SG::ReadHandle<Trk::SegmentCollection> segmentColl (m_segmentKey);
  if (!segmentColl.isValid() ) {
    msg(MSG::WARNING) << "Could not find MuonSegmentCollection at " << segmentColl.name() <<endmsg;
    return StatusCode::RECOVERABLE;
  }
    
  if( !segmentColl.cptr() ) {
    msg(MSG::WARNING) << "Obtained zero pointer for MuonSegmentCollection at " << segmentColl.name() <<endmsg;
    return StatusCode::RECOVERABLE;
  }
      
  if( msgLvl(MSG::DEBUG) ) msg(MSG::DEBUG) << "Retrieved MuonSegmentCollection "  << segmentColl->size() << endmsg;

  MuonSegmentCollection msc;
  msc.reserve(segmentColl->size());
  for (unsigned int i=0;i<segmentColl->size();++i){
    if (!segmentColl->at(i)) continue;
    const Muon::MuonSegment * ms = dynamic_cast<const Muon::MuonSegment*>(segmentColl->at(i));
    if (ms) msc.push_back( ms );
  }

  if (msc.size() != segmentColl->size()){
    msg(MSG::WARNING) << "Input segment collection (size " << segmentColl->size() << ") and translated MuonSegment collection (size "
                      << msc.size() << ") are not the same size." << endmsg;
  }

  TrackCollection * newtracks = m_trackMaker->find(msc);
  if (!newtracks) newtracks = new TrackCollection();

  SG::WriteHandle<TrackCollection> spectroTracks(m_spectroTrackKey); 	  
  if (spectroTracks.record(std::unique_ptr<TrackCollection>(newtracks)).isFailure()){    
      ATH_MSG_WARNING( "New Track Container " << spectroTracks.name() << " could not be recorded in StoreGate !");
      return StatusCode::RECOVERABLE;
  }
  ATH_MSG_DEBUG ("TrackCollection '" << m_spectroTrackKey.key() << "' recorded in storegate, ntracks: " << newtracks->size());

  return StatusCode::SUCCESS;
} // execute

