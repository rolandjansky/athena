/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// IegammaCheckEnergyDepositTool.h, (c) ATLAS Detector software 2008
///////////////////////////////////////////////////////////////////

#ifndef EGAMMAINTERFACES_IEGAMMACHECKENERGYDEPOSITTOOL_H
#define EGAMMAINTERFACES_IEGAMMACHECKENERGYDEPOSITTOOL_H

/** @class IegammaCheckEnergyDepositTool
  Interface for the Reconstruction/egamma/egammaTool/egammaCheckEnergyDepositTool

  @author Frederic Derue derue@lpnhe.in2p3.fr

CREATED : 03/07/2009
MODIFIED :
*/

// INCLUDE HEADER FILES:
#include "GaudiKernel/IAlgTool.h"

// Forward declarations

#include "xAODEgamma/EgammaFwd.h"
#include "xAODCaloEvent/CaloClusterFwd.h"

static const InterfaceID IID_IegammaCheckEnergyDepositTool("IegammaCheckEnergyDepositTool", 1, 0);

class IegammaCheckEnergyDepositTool : virtual public IAlgTool
{

 public:

  /** @brief Virtual destructor*/
  virtual ~IegammaCheckEnergyDepositTool() {};
  
  /** @brief AlgTool interface methods */
  static const InterfaceID& interfaceID();
  
  /** @brief initialize method*/
  virtual StatusCode initialize() = 0;
  /** @brief finalize method*/
  virtual StatusCode finalize() = 0;
  /** @brief execute method*/
  virtual bool checkFractioninSampling(const xAOD::Egamma* eg) = 0;
  virtual bool checkFractioninSamplingCluster(const xAOD::CaloCluster* cluster) = 0;

};

inline const InterfaceID& IegammaCheckEnergyDepositTool::interfaceID()
{
  return IID_IegammaCheckEnergyDepositTool;
}

#endif









