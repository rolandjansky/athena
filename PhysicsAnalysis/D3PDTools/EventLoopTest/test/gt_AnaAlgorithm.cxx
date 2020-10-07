/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

/// @author Nils Krumnack



//
// includes
//

#include <AnaAlgorithm/AnaAlgorithm.h>
#include <AnaAlgorithm/AnaAlgorithmConfig.h>

#include <AnaAlgorithm/AlgorithmWorkerData.h>
#include <EventLoopTest/UnitTestAlg5.h>
#include <AsgTools/ToolHandle.h>
#include <AsgMessaging/MessageCheck.h>
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
  AlgorithmWorkerData workerData;
  AnaAlgorithmConfig config;
  config.setName ("name");
  config.setType ("EL::AnaAlgorithm");
  std::unique_ptr<AnaAlgorithm> alg;
  ASSERT_SUCCESS (config.makeAlgorithm (alg, workerData));
  ASSERT_NE (nullptr, alg.get());
  ASSERT_EQ ("name", alg->name());
}

TEST (AnaAlgorithmTest, newAlg)
{
  std::unique_ptr<UnitTestAlg5> alg (new UnitTestAlg5 ("name", nullptr));
}

TEST (AnaAlgorithmTest, create)
{
  AlgorithmWorkerData workerData;
  AnaAlgorithmConfig config;
  config.setName ("name");
  config.setType ("EL::UnitTestAlg5");
  std::unique_ptr<AnaAlgorithm> alg;
  ASSERT_SUCCESS (config.makeAlgorithm (alg, workerData));
  ASSERT_NE (nullptr, alg.get());
  ASSERT_EQ ("name", alg->name());
}

TEST (AnaAlgorithmTest, setProperty_string)
{
  AlgorithmWorkerData workerData;
  AnaAlgorithmConfig config;
  config.setName ("name");
  config.setType ("EL::UnitTestAlg5");
  ASSERT_SUCCESS (config.setProperty ("string_property", "42"));
  std::unique_ptr<AnaAlgorithm> alg;
  ASSERT_SUCCESS (config.makeAlgorithm (alg, workerData));
  UnitTestAlg5 *myalg = dynamic_cast<UnitTestAlg5*>(alg.get());
  ASSERT_NE (nullptr, myalg);
  ASSERT_EQ ("42", myalg->m_string_property);
}

TEST (AnaAlgorithmTest, setProperty)
{
  AlgorithmWorkerData workerData;
  AnaAlgorithmConfig config;
  config.setName ("name");
  config.setType ("EL::UnitTestAlg5");
  ASSERT_SUCCESS (config.setProperty ("property", 42));
  std::unique_ptr<AnaAlgorithm> alg;
  ASSERT_SUCCESS (config.makeAlgorithm (alg, workerData));
  UnitTestAlg5 *myalg = dynamic_cast<UnitTestAlg5*>(alg.get());
  ASSERT_NE (nullptr, myalg);
  ASSERT_EQ (42, myalg->m_property);
}

TEST (AnaAlgorithmTest, setOutputLevel)
{
  AlgorithmWorkerData workerData;
  AnaAlgorithmConfig config;
  config.setName ("name");
  config.setType ("EL::UnitTestAlg5");
  ASSERT_SUCCESS (config.setProperty ("OutputLevel", MSG::Level::VERBOSE));
  std::unique_ptr<AnaAlgorithm> alg;
  ASSERT_SUCCESS (config.makeAlgorithm (alg, workerData));
  UnitTestAlg5 *myalg = dynamic_cast<UnitTestAlg5*>(alg.get());
  ASSERT_NE (nullptr, myalg);
  ASSERT_EQ (MSG::Level::VERBOSE, static_cast<int>(myalg->msg().level()));
}

TEST (AnaAlgorithmTest, setSubTool)
{
  AlgorithmWorkerData workerData;
  AnaAlgorithmConfig config;
  config.setName ("name");
  config.setType ("EL::UnitTestAlg5");
  ASSERT_SUCCESS (config.createPrivateTool ("toolHandle", "EL::UnitTestTool"));
  ASSERT_SUCCESS (config.setProperty ("toolHandle.propertyInt", 17));
  std::unique_ptr<AnaAlgorithm> alg;
  ASSERT_SUCCESS (config.makeAlgorithm (alg, workerData));
  UnitTestAlg5 *myalg = dynamic_cast<UnitTestAlg5*>(alg.get());
  ASSERT_NE (nullptr, myalg);
  ASSERT_NE (nullptr, &*myalg->m_toolHandle);
  ASSERT_EQ (17, myalg->m_toolHandle->getPropertyInt());
  ASSERT_EQ (nullptr, myalg->m_toolHandle->getSubtool());
}

TEST (AnaAlgorithmTest, setSubSubTool)
{
  AlgorithmWorkerData workerData;
  AnaAlgorithmConfig config;
  config.setName ("name");
  config.setType ("EL::UnitTestAlg5");
  ASSERT_SUCCESS (config.createPrivateTool ("toolHandle", "EL::UnitTestTool"));
  ASSERT_SUCCESS (config.createPrivateTool ("toolHandle.subtool", "EL::UnitTestTool"));
  ASSERT_SUCCESS (config.setProperty ("toolHandle.subtool.propertyInt", 17));
  std::unique_ptr<AnaAlgorithm> alg;
  ASSERT_SUCCESS (config.makeAlgorithm (alg, workerData));
  UnitTestAlg5 *myalg = dynamic_cast<UnitTestAlg5*>(alg.get());
  ASSERT_NE (nullptr, myalg);
  ASSERT_NE (nullptr, &*myalg->m_toolHandle);
  ASSERT_EQ (0, myalg->m_toolHandle->getPropertyInt());
  ASSERT_NE (nullptr, myalg->m_toolHandle->getSubtool());
  ASSERT_EQ (17, myalg->m_toolHandle->getSubtool()->getPropertyInt());
}

ATLAS_GOOGLE_TEST_MAIN
