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

#include <SystematicsHandles/SysWriteHandle.h>
#include <AnaAlgorithm/AnaAlgorithm.h>
#include <AsgTools/AsgTool.h>

//
// test code
//

// this tries to instantiate the various handles
int main ()
{
  CP::SystematicSet *sys = nullptr;

  EL::AnaAlgorithm *alg = nullptr;
  CP::SysWriteHandle<float> algHandle (alg, "test", "test", "test");
  algHandle.record (std::unique_ptr<float> (), *sys);

  asg::AsgTool *tool = nullptr;
  CP::SysWriteHandle<float,double> toolHandle (tool, "test", "test", "test");
  toolHandle.record (std::unique_ptr<float> (), std::unique_ptr<double> (), *sys);
}
