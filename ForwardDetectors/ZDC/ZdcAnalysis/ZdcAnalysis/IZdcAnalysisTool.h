/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef IZDCANALYSISTOOL_H__
#define IZDCANALYSISTOOL_H__

#include "AsgTools/IAsgTool.h"
#include "xAODForward/ZdcModuleContainer.h"

namespace ZDC
{

class IZdcAnalysisTool : virtual public asg::IAsgTool
{
  ASG_TOOL_INTERFACE( ZDC::IZdcAnalysisTool )

 public:

  virtual StatusCode recoZdcModule(const xAOD::ZdcModule& module) = 0;
  virtual StatusCode recoZdcModules(const xAOD::ZdcModuleContainer& moduleContainer) = 0;
  virtual StatusCode reprocessZdc() = 0;
};

}

#endif
