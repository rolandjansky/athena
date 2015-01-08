/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "MuonAlignErrorTool/MuonAlignErrorExampleAlg.h"
#include "MuonAlignErrorTool/AlignmentDeviation.h"
#include "TrkTrack/TrackCollection.h"
#include <iostream>

using namespace MuonAlign;
using namespace std;

MuonAlignErrorExampleAlg::MuonAlignErrorExampleAlg (const std::string& name, ISvcLocator* pSvcLocator)
: AthAlgorithm(name, pSvcLocator),
  m_alignErrorTool("MuonAlign::AlignmentErrorTool")
{
  declareProperty("alignErrorTool", m_alignErrorTool);
}

StatusCode MuonAlignErrorExampleAlg::initialize () {
  return StatusCode::SUCCESS;
}

StatusCode MuonAlignErrorExampleAlg::execute () {
  StatusCode sc;
  const ::TrackCollection* tracks = 0;
  sc = sgSvc()->retrieve(tracks, "MuonSpectrometerTracks"); // for example, should be a job option
  if(sc.isFailure()) return sc;

  // LOOP ON MUON TRACKS //
  for (::TrackCollection::const_iterator it=tracks->begin(), end=tracks->end(); it!=end; it++) {
    
    const Trk::Track* track = *it;
    muonTrack(track);

  }

  return StatusCode::SUCCESS;
}

void MuonAlignErrorExampleAlg::muonTrack (const Trk::Track* track) const {
  // Compute and verbose the alignment deviations for a given track

  ATH_MSG_DEBUG("Processing track with momentum " << track->trackParameters()->front()->momentum().mag()/1000. << " GeV and transverse momentum " << track->trackParameters()->front()->momentum().perp()/1000 << " GeV");

  std::vector<Trk::AlignmentDeviation*> deviations;
  m_alignErrorTool->makeAlignmentDeviations(*track, deviations);

  for (vector<Trk::AlignmentDeviation*>::iterator it(deviations.begin()), end(deviations.end()); it!=end; ++it)
    delete (*it);
}

StatusCode MuonAlignErrorExampleAlg::finalize () {
  return StatusCode::SUCCESS;
}

