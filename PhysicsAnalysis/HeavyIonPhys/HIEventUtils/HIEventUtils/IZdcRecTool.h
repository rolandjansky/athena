/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef __IZDCRECTOOL_H__
#define __IZDCRECTOOL_H__

#include "AsgTools/IAsgTool.h"
#include "xAODForward/ZdcModuleContainer.h"

namespace ZDC
{

class IZdcRecTool : virtual public asg::IAsgTool
{
  ASG_TOOL_INTERFACE( IZdcRecTool )
  
 public:

  /// Initialize the tool.
  virtual StatusCode initializeTool() = 0;
  virtual StatusCode recoZdcModule(const xAOD::ZdcModule& module) = 0;
  virtual StatusCode recoZdcModules(const xAOD::ZdcModuleContainer& moduleContainer) = 0;
  virtual StatusCode reprocessZdc() = 0;
};

}
  
#endif
