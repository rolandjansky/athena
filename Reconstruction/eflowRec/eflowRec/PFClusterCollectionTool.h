/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef PFCLUSTERCOLLECTIONTOOL_H
#define PFCLUSTERCOLLECTIONTOOL_H

#include "eflowRec/IPFClusterCollectionTool.h"
#include "AthenaBaseComps/AthAlgTool.h"

/**
 Inherits from IPFClusterCollectionTool and AthAlgTool. Creates containers of eflowRecClusters or xAOD::CaloCluster, which can be used in the methods to apply LC weights to the neutral PFO objects in PFLCCalibTool. The xAOD::CaloCluster container is also needed to calculate new cluster moments for the modified calorimeter clusters, which is done in PFMomentCalculatorTool.
*/
class PFClusterCollectionTool : public extends<AthAlgTool, IPFClusterCollectionTool> {

  public:
  
  PFClusterCollectionTool(const std::string& type,const std::string& name,const IInterface* parent);

  ~PFClusterCollectionTool() {};

  StatusCode initialize();
  std::unique_ptr<xAOD::CaloClusterContainer> execute(const eflowCaloObjectContainer& theEflowCaloObjectContainer, bool useNonModifiedClusters);
  std::unique_ptr<eflowRecClusterContainer> retrieve(const eflowCaloObjectContainer& theEflowCaloObjectContainer, bool useNonModifiedClusters);
  StatusCode finalize();

};
#endif

