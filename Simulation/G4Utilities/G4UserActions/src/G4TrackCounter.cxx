/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "G4UserActions/G4TrackCounter.h"
#include "MCTruth/TrackHelper.h"


namespace G4UA
{

  //---------------------------------------------------------------------------
  // Merge results
  //---------------------------------------------------------------------------
  void G4TrackCounter::Report::merge(const G4TrackCounter::Report& rep)
  {
    nEvents += rep.nEvents;
    nTotalTracks += rep.nTotalTracks;
    nPrimaryTracks += rep.nPrimaryTracks;
    nSecondaryTracks += rep.nSecondaryTracks;
    n50MeVTracks += rep.n50MeVTracks;
  }

  //---------------------------------------------------------------------------
  // Increment event counter
  //---------------------------------------------------------------------------
  void G4TrackCounter::BeginOfEventAction(const G4Event* /*event*/)
  {
    m_report.nEvents++;
  }

  //---------------------------------------------------------------------------
  // Increment track counters
  //---------------------------------------------------------------------------
  void G4TrackCounter::PreUserTrackingAction(const G4Track* track)
  {
    m_report.nTotalTracks++;
    TrackHelper helper(track);

    // Primary tracks
    if(helper.IsPrimary() || helper.IsRegeneratedPrimary())
      m_report.nPrimaryTracks++;

    // Secondary tracks
    if(helper.IsRegisteredSecondary())
      m_report.nSecondaryTracks++;

    // 50 MeV tracks
    const double minE = 50.;
    if(track->GetKineticEnergy() > minE)
      m_report.n50MeVTracks++;
  }

} // namespace G4UA
