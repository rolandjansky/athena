/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

/// @author Nils Krumnack


//
// includes
//

#include <AnaAlgorithm/AnaAlgorithm.h>
#include <AsgTools/AsgTool.h>
#include <SystematicsHandles/SysListHandle.h>
#include <SystematicsHandles/SysReadHandle.h>
#include <SystematicsHandles/SysCopyHandle.h>

//
// test code
//

// this tries to instantiate the various templates
void test ()
{
  EL::AnaAlgorithm *alg = nullptr;
  CP::SysListHandle algProperty (alg);
  CP::SysReadHandle<float> algHandle (alg, "test", "test", "test");
  algProperty.addHandle (algHandle);

  // we probably don't want to use SysListHandle with tools, but
  // we may as well allow it for now.
  asg::AsgTool *tool = nullptr;
  CP::SysListHandle toolProperty (tool);
  CP::SysReadHandle<float> toolHandle (tool, "test", "test", "test");
  toolProperty.addHandle (toolHandle);
}

int main ()
{
  return 0;
}
