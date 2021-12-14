/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#include "TrkTrack/TrackCollection.h"
#include "TrkAlignGenAlgs/AlignTrackCollSplitter.h"
#include "TrkAlignEvent/AlignTrack.h"


namespace Trk {

//___________________________________________________________________________
AlignTrackCollSplitter::AlignTrackCollSplitter(const std::string& name, ISvcLocator* pSvcLocator)
  : AthAlgorithm(name, pSvcLocator)
  , m_nevents(0)
  , m_ntracks(0)
  , m_trackTypeCounter(AlignTrack::NTrackTypes+1,0)
  , m_nRetrievalErrors(0)
{
  declareProperty("InputTrkCol",        m_inputCol           = "AlignTracks");
  declareProperty("OutputColPrefix",    m_outColPrefix       = "AlignTracks");
  declareProperty("MaxRetrievalErrors", m_maxRetrievalErrors = -1);
}

//___________________________________________________________________________
AlignTrackCollSplitter::~AlignTrackCollSplitter() 
{
  ATH_MSG_DEBUG("AlignTrackCollSplitter destructor");
}

//___________________________________________________________________________
StatusCode AlignTrackCollSplitter::initialize() 
{
  ATH_MSG_DEBUG("AlignTrackCollSplitter::initialize()");

  // set the track collection names
  ATH_MSG_INFO("Splitting "<<m_inputCol<<" into:");
  for(int icol=0; icol<AlignTrack::NTrackTypes+1; ++icol) {
    m_outColNames.push_back( m_outColPrefix + dumpAlignTrackType((AlignTrack::AlignTrackType)icol) );
    ATH_MSG_INFO(" - "<<m_outColNames[icol]);
  }

  return StatusCode::SUCCESS;
}

//___________________________________________________________________________
StatusCode AlignTrackCollSplitter::execute() 
{
  ATH_MSG_DEBUG("AlignTrackCollSplitter::execute()");

  // increment number of events
  m_nevents++;

  // loop over tracks
  const TrackCollection * originalTracks;

  // check if m_inputCol is a TrackCollection and exists for this event
  // perhaps this can be done in a much nicer way on the python side ala PyUtils/checkFile.py
  // do it here for now
  if( !evtStore()->contains<TrackCollection>(m_inputCol) ||
      evtStore()->retrieve(originalTracks,m_inputCol).isFailure() ) {
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

  // if we're here it means that the track collection m_inputCol exists in the file
  // (it can be empty) so we change the error cutoff to process the whole file
  if(m_maxRetrievalErrors>=0)
    m_maxRetrievalErrors=-1;

  if(originalTracks->empty())
    return StatusCode::SUCCESS;

  // increment number of processed tracks
  m_ntracks += originalTracks->size();

  // vector to keep all different AlignTrack collections
  // +1 is for undefined AlignTrack type
  std::vector<TrackCollection *> alignTracks;
  alignTracks.reserve(AlignTrack::NTrackTypes+1);

for(int i=0;i<AlignTrack::NTrackTypes+1;i++)
    alignTracks.push_back(new TrackCollection);

  // loop over AlignTracks and fill the corresponding track collections
  int itrk = 0;
  DataVector<Track>::const_iterator it     = originalTracks->begin();
  DataVector<Track>::const_iterator it_end = originalTracks->end();
  for ( ;it != it_end ; ++it, ++itrk) {

    ATH_MSG_DEBUG("Processing AlignTrack "<<itrk);

    // each track in the collection HAS to be an AlignTrack
    const AlignTrack * alignTrack = dynamic_cast<const AlignTrack*>(*it);
    if(!alignTrack) {
      msg(MSG::FATAL)<<"Track is not an AlignTrack. The code only works with TrackCollection"
                     <<" filled with AlignTracks."<<endmsg;
      return StatusCode::FAILURE;
    }

    int type = alignTrack->type();
    ATH_MSG_DEBUG("type of AlignTrack: "<<alignTrack->type());

    //previously this check was always returning true (type>=0 || type<AlignTrack::NTrackTypes), meaning that in principle you could try
    //to access an invalid vector element if type was negative. Assume that what we have below is what was meant, as last entry in vector
    // is for undefined tracks? (Nick Styles, 04/02/15)
    int colIndex = (type>=0 && type<AlignTrack::NTrackTypes) ? type : AlignTrack::NTrackTypes;

    ATH_MSG_DEBUG("collection index is "<<colIndex);
    alignTracks[colIndex]->push_back(new AlignTrack(*alignTrack));
    ATH_MSG_DEBUG("pushback ok");

    ++m_trackTypeCounter[colIndex];
  }

  // record the collections in the StoreGate that takes care of deletion
  // only record collection to storegate if it's not empty
  for(int icol=0;icol<AlignTrack::NTrackTypes+1;++icol) {
    ATH_MSG_DEBUG(icol<<": recording "<<m_outColNames[icol]<<"  size "<<alignTracks[icol]->size());
    if(!alignTracks[icol]->empty()) {
      if(evtStore()->record(alignTracks[icol],m_outColNames[icol],false).isFailure()) {
        ATH_MSG_ERROR("Unable to record "<<m_outColNames[icol]<<" in the StoreGate");
        delete alignTracks[icol];
      }
      ATH_MSG_DEBUG("Recorded \'"<<m_outColNames[icol]<<"\' with size "<<alignTracks[icol]->size()<<" to StoreGate");
    }
    else {
      ATH_MSG_DEBUG("not recording "<<m_outColNames[icol]);
      delete alignTracks[icol];
    }
  }

  ATH_MSG_DEBUG("returning SUCCESS");
  return StatusCode::SUCCESS;
}

//___________________________________________________________________________
StatusCode AlignTrackCollSplitter::stop()
{
  ATH_MSG_DEBUG("AlignTrackCollSplitter::stop()");

  ATH_MSG_INFO("Processed "<<m_nevents<<" with "<<m_ntracks<<" AlignTracks");
  ATH_MSG_INFO("Track collections recorded in StoreGate:");
  ATH_MSG_INFO("----------------------------------------------");
  for(int icol=0;icol<AlignTrack::NTrackTypes+1;++icol)
    ATH_MSG_INFO(m_outColPrefix<<AlignTrack::AlignTrackType(icol)<<": "<<m_trackTypeCounter[icol]);
  ATH_MSG_INFO("----------------------------------------------");

  return StatusCode::SUCCESS;
}

//___________________________________________________________________________
StatusCode AlignTrackCollSplitter::finalize()
{
  ATH_MSG_DEBUG("AlignTrackCollSplitter::finalize()");
  return StatusCode::SUCCESS;
}

}
