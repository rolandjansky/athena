/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MCTruthBase_AtlasTrajectory_H
#define MCTruthBase_AtlasTrajectory_H

#include "G4Trajectory.hh"

/// @brief Class to store G4 trajectory information
///
/// NEEDS DOCUMENTATION
///
class AtlasTrajectory : public G4Trajectory
{

public:

  /// Constructor
  AtlasTrajectory(const G4Track* aTrack, int subDetVolLevel);

  /// Overriden from G4 in order to do vertex analysis
  void AppendStep(const G4Step* aStep);

  /// Visualization stuff
#if G4VERSION_NUMBER >= 1010
  void DrawTrajectory() const override { DrawTrajectory(0); }
#endif
  void DrawTrajectory(G4int) const;

private:

  using G4Trajectory::DrawTrajectory;

  /// The level in the G4 volume hierarchy at which can we find the sub-detector name
  int m_subDetVolLevel;

};

#endif
