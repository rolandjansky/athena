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
#include <AnaAlgorithm/AnaAlgorithmConfig.h>

#include <EventLoopTest/UnitTestAlg2.h>
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

TEST (AnaAlgorithmTest, create_basic)
{
  AnaAlgorithmConfig config;
  config.setName ("name");
  config.setType ("EL::AnaAlgorithm");
  std::unique_ptr<AnaAlgorithm> alg;
  ASSERT_SUCCESS (config.makeAlgorithm (alg));
  ASSERT_NE (nullptr, alg.get());
  ASSERT_EQ ("name", alg->name());
}

TEST (AnaAlgorithmTest, newAlg)
{
  std::unique_ptr<UnitTestAlg2> alg (new UnitTestAlg2 ("name", nullptr));
}

TEST (AnaAlgorithmTest, create)
{
  AnaAlgorithmConfig config;
  config.setName ("name");
  config.setType ("EL::UnitTestAlg2");
  std::unique_ptr<AnaAlgorithm> alg;
  ASSERT_SUCCESS (config.makeAlgorithm (alg));
  ASSERT_NE (nullptr, alg.get());
  ASSERT_EQ ("name", alg->name());
}

TEST (AnaAlgorithmTest, setProperty_string)
{
  AnaAlgorithmConfig config;
  config.setName ("name");
  config.setType ("EL::UnitTestAlg2");
  ASSERT_SUCCESS (config.setProperty ("string_property", "42"));
  std::unique_ptr<AnaAlgorithm> alg;
  ASSERT_SUCCESS (config.makeAlgorithm (alg));
  UnitTestAlg2 *myalg = dynamic_cast<UnitTestAlg2*>(alg.get());
  ASSERT_NE (nullptr, myalg);
  ASSERT_EQ ("42", myalg->m_string_property);
}

TEST (AnaAlgorithmTest, setProperty)
{
  AnaAlgorithmConfig config;
  config.setName ("name");
  config.setType ("EL::UnitTestAlg2");
  ASSERT_SUCCESS (config.setProperty ("property", 42));
  std::unique_ptr<AnaAlgorithm> alg;
  ASSERT_SUCCESS (config.makeAlgorithm (alg));
  UnitTestAlg2 *myalg = dynamic_cast<UnitTestAlg2*>(alg.get());
  ASSERT_NE (nullptr, myalg);
  ASSERT_EQ (42, myalg->m_property);
}

ATLAS_GOOGLE_TEST_MAIN
