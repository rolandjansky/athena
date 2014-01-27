/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef FORWARDTRACKER_FORWARDTRACKERCONSTANTS_H
#define FORWARDTRACKER_FORWARDTRACKERCONSTANTS_H

#include <iosfwd>

namespace ForwardTracker {

  enum Bendplane {hBendplane = 0, vBendplane = 1};
  enum Side      {beam1      = 1, beam2      = 0};
  
  class ForwardTrackerConstants {
    
  public:
  
    static const double beamPipeRadius2;
    static const double aside[2];
    static const double xdis;         // half the beamline separation in the parallel region, COLD.
    static const double ydis;         // half the beamline separation in the parallel region, COLD.
    static const double zOfBeamSeparation;
  };
  
  std::ostream& operator<<(std::ostream&, const Side& side);
}

#endif
