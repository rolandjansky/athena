/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// vim: ts=2 sw=2
#ifndef HAVE_ICHAINREGISTRY_H
#define HAVE_ICHAINREGISTRY_H

#include "AsgTools/IAsgTool.h"

class IChainRegistry : public virtual asg::IAsgTool {
  ASG_TOOL_INTERFACE(IChainRegistry)

  public:

    virtual ~IChainRegistry() {};
    virtual StatusCode initialize() = 0;

};
#endif
