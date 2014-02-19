/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "FPTracker/beamlineXPosition.h"
#include "FPTracker/FPTrackerConstants.h"
#include "FPTracker/TransversePoint.h"
#include <cmath>
namespace FPTracker
{
  TransversePoint beamlineXPosition(double z)
  {
    if (std::fabs(z)> FPTrackerConstants::zOfBeamSeparation)
      {
	TransversePoint tp(-FPTrackerConstants::xdis, FPTrackerConstants::ydis);
	return tp;
      }
    TransversePoint tp( 0., 0. );
    return tp;
  }
}
