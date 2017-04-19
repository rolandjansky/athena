/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//IAsgPhotonEfficiencyCorrection.h to be used in the tool.
#ifndef __IASGPHOTONEFFICIENCYCORRECTION__
#define __IASGPHOTONEFFICIENCYCORRECTION__

// Framework include(s):
#include "AsgTools/IAsgTool.h"

// EDM include(s):
#include "xAODEgamma/Egamma.h"

// Local include(s):
#include "PATInterfaces/CorrectionCode.h"
#include "PATInterfaces/ISystematicsTool.h"

namespace xAOD{
  class IParticle;
}

class IAsgPhotonEfficiencyCorrectionTool : virtual public asg::IAsgTool, virtual public CP::ISystematicsTool
{
  ASG_TOOL_INTERFACE(IAsgPhotonEfficiencyCorrectionTool)

  public:  
  //virtual StatusCode initialize() = 0;
  
  /// Get the "photon scale factor" as a return value
  virtual CP::CorrectionCode getEfficiencyScaleFactor(const xAOD::Egamma&, double&) const = 0;
  
  /// Get the "photon scale factor error" as a return value
  virtual CP::CorrectionCode getEfficiencyScaleFactorError(const xAOD::Egamma&, double&) const = 0;
  
  /// Decorate the photon with its scale factor
  virtual CP::CorrectionCode applyEfficiencyScaleFactor(xAOD::Egamma&) const = 0;
  
  virtual ~IAsgPhotonEfficiencyCorrectionTool() {}  
}; // class IAsgPhotonEfficiencyCorrectionTool

#endif

