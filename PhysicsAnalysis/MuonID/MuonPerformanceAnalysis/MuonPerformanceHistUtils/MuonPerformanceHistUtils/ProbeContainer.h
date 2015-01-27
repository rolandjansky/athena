/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// ProbeContainer.h
#ifndef PROBECONTAINER_H
#define PROBECONTAINER_H
  
#include <stdint.h>
#include <vector>

#include "MuonPerformanceHistUtils/Probe.h"

/// The container is a simple typedef for now
typedef std::vector< Probe* > ProbeContainer;

#endif //PROBECONTAINER_H
