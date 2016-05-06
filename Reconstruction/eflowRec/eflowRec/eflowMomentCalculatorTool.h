/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef EFLOWMOMENTCALCULATORTOOL_H
#define EFLOWMOMENTCALCULATORTOOL_H

/********************************************************************

NAME:     eflowMomentCalculatorTool - split from eflowLCCalibTool
PACKAGE:  offline/Reconstruction/eflowRec

AUTHORS:  M.Hodgkinson
CREATED:  May 2014

********************************************************************/

#include "AthenaBaseComps/AthAlgTool.h"
#include "GaudiKernel/ToolHandle.h"
#include "xAODCaloEvent/CaloCluster.h"
#include "xAODCaloEvent/CaloClusterContainer.h"
#include "CaloRec/CaloClusterCollectionProcessor.h"
#include "eflowRec/eflowBaseAlgTool.h"
#include "eflowRec/IEFlowClusterCollectionTool.h"

#include <cassert>

class eflowCaloObjectContainer;

static const InterfaceID IID_eflowMomentCalculatorTool("eflowMomentCalculatorTool", 1, 0);

class eflowMomentCalculatorTool : virtual public eflowBaseAlgTool, public AthAlgTool {

  public:
  
  eflowMomentCalculatorTool(const std::string& type,const std::string& name,const IInterface* parent);

  ~eflowMomentCalculatorTool() {}

  static const InterfaceID& interfaceID();

  StatusCode initialize();
  void execute(eflowCaloObjectContainer* theEflowCaloObjectContainer);
  StatusCode finalize();

 private:

  /** Tool to put all clusters into a temporary container - then we use this to calculate moments, some of which depend on configuration of nearby clusters */
  ToolHandle<IEFlowClusterCollectionTool> m_clusterCollectionTool;
  
  /* Tool to calculate cluster moments */
  ToolHandle<CaloClusterCollectionProcessor> m_clusterMomentsMaker;

  /** Bool to toggle whether we are in LC mode - false by default */
  bool m_LCMode;

};

inline const InterfaceID& eflowMomentCalculatorTool::interfaceID()
{ 
  return IID_eflowMomentCalculatorTool;
}

#endif
