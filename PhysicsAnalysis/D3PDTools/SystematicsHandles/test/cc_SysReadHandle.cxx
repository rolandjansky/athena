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

#include <SystematicsHandles/SysReadHandle.h>
#include <AnaAlgorithm/AnaAlgorithm.h>
#include <AsgTools/AsgTool.h>

//
// test code
//

// this tries to instantiate the various handles
int main ()
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
