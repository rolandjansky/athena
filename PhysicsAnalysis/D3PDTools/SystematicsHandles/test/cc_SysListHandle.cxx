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

#include <AnaAlgorithm/AnaAlgorithm.h>
#include <AsgTools/AsgTool.h>
#include <SystematicsHandles/SysListHandle.h>
#include <SystematicsHandles/SysReadHandle.h>
#include <SystematicsHandles/SysCopyHandle.h>

//
// test code
//

// this tries to instantiate the various templates
int main ()
{
  CP::SystematicSet *sys = nullptr;

  EL::AnaAlgorithm *alg = nullptr;
  EL::SysListHandle algProperty (alg);
  EL::SysReadHandle<float> algHandle (alg, "test", "test", "test");
  algProperty.addHandle (algHandle);

  // we probably don't want to use SysListHandle with tools, but
  // we may as well allow it for now.
  asg::AsgTool *tool = nullptr;
  EL::SysListHandle toolProperty (tool);
  EL::SysReadHandle<float> toolHandle (tool, "test", "test", "test");
  toolProperty.addHandle (toolHandle);
}
