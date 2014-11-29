/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//IAsgElectronEfficiencyCorrection.h to be used in the tool.
#ifndef __IASGELECTRONEFFICIENCYCORRECTION__
#define __IASGELECTRONEFFICIENCYCORRECTION__

#include "AsgTools/IAsgTool.h"
#include "xAODEgamma/ElectronFwd.h"
#include "PATInterfaces/ISystematicsTool.h"
namespace xAOD{
  class IParticle;
}


class IAsgElectronEfficiencyCorrectionTool : virtual public asg::IAsgTool, virtual public CP::ISystematicsTool
{
  ASG_TOOL_INTERFACE(IAsgElectronEfficiencyCorrectionTool)

  public:

  virtual const Root::TResult& calculate( const xAOD::IParticle* part ) const = 0;
  virtual const Root::TResult& calculate( const xAOD::Electron* egam ) const = 0;
  virtual const Root::TResult& getTResult() const = 0;

  virtual CP::CorrectionCode getEfficiencyScaleFactor(const xAOD::Electron& inputObject, double& efficiencyScaleFactor) const = 0;
  virtual CP::CorrectionCode applyEfficiencyScaleFactor(xAOD::Electron& inputObject) const = 0;

  virtual ~IAsgElectronEfficiencyCorrectionTool() {}

};








#endif
