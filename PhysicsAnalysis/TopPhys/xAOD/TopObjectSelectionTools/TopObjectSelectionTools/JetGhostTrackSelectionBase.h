/*
   Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
 */

#ifndef JETGHOSTTRACKSELECTIONBASE_H_
#define JETGHOSTTRACKSELECTIONBASE_H_

#include "xAODTracking/TrackParticle.h"

namespace top {
  class JetGhostTrackSelectionBase {
  public:
    JetGhostTrackSelectionBase();
    virtual ~JetGhostTrackSelectionBase();
    
    ///For the main analysis object selection
    virtual bool passSelection(const xAOD::TrackParticle&, const xAOD::Vertex&) const = 0;
    ///To see if a jet hase the requirements to retrieve the corresponding tracks
    virtual bool JetConsideredForGhostTrackSelection(double pt, double eta) const = 0;
    ///Because everybody likes to know what object definitions they ran with
    virtual void print(std::ostream&) const = 0;
    
  };

std::ostream& operator << (std::ostream& os, const top::JetGhostTrackSelectionBase& selection);

}
#endif
