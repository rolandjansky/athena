/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/*file MDT_OccupancyHoleFinder imp author Justin Griffiths 
*/

#ifndef DQM_ALGORITHMS_MDT_OCCUPANCYHOLEFINDER_H
#define DQM_ALGORITHMS_MDT_OCCUPANCYHOLEFINDER_H

#include "dqm_core/Algorithm.h"
#include <dqm_algorithms/OccupancyHoleFinder.h>

namespace dqm_algorithms {


  struct MDT_OccupancyHoleFinder : public OccupancyHoleFinder
  {
    MDT_OccupancyHoleFinder(): OccupancyHoleFinder("MDT") {
    };    
  };



} //dqm_algorithms

#endif // DQM_ALGORITHMS_MDT_OCCUPANCYHOLEFINDER_H
