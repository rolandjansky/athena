/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef FORWARDTRACKER_MAGNETSET_H
#define FORWARDTRACKER_MAGNETSET_H

#include "ForwardTrackerConstants.h"
#include "Magnet.h"

namespace ForwardTracker {
  
  class ConfigData;
  
  Magnet::Container_t MagnetSet(const ConfigData&, const Side&);
}

#endif
