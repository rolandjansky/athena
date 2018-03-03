//        Copyright Iowa State University 2017.
//                  Author: Nils Krumnack
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

// Please feel free to contact me (nils.erik.krumnack@cern.ch) for bug
// reports, feature suggestions, praise and complaints.


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
int main ()
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
