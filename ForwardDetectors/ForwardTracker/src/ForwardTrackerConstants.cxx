/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "ForwardTracker/ForwardTrackerConstants.h"

#include <ostream>

namespace ForwardTracker {

  const double ForwardTrackerConstants::beamPipeRadius2   = 0.2*0.2;
  const double ForwardTrackerConstants::aside[2]          = {-1., 1.};
  const double ForwardTrackerConstants::xdis              = 0.097; // half the beamline separation in the parallel region (194mm/2 = 0.097mm)
  const double ForwardTrackerConstants::ydis              = 0.0;
  const double ForwardTrackerConstants::zOfBeamSeparation = 155.0;

  std::ostream& operator<<(std::ostream& os, const Side& side) { if (side == beam1) os << " beam1 "; else os << " beam2 "; return os; }
}
