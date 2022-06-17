/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/
#include <iostream>
#include "TrigHTTObjects/HTTCluster.h"

std::ostream& operator<<(std::ostream& o, const HTTCluster& cluster)
{
  return o << "Cluster formed from " << cluster.getHitList().size() << " hits";
}
