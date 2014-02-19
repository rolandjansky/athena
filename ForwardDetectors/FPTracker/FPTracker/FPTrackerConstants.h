/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef FPTRACKER_FPTRACKERCONSTANTS_H
#define FPTRACKER_FPTRACKERCONSTANTS_H

#include <iosfwd>

namespace FPTracker{
  enum Bendplane {hBendplane=0, vBendplane=1};
  enum Side {beam1 = 1, beam2 = 0};
  class FPTrackerConstants{
  public:
    static const double beamPipeRadius2;
    static const double aside[2];
    static const double qkConst;
    static const double xdis;      // half the beamline sepn. in the parallel region, COLD.
    static const double ydis;      // half the beamline sepn. in the parallel region, COLD.
    static const double zOfBeamSeparation;
};
  
  std::ostream& operator<<(std::ostream&, const Side& side);
}
#endif
