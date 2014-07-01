///////////////////////// -*- C++ -*- /////////////////////////////

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// IMETRecoTool.h 
// Header file for interface IMETRecoTool
//
// This is the tool that runs the MET reconstruction chain,
// scheduling the reconstruction tools (inheriting from IMEToolBase)
//
//  * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
//
// Author: P Loch, S Resconi, TJ Khoo
/////////////////////////////////////////////////////////////////// 
	
#ifndef METRECOINTERFACE_IMETRECOTOOL_H
#define METRECOINTERFACE_IMETRECOTOOL_H
	
#include "AsgTools/IAsgTool.h"
	
class IMETRecoTool : virtual public asg::IAsgTool {
  ASG_TOOL_INTERFACE(IMETRecoTool)
	
    public:
	
  virtual StatusCode execute() const = 0;
	
};
	
#endif
