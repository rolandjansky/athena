/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#ifndef ZDCANALYSIS_IZDCRECTOOL_H
#define ZDCANALYSIS_IZDCRECTOOL_H

#include "AsgTools/IAsgTool.h"
#include "xAODForward/ZdcModuleContainer.h"

namespace ZDC
{

class IZdcAnalysisTool : virtual public asg::IAsgTool
{
  ASG_TOOL_INTERFACE( ZDC::IZdcAnalysisTool )
  
 public:

  /// Initialize the tool.
  virtual StatusCode initializeTool() = 0;
  virtual StatusCode recoZdcModule(const xAOD::ZdcModule& module) = 0;
  virtual StatusCode recoZdcModules(const xAOD::ZdcModuleContainer& moduleContainer) = 0;
};

}
  
#endif
