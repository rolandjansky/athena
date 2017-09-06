/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef EFLOWCLUSTERCOLLECTIONTOOL_H
#define EFLOWCLUSTERCOLLECTIONTOOL_H

/********************************************************************

NAME:     eflowClusterCollectionTool.h - split off from eflowLCCalibTool
PACKAGE:  offline/Reconstruction/eflowRec

AUTHORS:  M.Hodgkinson
CREATED:  May 2014

********************************************************************/

#include "GaudiKernel/ToolHandle.h"
#include "xAODCaloEvent/CaloCluster.h"
#include "xAODCaloEvent/CaloClusterContainer.h"
#include "eflowRec/IEFlowClusterCollectionTool.h"
#include "AthenaBaseComps/AthAlgTool.h"
class eflowRecClusterContainer;

class eflowClusterCollectionTool : virtual public IEFlowClusterCollectionTool, public AthAlgTool {

  public:
  
  eflowClusterCollectionTool(const std::string& type,const std::string& name,const IInterface* parent);

  ~eflowClusterCollectionTool() {};

  static const InterfaceID& interfaceID();

  StatusCode initialize();
  std::unique_ptr<xAOD::CaloClusterContainer> execute(eflowCaloObjectContainer* theEflowCaloObjectContainer, bool useNonModifiedClusters);
  std::unique_ptr<eflowRecClusterContainer> retrieve(eflowCaloObjectContainer* theEflowCaloObjectContainer, bool useNonModifiedClusters);
  StatusCode finalize();

};
#endif

