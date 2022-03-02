#ifndef PFCLUSTERFILLER_H
#define PFCLUSTERFILLER_H

#include "eflowRec/eflowRecCluster.h"
#include "eflowRec/PFData.h"

class PFClusterFiller {

public:
  PFClusterFiller(){};
  ~PFClusterFiller(){};

  void fillClustersToRecover(PFData &data) const;
  void fillClustersToConsider(PFData &data, eflowRecClusterContainer &recClusterContainer) const;

};
#endif