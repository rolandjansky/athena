#ifndef PFSUBTRACTIONSTATUSSETTER_H
#define PFSUBTRACTIONSTATUSSETTER_H

#include "xAODCaloEvent/CaloCluster.h"
class eflowCaloObject;

#include <vector>
#include <utility>

class PFSubtractionStatusSetter {

public:
  void markSubtractionStatus(const std::vector<std::pair<xAOD::CaloCluster*, bool> >& clusterList, eflowCaloObject& thisEflowCaloObject);
    
};
#endif
