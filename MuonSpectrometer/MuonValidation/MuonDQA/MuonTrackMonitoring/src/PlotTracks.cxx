/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "MuonTrackMonitoring/PlotTracks.h"

PlotTracks::PlotTracks(PlotBase* pParent, std::string sDir):PlotBase(pParent, sDir),
    m_plots_Impact(this, "/"),
    m_plots_TrkRecoInfo(this, "/"),
    m_plots_MSHit(this, "/")
{}

void PlotTracks::fill(const xAOD::Muon& muon){

  std::cout << muon.pt() << std::endl; 
  // tracksID = get_associated_tracks_ID(muon)
  // for trackID in tracksID:
  //     fill(trackID)
  //
  // tracksMS = get_associated_tracks_MS(muon)
  // for trackMS in tracksMS:
  //     fill(trackMS)
  //
  // tracksEX = get_associated_tracks_EX(muon)
  // for trackEX in tracksEX:
  //     fill(trackEX)

}

void PlotTracks::fill(const xAOD::TrackParticle& track){

    std::cout << track.pt() << std::endl; 
    // m_plots_Tracks.fill(track);

}

