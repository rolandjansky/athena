/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef EGAMMAINTERFACES_IEGAMMATOPOCLUSTERCOPIER_H
#define EGAMMAINTERFACES_IEGAMMATOPOCLUSTERCOPIER_H

#include "egammaInterfaces/IegammaBaseTool.h"
#include "xAODCaloEvent/CaloCluster.h" // cannot use CaloClusterFwd b/c of ClusterSize
#include "xAODEgamma/EgammaEnums.h"

/**
   @class IegammaTopoClusterCopier
   Interface for tool egamma/egammaTools/egammaTopoClusterCopier
   @author christos
*/

static const InterfaceID IID_IegammaTopoClusterCopier("IegammaTopoClusterCopier", 1, 0);

class IegammaTopoClusterCopier : virtual public IegammaBaseTool {

 public:
  /** @brief Virtual destructor*/
  virtual ~IegammaTopoClusterCopier() {};  
  /** @brief AlgTool interface methods */
  static const InterfaceID& interfaceID();
  /** @brief initialize method*/
  virtual StatusCode initialize() = 0;
  /** @brief execute on containers */
  virtual StatusCode contExecute() = 0;
  /** @brief finalize method*/
  virtual StatusCode finalize() = 0;

};

inline const InterfaceID& IegammaTopoClusterCopier::interfaceID()
{
  return IID_IegammaTopoClusterCopier;
}

#endif // EGAMMAINTERFACES_IEMCLUSTERTOOL_H 
