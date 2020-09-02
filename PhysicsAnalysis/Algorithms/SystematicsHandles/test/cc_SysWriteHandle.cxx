/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

/// @author Nils Krumnack


//
// includes
//

#include <SystematicsHandles/SysWriteHandle.h>
#include <AnaAlgorithm/AnaAlgorithm.h>
#include <AsgTools/AsgTool.h>

//
// test code
//

// this tries to instantiate the various handles
void test ()
{
  CP::SystematicSet *sys = nullptr;

  EL::AnaAlgorithm *alg = nullptr;
  CP::SysWriteHandle<float> algHandle (alg, "test", "test", "test");
  algHandle.record (std::unique_ptr<float> (), *sys);

  asg::AsgTool *tool = nullptr;
  CP::SysWriteHandle<float,double> toolHandle (tool, "test", "test", "test");
  toolHandle.record (std::unique_ptr<float> (), std::unique_ptr<double> (), *sys);
}

int main ()
{
  return 0;
}
