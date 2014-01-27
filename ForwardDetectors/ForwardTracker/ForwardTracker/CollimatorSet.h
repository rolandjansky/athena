/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef FORWARDTRACKER_COLLIMATORSET_H
#define FORWARDTRACKER_COLLIMATORSET_H

#include "Collimator.h"

namespace ForwardTracker {

  class ConfigData;
  
  Collimator::Container_t CollimatorSet(const ConfigData&, const Side& side);
}

#endif
