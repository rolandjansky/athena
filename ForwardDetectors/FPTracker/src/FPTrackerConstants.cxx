/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "FPTracker/FPTrackerConstants.h"
#include <ostream>

namespace FPTracker{
  const double FPTrackerConstants::beamPipeRadius2   = 0.2*0.2;
  const double FPTrackerConstants::qkConst           = 0.2997925;
  const double FPTrackerConstants::aside[2]          = {-1., 1.};
  const double FPTrackerConstants::xdis              = 0.097;// half the beamline sepn. in the parallel region, COLD
  const double FPTrackerConstants::ydis              = 0.0;  // half the beamline sepn. in the parallel region, COLD
  const double FPTrackerConstants::zOfBeamSeparation = 155.0;

  std::ostream& operator<<(std::ostream& os, const Side& side){
    std::string label = side == beam1 ? " beam1 ":" beam2 ";
    os << label;
    return os;
  }
}
