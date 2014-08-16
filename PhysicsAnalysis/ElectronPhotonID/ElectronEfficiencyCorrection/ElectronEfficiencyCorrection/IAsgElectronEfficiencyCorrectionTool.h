/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//IAsgElectronEfficiencyCorrection.h to be used in the tool.
#ifndef __IASGELECTRONEFFICIENCYCORRECTION__
#define __IASGELECTRONEFFICIENCYCORRECTION__

#include "AsgTools/IAsgTool.h"
#include "xAODEgamma/EgammaFwd.h"
#include "xAODBase/IParticle.h"


class IAsgElectronEfficiencyCorrectionTool : virtual public asg::IAsgTool
{
  ASG_TOOL_INTERFACE(IAsgElectronEfficiencyCorrectionTool)

  public:

  virtual const Root::TResult& calculate( const xAOD::IParticle* part ) const = 0;
  virtual const Root::TResult& calculate( const xAOD::Egamma* egam ) const = 0;
  virtual const Root::TResult& getTResult() const= 0;

};








#endif
