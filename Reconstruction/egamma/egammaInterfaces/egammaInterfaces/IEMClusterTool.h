/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef EGAMMAINTERFACES_IEMCLUSTERTOOL_H
#define EGAMMAINTERFACES_IEMCLUSTERTOOL_H

#include "GaudiKernel/IAlgTool.h"
#include "xAODCaloEvent/CaloCluster.h" // cannot use CaloClusterFwd b/c of ClusterSize
#include "xAODEgamma/EgammaEnums.h"
#include "xAODEgamma/ElectronContainerFwd.h"
#include "xAODEgamma/PhotonContainerFwd.h"

/**
   @class IEMClusterTool
   Interface for tool egamma/egammaTools/EMClusterTool
   @author Thomas Koffas, Bruno Lenzi
*/

static const InterfaceID IID_IEMClusterTool("IEMClusterTool", 1, 0);
class CaloDetDescrManager;

class IEMClusterTool : virtual public IAlgTool {

 public:
  /** @brief Virtual destructor*/
  virtual ~IEMClusterTool() {};

  /** @brief AlgTool interface methods */
  static const InterfaceID& interfaceID();

  /** @brief initialize method*/
  virtual StatusCode initialize() = 0;
  /** @brief execute on containers */
  virtual StatusCode contExecute(const EventContext& ctx,
                                 const CaloDetDescrManager& cmgr,
                                 xAOD::ElectronContainer* electronContainer,
                                 xAOD::PhotonContainer* photonContainer) const = 0;

};

inline const InterfaceID& IEMClusterTool::interfaceID()
{
  return IID_IEMClusterTool;
}

#endif // EGAMMAINTERFACES_IEMCLUSTERTOOL_H
