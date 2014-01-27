/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef FORWARDTRACKER_BEAMLINESETUP_H
#define FORWARDTRACKER_BEAMLINESETUP_H

#include "ForwardTrackerConstants.h"

#include <fstream>

namespace ForwardTracker {

  class Beamline;
  class ConfigData;
  
  Beamline BeamlineSetup(const ConfigData&, const Side&);
}

#endif
