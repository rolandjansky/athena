#ifndef PFDATA_H
#define PFDATA_H

#include "eflowRec/EtaPhiLUT.h"

class eflowCaloObjectContainer;
class eflowRecTrack;
class eflowRecCluster;

struct PFData
  {
    eflowCaloObjectContainer *caloObjects;
    std::vector<eflowRecTrack *> tracks;
    std::vector<eflowRecCluster *> clusters;
    eflowRec::EtaPhiLUT clusterLUT;
  };

#endif