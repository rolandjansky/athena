/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

/// @author Nils Krumnack


//
// includes
//

#include <SystematicsHandles/SysDecorationHandle.h>
#include <AnaAlgorithm/AnaAlgorithm.h>
#include <AsgTools/AsgTool.h>

//
// test code
//

// this tries to instantiate the various handles
void test ()
{
  CP::SystematicSet *sys = nullptr;
  const SG::AuxElement *constObject = nullptr;
  SG::AuxElement *object = nullptr;

  EL::AnaAlgorithm *alg = nullptr;
  CP::SysDecorationHandle<float> algHandle (alg, "test", "test", "test");
  float value = algHandle.get (*constObject, *sys);
  algHandle.set (*object, value, *sys);

  asg::AsgTool *tool = nullptr;
  CP::SysDecorationHandle<float> toolHandle (tool, "test", "test", "test");
}

int main ()
{
  return 0;
}
