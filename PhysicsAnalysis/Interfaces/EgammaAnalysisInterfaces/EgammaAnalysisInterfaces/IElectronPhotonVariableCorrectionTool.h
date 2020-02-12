/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef EGAMMAANALYSISINTERFACES_IELECTRONPHOTONVARIABLECORRECTIONTOOL_H
#define EGAMMAANALYSISINTERFACES_IELECTRONPHOTONVARIABLECORRECTIONTOOL_H

/**
   @class ElectronPhotonVariableCorrectionTool
   @brief Tool to correct electron and photon MC variables.

   @author Nils Gillwald (DESY) nils.gillwald@desy.de
   @date   February 2020


*/

// ATLAS include(s):
#include "AsgTools/IAsgTool.h"

//EDM includes
#include "xAODEgamma/Electron.h"
#include "xAODEgamma/Photon.h"


class IElectronPhotonVariableCorrectionTool : public virtual asg::IAsgTool
{
  // Declare the interface that the class provides
  ASG_TOOL_INTERFACE(IElectronPhotonVariableCorrectionTool)

public:
  /** Gaudi Service Interface method implementations */
  virtual StatusCode initialize() = 0;
  /** Gaudi Service Interface method implementations */
  virtual StatusCode finalize() = 0; //does finalize need to be public? It's called in the destructor...
  
  virtual const StatusCode applyCorrection( xAOD::Photon& photon ) = 0;
  virtual const StatusCode applyCorrection( xAOD::Electron& electron ) = 0;
  virtual const StatusCode correctedCopy( const xAOD::Photon& in_photon, xAOD::Photon*& out_photon ) = 0;
  virtual const StatusCode correctedCopy( const xAOD::Electron& in_electron, xAOD::Electron*& out_electron) = 0;
  virtual const std::string GetCorrectionVariable() = 0;

}; // class IElectronPhotonVariableCorrectionTool

#endif // _IASGFUDGEMCTOOL_H
