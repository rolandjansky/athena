/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "xAODCaloEvent/CaloCluster.h"

std::unique_ptr<xAOD::CaloCluster> xAOD::prepareElementForShallowCopy(const xAOD::CaloCluster* orgCluster) {
  //std::cout << "Executing overloaded version of prepareElementForShallowCopy" << std::endl;
  std::unique_ptr<xAOD::CaloCluster> newCluster(new CaloCluster());
  newCluster->setSamplingPattern(orgCluster->samplingPattern(),false);
  return newCluster;
}

