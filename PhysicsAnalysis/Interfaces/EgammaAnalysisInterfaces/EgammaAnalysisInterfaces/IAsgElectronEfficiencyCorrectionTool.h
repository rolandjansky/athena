/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//IAsgElectronEfficiencyCorrection.h to be used in the tool.
#ifndef EGAMMAANALYSISINTERFACES__IASGELECTRONEFFICIENCYCORRECTION__
#define EGAMMAANALYSISINTERFACES__IASGELECTRONEFFICIENCYCORRECTION__

#include "AsgTools/IAsgTool.h"
#include "xAODEgamma/ElectronFwd.h"
#include "PATInterfaces/CorrectionCode.h"
#include "PATInterfaces/ISystematicsTool.h"
namespace xAOD{
  class IParticle;
}


class IAsgElectronEfficiencyCorrectionTool : virtual public CP::ISystematicsTool
{
  ASG_TOOL_INTERFACE(IAsgElectronEfficiencyCorrectionTool)

  public:
  ///The interface for Scale Factors
  virtual CP::CorrectionCode getEfficiencyScaleFactor(const xAOD::Electron& inputObject, double& efficiencyScaleFactor) const = 0;
  virtual CP::CorrectionCode applyEfficiencyScaleFactor(const xAOD::Electron& inputObject) const = 0;
  virtual int systUncorrVariationIndex( const xAOD::Electron &inputObject) const =0;
  virtual ~IAsgElectronEfficiencyCorrectionTool() {}

};


#endif
