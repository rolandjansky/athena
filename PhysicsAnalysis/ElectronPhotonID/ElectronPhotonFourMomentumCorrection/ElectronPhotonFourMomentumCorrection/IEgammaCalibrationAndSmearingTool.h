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
// PAT includes
#include "PATCore/PATCoreEnums.h"


namespace EgammaCalibPeriodRunNumbersExample {
  const int run_2016 = 297730;
  const int run_2015 = 252604;
  struct ROOT6_NamespaceAutoloadHook{};
}


namespace CP {

  class IEgammaCalibrationAndSmearingTool : public CP::ISystematicsTool{
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
    virtual void setRandomSeed(unsigned seed) = 0;
    
    virtual double resolution( double energy, double cl_eta, double cl_etaCalo,
			       PATCore::ParticleType::Type ptype = PATCore::ParticleType::Electron, bool withCT=false) const = 0;
    virtual double getResolution(const xAOD::Egamma& particle, bool withCT=true) const = 0;
  };
  
}
#endif
