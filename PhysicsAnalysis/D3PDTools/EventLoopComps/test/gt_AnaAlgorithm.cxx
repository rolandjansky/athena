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

#include <EventLoopComps/AnaAlgorithm.h>
#include <EventLoopComps/AnaAlgorithmConfig.h>

#include <AsgTools/ToolHandle.h>
#include <AsgTools/MessageCheck.h>
#include <AsgTools/UnitTest.h>
#include <cmath>
#include <gtest/gtest.h>

#ifdef ROOTCORE
#include <xAODRootAccess/Init.h>
#endif

#include <RootCoreUtils/Assert.h>

using namespace EL;

//
// unit test
//

TEST (AnaAlgorithmTest, create)
{
  AnaAlgorithmConfig config;
  config.setName ("name");
  config.setType ("EL::AnaAlgorithm");
  std::unique_ptr<AnaAlgorithm> alg;
  ASSERT_SUCCESS (config.makeAlgorithm (alg));
  ASSERT_NE (nullptr, alg.get());
  ASSERT_EQ ("name", alg->name());
}

ATLAS_GOOGLE_TEST_MAIN
