// Dear Emacs, this is -*- C++ -*-

/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/


#ifndef EGAMMAANALYSISINTERFACES_IEGAMMAMVASVC_H
#define EGAMMAANALYSISINTERFACES_IEGAMMAMVASVC_H

#include "AsgServices/IAsgService.h"
#include "AsgMessaging/StatusCode.h"
// EDM includes
#include "xAODEgamma/EgammaFwd.h"
#include "xAODEgamma/EgammaEnums.h"
#include "xAODCaloEvent/CaloClusterFwd.h"


class IegammaMVASvc : virtual public asg::IAsgService {
public:
  DeclareInterfaceID(IegammaMVASvc,1,0);

  virtual ~IegammaMVASvc() override {};

  /** Compute the calibrated energy **/
  virtual StatusCode getEnergy(const xAOD::CaloCluster& cluster,
                               const xAOD::Egamma& eg,
                               double& energy) const = 0;

  /** Compute the calibrated energy when the full egamma object is not available **/
  virtual StatusCode getEnergy(const xAOD::CaloCluster& cluster,
                               const xAOD::EgammaParameters::EgammaType egType,
                               double& energy) const = 0;


  /// Standard execute when you have both cluster and eg object
  virtual StatusCode execute(xAOD::CaloCluster& cluster,
                             const xAOD::Egamma& eg) const = 0;

  /// An execute that just has cluster and egType. A converted photon
  /// is treated like an unconverted photon since no access to vertex.
  virtual StatusCode execute(xAOD::CaloCluster& cluster,
                             const xAOD::EgammaParameters::EgammaType egType) const = 0;


};

#endif
