/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "ForwardTracker/ForwardTrackerConstants.h"
#include "ForwardTracker/TransversePoint.h"
#include "ForwardTracker/TransverseShift.h"

#include <cmath>

namespace ForwardTracker {
  
  TransversePoint TransverseShift(double z) { // account of beam splitting around 155 m
    
    if (std::fabs(z) > ForwardTrackerConstants::zOfBeamSeparation) 
      return TransversePoint(-ForwardTrackerConstants::xdis, ForwardTrackerConstants::ydis);
    
    return TransversePoint(0., 0.);
  }
}
