/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

/// @author Nils Krumnack



//
// includes
//

#include <AnaAlgorithm/AnaAlgorithm.h>
#include <AnaAlgorithm/AnaAlgorithmConfig.h>

#include <EventLoopTest/UnitTestAlg2.h>
#include <AsgTools/ToolHandle.h>
#include <AsgTools/MessageCheck.h>
#include <AsgTesting/UnitTest.h>
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

TEST (AnaAlgorithmTest, setOutputLevel)
{
  AnaAlgorithmConfig config;
  config.setName ("name");
  config.setType ("EL::UnitTestAlg2");
  ASSERT_SUCCESS (config.setProperty ("OutputLevel", MSG::Level::VERBOSE));
  std::unique_ptr<AnaAlgorithm> alg;
  ASSERT_SUCCESS (config.makeAlgorithm (alg));
  UnitTestAlg2 *myalg = dynamic_cast<UnitTestAlg2*>(alg.get());
  ASSERT_NE (nullptr, myalg);
  ASSERT_EQ (MSG::Level::VERBOSE, static_cast<int>(myalg->msg().level()));
}

TEST (AnaAlgorithmTest, setSubTool)
{
  AnaAlgorithmConfig config;
  config.setName ("name");
  config.setType ("EL::UnitTestAlg2");
  ASSERT_SUCCESS (config.createPrivateTool ("toolHandle", "EL::UnitTestTool"));
  ASSERT_SUCCESS (config.setProperty ("toolHandle.propertyInt", 17));
  std::unique_ptr<AnaAlgorithm> alg;
  ASSERT_SUCCESS (config.makeAlgorithm (alg));
  UnitTestAlg2 *myalg = dynamic_cast<UnitTestAlg2*>(alg.get());
  ASSERT_NE (nullptr, myalg);
  ASSERT_NE (nullptr, &*myalg->m_toolHandle);
  ASSERT_EQ (17, myalg->m_toolHandle->getPropertyInt());
  ASSERT_EQ (nullptr, myalg->m_toolHandle->getSubtool());
}

TEST (AnaAlgorithmTest, setSubSubTool)
{
  AnaAlgorithmConfig config;
  config.setName ("name");
  config.setType ("EL::UnitTestAlg2");
  ASSERT_SUCCESS (config.createPrivateTool ("toolHandle", "EL::UnitTestTool"));
  ASSERT_SUCCESS (config.createPrivateTool ("toolHandle.subtool", "EL::UnitTestTool"));
  ASSERT_SUCCESS (config.setProperty ("toolHandle.subtool.propertyInt", 17));
  std::unique_ptr<AnaAlgorithm> alg;
  ASSERT_SUCCESS (config.makeAlgorithm (alg));
  UnitTestAlg2 *myalg = dynamic_cast<UnitTestAlg2*>(alg.get());
  ASSERT_NE (nullptr, myalg);
  ASSERT_NE (nullptr, &*myalg->m_toolHandle);
  ASSERT_EQ (0, myalg->m_toolHandle->getPropertyInt());
  ASSERT_NE (nullptr, myalg->m_toolHandle->getSubtool());
  ASSERT_EQ (17, myalg->m_toolHandle->getSubtool()->getPropertyInt());
}

ATLAS_GOOGLE_TEST_MAIN
