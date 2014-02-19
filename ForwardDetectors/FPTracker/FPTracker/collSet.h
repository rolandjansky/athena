/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef FPTRACKER_COLLSET_H
#define FPTRACKER_COLLSET_H

#include "Collimator.h"
#include "FPTrackerConstants.h"

namespace FPTracker{
  class CollimatorData;
  Collimator::Container_t collSet(const CollimatorData&, Side);
}
#endif
