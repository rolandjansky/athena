/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef IPFCLUSTERCOLLECTIONTOOL_H
#define IPFCLUSTERCOLLECTIONTOOL_H

#include "GaudiKernel/IAlgTool.h"
#include "xAODCaloEvent/CaloClusterContainer.h"

class eflowCaloObjectContainer;
class eflowRecClusterContainer;

class IPFClusterCollectionTool : virtual public IAlgTool {

  public:
   /** Put all clusters into a temporary container - then we use this to calculate moments, some of which depend on configuration of nearby clusters */
  virtual std::unique_ptr<xAOD::CaloClusterContainer> execute(const eflowCaloObjectContainer& theEflowCaloObjectContainer, bool useNonModifiedClusters, xAOD::CaloClusterContainer& theCaloClusterContainer) = 0;
  virtual std::unique_ptr<eflowRecClusterContainer> retrieve(const eflowCaloObjectContainer& theEflowCaloObjectContainer, bool useNonModifiedClusters) = 0;

  DeclareInterfaceID(IPFClusterCollectionTool,1,0);
  
};

#endif
