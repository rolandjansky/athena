/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef DQM_ALGORITHMS_MDTOVERVIEW_STATION_H
#define DQM_ALGORITHMS_MDTOVERVIEW_STATION_H

#include "dqm_core/Algorithm.h"
#include "dqm_algorithms/MDTOverview.h"

namespace dqm_algorithms {

  struct MDTOverview_Station : public MDTOverview 
  {
    MDTOverview_Station():  MDTOverview( "MDTOverview_Station" ) {};
  };
  
} //namespace dqm_algorithms

#endif // DQM_ALGORITHMS_MDTOVERVIEW_STATION_H
