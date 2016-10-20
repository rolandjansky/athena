///////////////////////// -*- C++ -*- /////////////////////////////

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// IMETToolBase.h 
// Header file for interface IMETToolBase
//
// This is the base class for all tools that calculate MET Terms
//
//  * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
//
// Author: P Loch, S Resconi, TJ Khoo
/////////////////////////////////////////////////////////////////// 
	
#ifndef IMETToolBase_H
#define IMETToolBase_H

#include "AsgTools/IAsgTool.h"

#include "xAODMissingET/MissingET.h"
#include "xAODMissingET/MissingETComponentMap.h"
	
class IMETToolBase : virtual public asg::IAsgTool {
  ASG_TOOL_INTERFACE(IMETToolBase)
	
  public:

  virtual StatusCode execute(xAOD::MissingET*,xAOD::MissingETComponentMap*) const = 0;
	
};
	
#endif
