/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef AtlasTrajectory_H
#define AtlasTrajectory_H

#include "G4Trajectory.hh"
#include "G4TrackingManager.hh"

#include "SimHelpers/SecondaryTracksHelper.h"

/// @brief Class to store G4 trajectory information
///
/// NEEDS DOCUMENTATION
///
class AtlasTrajectory : public G4Trajectory
{

public:

  /// Constructors
  AtlasTrajectory() {};
  AtlasTrajectory(const G4Track* aTrack):G4Trajectory(aTrack){};
  ~AtlasTrajectory() {};

  /// Overriden from G4 in order to do vertex analysis
  void AppendStep(const G4Step* aStep);

  /// Visualization stuff
#if G4VERSION_NUMBER >= 1010
  void DrawTrajectory() const override { DrawTrajectory(0); }
#endif
  void DrawTrajectory(G4int) const;

  void setTrackingManager(G4TrackingManager* tm){
    theTrackManager=tm;
    theSecondaryTrackHelper.SetTrackingManager(tm);
  };
  
private:

  const G4TrackingManager *theTrackManager;
  SecondaryTracksHelper theSecondaryTrackHelper;
  using G4Trajectory::DrawTrajectory;

};

#endif
