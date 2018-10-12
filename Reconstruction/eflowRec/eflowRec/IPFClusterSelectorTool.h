/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

#ifndef IPFCLUSTERSELECTORTOOL_H
#define IPFCLUSTERSELECTORTOOL_H

#include "GaudiKernel/IAlgTool.h"
#include "xAODCaloEvent/CaloClusterContainer.h"
#include "eflowRec/eflowRecCluster.h"

class IPFClusterSelectorTool : virtual public IAlgTool {

 public:

  /** Execute method to modify xAOD::CaloClusterContainer */
  virtual StatusCode execute(eflowRecClusterContainer&,xAOD::CaloClusterContainer&) = 0;

  DeclareInterfaceID(IPFClusterSelectorTool,1,0);

};
#endif
