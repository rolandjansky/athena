/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

/// @author Nils Krumnack


//
// includes
//

#include <SystematicsHandles/SysCopyHandle.h>
#include <AnaAlgorithm/AnaAlgorithm.h>
#include <AsgTools/AsgTool.h>
#include <xAODEventInfo/EventInfo.h>
#include <xAODJet/JetContainer.h>

//
// test code
//

// this tries to instantiate the various handles
void test ()
{
  CP::SystematicSet *sys = nullptr;
  xAOD::EventInfo *object1 = nullptr;
  xAOD::JetContainer *object2 = nullptr;

  EL::AnaAlgorithm *alg = nullptr;
  CP::SysCopyHandle<xAOD::EventInfo> algHandle (alg, "test", "test", "test");
  algHandle.getCopy (object1, *sys);

  asg::AsgTool *tool = nullptr;
  CP::SysCopyHandle<xAOD::JetContainer> toolHandle (tool, "test", "test", "test");
  toolHandle.getCopy (object2, *sys);
}

int main ()
{
  return 0;
}
