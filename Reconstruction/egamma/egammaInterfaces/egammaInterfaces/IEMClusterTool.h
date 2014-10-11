/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef EGAMMAINTERFACES_IEMCLUSTERTOOL_H
#define EGAMMAINTERFACES_IEMCLUSTERTOOL_H

#include "egammaInterfaces/IegammaBaseTool.h"
#include "xAODCaloEvent/CaloClusterFwd.h"
#include "xAODCaloEvent/CaloClusterFwd.h"
#include "xAODEgamma/EgammaEnums.h"

/**
   @class IEMClusterTool
   Interface for tool egamma/egammaTools/EMClusterTool
   @author Thomas Koffas, Bruno Lenzi
*/

static const InterfaceID IID_IEMClusterTool("IEMClusterTool", 1, 0);

class IEMClusterTool : virtual public IegammaBaseTool {

 public:
  /** @brief Virtual destructor*/
  virtual ~IEMClusterTool() {};
  
  /** @brief AlgTool interface methods */
  static const InterfaceID& interfaceID();
  
  /** @brief initialize method*/
  virtual StatusCode initialize() = 0;
  /** @brief execute on containers */
  virtual StatusCode contExecute() = 0;
  /** @brief finalize method*/
  virtual StatusCode finalize() = 0;
  /** @brief creation of new cluster based on existing one */
  virtual xAOD::CaloCluster* makeNewCluster(const xAOD::CaloCluster&, xAOD::Egamma *eg, xAOD::EgammaParameters::EgammaType) = 0;

  virtual void fillPositionsInCalo(xAOD::CaloCluster* cluster) =0;
};

inline const InterfaceID& IEMClusterTool::interfaceID()
{
  return IID_IEMClusterTool;
}

#endif // EGAMMAINTERFACES_IEMCLUSTERTOOL_H 
