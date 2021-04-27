/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/
#ifndef EGAMMAANALYSISINTERFACES_IEGAMMAMVACALIBTOOL_H
#define EGAMMAANALYSISINTERFACES_IEGAMMAMVACALIBTOOL_H

#include "AsgTools/IAsgTool.h"

// EDM includes
#include "xAODEgamma/EgammaFwd.h"
#include "xAODCaloEvent/CaloClusterFwd.h"

/**
 * @class IegammaMVACalibTool
 * @brief A tool used by the egammaMVASvc to help manage the MVAs.
 **/
class IegammaMVACalibTool : virtual public asg::IAsgTool{
  ASG_TOOL_INTERFACE(IegammaMVACalibTool)
public:
  virtual ~IegammaMVACalibTool() override {};

  ///Return MVA energy for the given cluster, an eg object is optional
  virtual float getEnergy(const xAOD::CaloCluster& clus,
                         const xAOD::Egamma* eg) const = 0;

};

#endif
