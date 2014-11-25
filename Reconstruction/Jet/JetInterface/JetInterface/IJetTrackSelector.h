/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// IJetTrackSelector.h

#ifndef IJetTrackSelector_H
#define IJetTrackSelector_H

// David Adams
// March 2014

/// ITrackSelector is a dual-use tool interface for a tool that selects
/// a track particle.

#include "AsgTools/IAsgTool.h"
#include "xAODTracking/TrackParticle.h"

class IJetTrackSelector : virtual public asg::IAsgTool {
ASG_TOOL_INTERFACE(ITrackSelector)

public:

  /// Destructor.
  virtual ~IJetTrackSelector() { };

  /// Method to select.
  /// Returns true if jet is selected.
  virtual int keep(const xAOD::TrackParticle& trk) const =0;

};

#endif
