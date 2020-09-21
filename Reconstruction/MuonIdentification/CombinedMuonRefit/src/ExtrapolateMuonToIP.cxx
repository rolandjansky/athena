/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// ExtrapolateMuonToIP.cxx
//   Implementation file for class ExtrapolateMuonToIP
///////////////////////////////////////////////////////////////////
// Niels van Eldik
///////////////////////////////////////////////////////////////////

#include "ExtrapolateMuonToIP.h"
#include "MuonRecToolInterfaces/IMuonTrackExtrapolationTool.h"

#include "MuonRecHelperTools/MuonEDMPrinterTool.h"
#include "TrkTrack/TrackCollection.h"

// Constructor with parameters:
ExtrapolateMuonToIP::ExtrapolateMuonToIP(const std::string &name, ISvcLocator *pSvcLocator) :
  AthAlgorithm(name,pSvcLocator)
{
  // Get parameter values from jobOptions file
  declareProperty("MuonTrackLocation",             m_muonTrackLocation = "MooreTracks");
  declareProperty("ExtrapolatedMuonTrackLocation", m_extrapolateMuonTrackLocation = "MooreExtrapolatedTracks" );
}

// Initialize method:
StatusCode ExtrapolateMuonToIP::initialize()
{
  if (!m_backExtrapolator.empty()) {
    ATH_CHECK(m_backExtrapolator.retrieve());
  }
  if (!m_printer.empty()) {
    ATH_CHECK(m_printer.retrieve());
  }

  ATH_MSG_INFO("ExtrapolateMuonToIP::initialize() -- done");
  return StatusCode::SUCCESS;
}

// Execute method:
StatusCode ExtrapolateMuonToIP::execute()
{
  // retrieve muon tracks
  const TrackCollection* muonTracks = 0;
  ATH_MSG_DEBUG("Retrieving " << m_muonTrackLocation);

  ATH_CHECK(evtStore()->retrieve(muonTracks, m_muonTrackLocation));
  ATH_MSG_DEBUG("Muon Tracks found at " << m_muonTrackLocation << " size " << muonTracks->size());

  // use tool to perform back extrapolation
  TrackCollection* extrapolateTracks = m_backExtrapolator->extrapolate(*muonTracks);
  if( !extrapolateTracks) {
    ATH_MSG_WARNING("BackExtrapolator returned zero pointer, creating empty collection ");
    extrapolateTracks = new TrackCollection();
  }


if (evtStore()->contains<TrackCollection>(m_extrapolateMuonTrackLocation))
     {
         const TrackCollection* extrapolatedTracks;
         ATH_CHECK(evtStore()->retrieve(extrapolatedTracks,
                                        m_extrapolateMuonTrackLocation));
         if (StatusCode::SUCCESS == evtStore()->remove(extrapolatedTracks))
         {
           ATH_MSG_VERBOSE( " removed pre-existing extrapolated track collection" );
         }
    }


  StatusCode sc = evtStore()->record(extrapolateTracks, m_extrapolateMuonTrackLocation);
  if (sc.isFailure()){
    ATH_MSG_WARNING("Could not record Track collection named " << m_extrapolateMuonTrackLocation);
    return StatusCode::SUCCESS;
  } else {
    ATH_MSG_DEBUG("Extrapolated muons tracks recoreded at " << m_extrapolateMuonTrackLocation << " size " << extrapolateTracks->size());
  }

  // now create track particles


  return StatusCode::SUCCESS;
}

// Finalize method:
StatusCode ExtrapolateMuonToIP::finalize()
{
  ATH_MSG_INFO("ExtrapolateMuonToIP::finalize()");
  return StatusCode::SUCCESS;
}

