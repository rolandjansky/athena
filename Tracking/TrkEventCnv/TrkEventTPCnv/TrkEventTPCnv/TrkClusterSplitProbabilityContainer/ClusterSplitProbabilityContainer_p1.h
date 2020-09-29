/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef _ClusterSplitProbabilityContainer_p1_H_
#define _ClusterSplitProbabilityContainer_p1_H_

#include "ClusterSplitProbability_p1.h"
#include <vector>

namespace Trk {
   class ClusterSplitProbabilityContainer_p1 : public std::vector<ClusterSplitProbability_p1> {};
}
#endif
