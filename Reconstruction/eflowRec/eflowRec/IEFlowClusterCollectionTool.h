/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef IEFLOWCLUSTERCOLLECTIONTOOL_H
#define IEFLOWCLUSTERCOLLECTIONTOOL_H

#include "GaudiKernel/IAlgTool.h"

class eflowCaloObjectContainer;
class eflowRecClusterContainer;

static const InterfaceID IID_IEFlowClusterCollectionTool("IEFlowClusterCollectionTool", 1, 0);

class IEFlowClusterCollectionTool : virtual public IAlgTool {

  public:
  /**Declared here, and defined below*/
  static const InterfaceID& interfaceID();
  
   /** Put all clusters into a temporary container - then we use this to calculate moments, some of which depend on configuration of nearby clusters */
  virtual std::unique_ptr<xAOD::CaloClusterContainer> execute(eflowCaloObjectContainer* theEflowCaloObjectContainer, bool useNonModifiedClusters) = 0;
  virtual std::unique_ptr<eflowRecClusterContainer> retrieve(eflowCaloObjectContainer* theEflowCaloObjectContainer, bool useNonModifiedClusters) = 0;

};

inline const InterfaceID& IEFlowClusterCollectionTool::interfaceID()
{ 
  return IID_IEFlowClusterCollectionTool;
}


#endif
