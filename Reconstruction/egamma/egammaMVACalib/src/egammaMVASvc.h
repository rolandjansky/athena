// Dear Emacs, this is -*- C++ -*-

/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#ifndef EGAMMAMVACALIB_EGAMMAMVASVC_H
#define EGAMMAMVACALIB_EGAMMAMVASVC_H

#include "xAODEgamma/EgammaEnums.h"
#include "egammaInterfaces/IegammaMVASvc.h"
#include "egammaInterfaces/IegammaMVACalibTool.h"
#include "AthenaBaseComps/AthService.h"

#include <string>

class egammaMVASvc : public extends1<AthService, IegammaMVASvc>
{
public:
  egammaMVASvc( const std::string& name, ISvcLocator* svc );

  virtual ~egammaMVASvc() override {};

  virtual StatusCode initialize() override;

  virtual StatusCode finalize() override;

  /** Main execute. We need to calibrate the cluster.
      Use full egamma object instead of Type
      As we employ further variables than the ones present in the cluster
      This method needs to be valid also for reconstruction
  */

  StatusCode execute(xAOD::CaloCluster& cluster,
                     const xAOD::Egamma& eg) const override final;

  StatusCode execute(xAOD::CaloCluster& cluster,
                     const xAOD::EgammaParameters::EgammaType egType) const override final;

private:

  /// MVA tool for electron
  ToolHandle<IegammaMVACalibTool> m_mvaElectron {this,
      "ElectronTool", "", "Tool to handle MVA trees for electrons"};

  /// MVA tool for uncovnerted photon
  ToolHandle<IegammaMVACalibTool> m_mvaUnconvertedPhoton {this,
      "UnconvertedPhotonTool", "", "Tool to handle MVA trees for unconverted photons"};

  /// MVA tool for converted photon
  ToolHandle<IegammaMVACalibTool> m_mvaConvertedPhoton {this,
      "ConvertedPhotonTool", "", "Tool to handle MVA trees for converted photons"};

  Gaudi::Property<float> m_maxConvR {this,
      "MaxConvRadius", 800.0,
      "The maximum conversion radius for a photon to be considered converted"};

};

#endif
