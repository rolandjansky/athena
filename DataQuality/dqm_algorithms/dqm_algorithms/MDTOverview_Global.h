/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef DQM_ALGORITHMS_MDTOVERVIEW_GLOBAL_H
#define DQM_ALGORITHMS_MDTOVERVIEW_GLOBAL_H

#include "dqm_core/Algorithm.h"
#include "dqm_algorithms/MDTOverview.h"

namespace dqm_algorithms {

  struct MDTOverview_Global : public MDTOverview 
  {
    MDTOverview_Global():  MDTOverview( "MDTOverview_Global" ) {};
  };

} //namespace dqm_algorithms

#endif // DQM_ALGORITHMS_MDTOVERVIEW_GLOBAL_H
