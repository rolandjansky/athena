/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef EGAMMAINTERFACES_IEGAMMATOPOCLUSTERCOPIER_H
#define EGAMMAINTERFACES_IEGAMMATOPOCLUSTERCOPIER_H

#include "egammaInterfaces/IegammaBaseTool.h"
#include "xAODCaloEvent/CaloCluster.h" // cannot use CaloClusterFwd b/c of ClusterSize
#include "AthContainers/ConstDataVector.h"
#include "xAODCaloEvent/CaloClusterContainer.h"
#include "xAODCore/ShallowAuxContainer.h"

/**
   @class IegammaTopoClusterCopier
   Interface for tool egamma/egammaTools/egammaTopoClusterCopier
   @author christos
*/

static const InterfaceID IID_IegammaTopoClusterCopier("IegammaTopoClusterCopier", 1, 1);

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
  /** @brief execute on direct objects */
  virtual StatusCode hltExecute(std::pair<xAOD::CaloClusterContainer*, xAOD::ShallowAuxContainer* > &inputShallowcopy, ConstDataVector<xAOD::CaloClusterContainer>* viewCopy) const = 0;
  /** @brief finalize method*/
  virtual StatusCode finalize() = 0;

};

inline const InterfaceID& IegammaTopoClusterCopier::interfaceID()
{
  return IID_IegammaTopoClusterCopier;
}

#endif // EGAMMAINTERFACES_IEMCLUSTERTOOL_H 
