// Dear Emacs, this is -*- C++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


#ifndef IMVA_CALIBRATION_H_
#define IMVA_CALIBRATION_H_

#include "AsgTools/IAsgTool.h"

// EDM includes
#include "xAODEgamma/EgammaFwd.h"
#include "xAODEgamma/ElectronFwd.h"
#include "xAODEgamma/PhotonFwd.h"
#include "xAODEgamma/EgammaEnums.h"
#include "xAODCaloEvent/CaloClusterFwd.h"
#include "xAODTracking/VertexFwd.h"


class IegammaMVATool : virtual public asg::IAsgTool{
  /// Declare the interface that the class provides
  ASG_TOOL_INTERFACE( IegammaMVATool )
public:
  virtual ~IegammaMVATool() {};

  /** @brief initialize method*/
  virtual StatusCode initialize() = 0;
  /** @brief finalize method*/
  virtual StatusCode finalize() = 0;
 /** @brief execute method*/
  virtual StatusCode execute(xAOD::CaloCluster* cluster,const xAOD::Egamma* eg)=0;
  virtual StatusCode execute(xAOD::CaloCluster* cluster,const xAOD::EgammaParameters::EgammaType egType)=0;
  virtual StatusCode hltexecute(xAOD::CaloCluster* cluster, const std::string& egType)=0;

  virtual float getEnergy(const xAOD::Egamma* eg)=0;
  virtual float getEnergy(const xAOD::CaloCluster* cluster, const xAOD::Egamma*)=0;
  virtual float getEnergy(const xAOD::CaloCluster* cluster, const xAOD::EgammaParameters::EgammaType egType)=0;

  virtual float getEnergy(const xAOD::CaloCluster* cluster, const xAOD::Electron*)=0;
  virtual float getEnergy(const xAOD::CaloCluster* cluster, const xAOD::Photon*)=0;
  virtual float getEnergy(const xAOD::CaloCluster* cluster, const std::string&)=0;




};

#endif
