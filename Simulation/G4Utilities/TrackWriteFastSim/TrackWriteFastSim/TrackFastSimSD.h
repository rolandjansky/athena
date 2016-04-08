/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRACKWRITEFASTSIM_TRACKFASTSIMSD_H
#define TRACKWRITEFASTSIM_TRACKFASTSIMSD_H

// Base class header
#include "G4VSensitiveDetector.hh"

// Athena headers
#include "TrackRecord/TrackRecordCollection.h"
#include "StoreGate/WriteHandle.h"

// STL headers
#include <string>

// G4 needed classes
class G4Step;
class G4TouchableHistory;
class G4Track;

class TrackFastSimSD : public G4VSensitiveDetector
{
public:
  TrackFastSimSD(const std::string& name, const std::string& outputCollectionName, const int SD_type=0);
  ~TrackFastSimSD() {}

  // Initialize from G4
  void Initialize(G4HCofThisEvent *) override final;

  G4bool ProcessHits(G4Step*, G4TouchableHistory*) override final;
  void   WriteTrack(const G4Track*, const bool, const bool);

  /** Templated method to stuff a single hit into the sensitive detector class.  This
   could get rather tricky, but the idea is to allow fast simulations to use the very
   same SD classes as the standard simulation. */
  template <class... Args> void AddHit(Args&&... args){ m_trackRecordCollection->Emplace( args... ); }

private:
  // The hits collection
  SG::WriteHandle<TrackRecordCollection> m_trackRecordCollection;

  int m_SD_type; //TrackRecorderSD=1, CosmicTRSD=2
};

#endif // TRACKWRITEFASTSIM_TRACKFASTSIMSD_H
