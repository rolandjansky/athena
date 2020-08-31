/*
   Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
 */

#ifndef TRACKSELECTIONBASE_H_
#define TRACKSELECTIONBASE_H_

#include "xAODTracking/TrackParticle.h"

namespace top {
  class TrackSelectionBase {
  public:
    TrackSelectionBase();
    virtual ~TrackSelectionBase();

    ///For the main analysis object selection
    virtual bool passSelection(const xAOD::TrackParticle&, const xAOD::Vertex&) = 0;
    ///Because everybody likes to know what object definitions they ran with
    virtual void print(std::ostream&) const = 0;
  };
}

std::ostream& operator << (std::ostream& os, const top::TrackSelectionBase& selection);

#endif
