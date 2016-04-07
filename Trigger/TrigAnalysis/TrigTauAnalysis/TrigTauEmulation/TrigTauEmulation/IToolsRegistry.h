/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// vim: ts=2 sw=2
#ifndef TOOLSREGISTRY_ITOOLSREGISTRY_H
#define TOOLSREGISTRY_ITOOLSREGISTRY_H

#include "AsgTools/IAsgTool.h"

class IToolsRegistry : public virtual asg::IAsgTool
{
  ASG_TOOL_INTERFACE(IToolsRegistry)

 public:
  
  virtual ~IToolsRegistry() {};
  virtual StatusCode initialize() = 0;

};
#endif
