/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#include "TrkAmbiguitySolver/TrkAmbiguitySolver.h"

Trk::TrkAmbiguitySolver::TrkAmbiguitySolver(const std::string& name, ISvcLocator* pSvcLocator) :
  AthAlgorithm(name, pSvcLocator)
{  
  declareProperty("TrackInput", m_tracksLocation={""});
}

//-----------------------------------------------------------------------
StatusCode
Trk::TrkAmbiguitySolver::initialize()
{
  ATH_MSG_INFO("TrkAmbiguitySolver::initialize(). ");

  if (not m_resolveTracks) {
    ATH_MSG_INFO("ATTENTION: Resolving tracks turned off! ");
    m_ambiTool.disable();
  } else {
    // Get Tools sevices
    if (m_ambiTool.retrieve().isFailure()) {
      ATH_MSG_FATAL("Failed to retrieve tool " << m_ambiTool);
      return StatusCode::FAILURE;
    } else {
      ATH_MSG_INFO("Retrieved tool " << m_ambiTool);
    }
  }

  ATH_CHECK(m_resolvedTracksLocation.initialize());
  ATH_CHECK(m_tracksLocation.initialize());

  return StatusCode::SUCCESS;
}

//-------------------------------------------------------------------------
StatusCode
Trk::TrkAmbiguitySolver::execute()
{
  ATH_MSG_VERBOSE("TrkAmbiguitySolver::execute()");
  ConstDataVector<TrackCollection> oldTracks(SG::VIEW_ELEMENTS);
  StatusCode loadedTracks = getTracks(oldTracks);

  if (loadedTracks.isFailure()) {
    ATH_MSG_ERROR("TrkAmbiguitySolver could not find any tracks. Aborting.");
    return StatusCode::SUCCESS;
  }
	
  // Now pass tracks to AmbiTool
  std::unique_ptr<TrackCollection> tracks;
  resolveTracks(tracks, oldTracks);

  SG::WriteHandle<TrackCollection> writeh(m_resolvedTracksLocation);
  ATH_CHECK(writeh.record(std::move(tracks)));
  ATH_MSG_VERBOSE("Saved "<<writeh->size()<<" tracks");

  return StatusCode::SUCCESS;
}

//---------------------------------------------------------------------------

StatusCode
Trk::TrkAmbiguitySolver::finalize()
{
  m_ambiTool->statistics();

  ATH_MSG_INFO("Finalizing with "<< m_trackInCount << " tracks input, and "<< m_trackOutCount<< " output");

  return StatusCode::SUCCESS;
}

//-------------------------------------------------------------------------------

StatusCode Trk::TrkAmbiguitySolver::getTracks(ConstDataVector<TrackCollection>& oldTracks)
{
  auto handles = m_tracksLocation.makeHandles();
  size_t totalsize = 0;
  for (SG::ReadHandle<TrackCollection>& tmpTracks : handles) {
    if (not tmpTracks.isValid()) {
      ATH_MSG_WARNING("Could not retrieve tracks from "<< tmpTracks.key());
    }
    totalsize += tmpTracks->size();
  }
  oldTracks.reserve(totalsize);
  for (SG::ReadHandle<TrackCollection>& tmpTracks : handles) {
    ATH_MSG_VERBOSE("Successfully retrieved "<<tmpTracks->size()
                    <<" tracks from "<< tmpTracks.key());
    copy(tmpTracks->begin(), tmpTracks->end(), std::back_inserter(oldTracks));
  }
  return StatusCode::SUCCESS;
}

//-------------------------------------------------------------------------------

void Trk::TrkAmbiguitySolver::resolveTracks(std::unique_ptr<TrackCollection>& tracks, ConstDataVector<TrackCollection>& oldTracks)
{ 
  if (m_resolveTracks) {
    // okay, and let's call the ambiguity processor, just for a laugh.
    ATH_MSG_VERBOSE("TrkAmbiguitySolver::resolveTracks() resolving "
                    << oldTracks.size()<<"  tracks");
    m_trackInCount += oldTracks.size();
    tracks = std::unique_ptr<TrackCollection>(m_ambiTool->process(oldTracks.asDataVector()));
    m_trackOutCount += tracks->size();
  } else {
    // copy tracks. NOT a shallow copy as it was before, since
    // this causes mem leaks when reading back in again
    // see bug#9886 for details
    tracks = std::make_unique<TrackCollection>();
    tracks->reserve(oldTracks.size());
    for (const Track* oldTrack : oldTracks) {
      tracks->push_back(new Track(*oldTrack));
    }
  }
  return;
}
