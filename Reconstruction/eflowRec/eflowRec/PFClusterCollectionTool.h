/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef PFCLUSTERCOLLECTIONTOOL_H
#define PFCLUSTERCOLLECTIONTOOL_H

#include "eflowRec/IPFClusterCollectionTool.h"
#include "AthenaBaseComps/AthAlgTool.h"

class PFClusterCollectionTool : public extends<AthAlgTool, IPFClusterCollectionTool> {

  public:
  
  PFClusterCollectionTool(const std::string& type,const std::string& name,const IInterface* parent);

  ~PFClusterCollectionTool() {};

  StatusCode initialize();
  std::unique_ptr<xAOD::CaloClusterContainer> execute(const eflowCaloObjectContainer& theEflowCaloObjectContainer, bool useNonModifiedClusters, xAOD::CaloClusterContainer& theCaloClusterContainer);
  std::unique_ptr<eflowRecClusterContainer> retrieve(const eflowCaloObjectContainer& theEflowCaloObjectContainer, bool useNonModifiedClusters);
  StatusCode finalize();

};
#endif

