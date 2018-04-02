/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

/// @author Nils Krumnack


//
// includes
//

#include <SystematicsHandles/SysReadHandle.h>
#include <AnaAlgorithm/AnaAlgorithm.h>
#include <AsgTools/AsgTool.h>

//
// test code
//

// this tries to instantiate the various handles
void test ()
{
  CP::SystematicSet *sys = nullptr;
  const float *object = nullptr;

  EL::AnaAlgorithm *alg = nullptr;
  CP::SysReadHandle<float> algHandle (alg, "test", "test", "test");
  algHandle.retrieve (object, *sys);

  asg::AsgTool *tool = nullptr;
  CP::SysReadHandle<float> toolHandle (tool, "test", "test", "test");
  toolHandle.retrieve (object, *sys);
}

int main ()
{
  return 0;
}
