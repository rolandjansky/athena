// Dear Emacs, this is -*- C++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


#ifndef IEGAMMA_CALIB_TOOL_H_
#define IEGAMMA_CALIB_TOOL_H_

#include "AsgTools/IAsgTool.h"

//xAOD
#include "xAODEgamma/ElectronFwd.h"
#include "xAODEgamma/PhotonFwd.h"
#include "xAODEgamma/EgammaFwd.h"


#include "PATInterfaces/CorrectionCode.h"
#include "PATInterfaces/ISystematicsTool.h"

// TODO: remove as soon as possibile
#include "ElectronPhotonFourMomentumCorrection/egammaEnergyCorrectionTool.h"

namespace CP {

class IEgammaCalibrationAndSmearingTool : virtual public asg::IAsgTool, virtual public CP::ISystematicsTool{
  /// Declare the interface that the class provides
  ASG_TOOL_INTERFACE( IEgammaCalibrationAndSmearingTool )
public:
  
  virtual ~IEgammaCalibrationAndSmearingTool() {};

  virtual StatusCode initialize() = 0;

  //Apply the correction on a modifyable egamma object (xAOD::Electron or xAOD::Photon) 
  virtual CP::CorrectionCode applyCorrection(xAOD::Egamma &) = 0;

  //Create a corrected copy from a constant egamma object
  virtual CP::CorrectionCode correctedCopy(const xAOD::Electron&, xAOD::Electron*&) = 0;
  virtual CP::CorrectionCode correctedCopy(const xAOD::Photon&, xAOD::Photon*&) = 0;

  //functions to be used per-event
  virtual void forceSmearing( bool force ) = 0;
  virtual void forceScaleCorrection( bool force ) = 0;
  virtual void setRandomSeed(unsigned seed) = 0; 
  
};

}
#endif
