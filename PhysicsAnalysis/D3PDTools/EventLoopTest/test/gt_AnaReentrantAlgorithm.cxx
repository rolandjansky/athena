/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

/// @author Nils Krumnack



//
// includes
//

#include <AnaAlgorithm/AnaReentrantAlgorithm.h>
#include <AnaAlgorithm/AnaReentrantAlgorithmConfig.h>

#include <AnaAlgorithm/AlgorithmWorkerData.h>
#include <EventLoopTest/UnitTestAlg6.h>
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

TEST (AnaReentrantAlgorithmTest, create_basic)
{
  AlgorithmWorkerData workerData;
  AnaReentrantAlgorithmConfig config;
  config.setName ("name");
  config.setType ("EL::AnaReentrantAlgorithm");
  std::unique_ptr<AnaReentrantAlgorithm> alg;
  ASSERT_SUCCESS (config.makeAlgorithm (alg, workerData));
  ASSERT_NE (nullptr, alg.get());
  ASSERT_EQ ("name", alg->name());
}

TEST (AnaReentrantAlgorithmTest, newAlg)
{
  std::unique_ptr<UnitTestAlg6> alg (new UnitTestAlg6 ("name", nullptr));
}

TEST (AnaReentrantAlgorithmTest, create)
{
  AlgorithmWorkerData workerData;
  AnaReentrantAlgorithmConfig config;
  config.setName ("name");
  config.setType ("EL::UnitTestAlg6");
  std::unique_ptr<AnaReentrantAlgorithm> alg;
  ASSERT_SUCCESS (config.makeAlgorithm (alg, workerData));
  ASSERT_NE (nullptr, alg.get());
  ASSERT_EQ ("name", alg->name());
}

TEST (AnaReentrantAlgorithmTest, setProperty_string)
{
  AlgorithmWorkerData workerData;
  AnaReentrantAlgorithmConfig config;
  config.setName ("name");
  config.setType ("EL::UnitTestAlg6");
  ASSERT_SUCCESS (config.setProperty ("string_property", "42"));
  std::unique_ptr<AnaReentrantAlgorithm> alg;
  ASSERT_SUCCESS (config.makeAlgorithm (alg, workerData));
  UnitTestAlg6 *myalg = dynamic_cast<UnitTestAlg6*>(alg.get());
  ASSERT_NE (nullptr, myalg);
  ASSERT_EQ ("42", myalg->m_string_property);
}

TEST (AnaReentrantAlgorithmTest, setProperty)
{
  AlgorithmWorkerData workerData;
  AnaReentrantAlgorithmConfig config;
  config.setName ("name");
  config.setType ("EL::UnitTestAlg6");
  ASSERT_SUCCESS (config.setProperty ("property", 42));
  std::unique_ptr<AnaReentrantAlgorithm> alg;
  ASSERT_SUCCESS (config.makeAlgorithm (alg, workerData));
  UnitTestAlg6 *myalg = dynamic_cast<UnitTestAlg6*>(alg.get());
  ASSERT_NE (nullptr, myalg);
  ASSERT_EQ (42, myalg->m_property);
}

TEST (AnaReentrantAlgorithmTest, setOutputLevel)
{
  AlgorithmWorkerData workerData;
  AnaReentrantAlgorithmConfig config;
  config.setName ("name");
  config.setType ("EL::UnitTestAlg6");
  ASSERT_SUCCESS (config.setProperty ("OutputLevel", MSG::Level::VERBOSE));
  std::unique_ptr<AnaReentrantAlgorithm> alg;
  ASSERT_SUCCESS (config.makeAlgorithm (alg, workerData));
  UnitTestAlg6 *myalg = dynamic_cast<UnitTestAlg6*>(alg.get());
  ASSERT_NE (nullptr, myalg);
  ASSERT_EQ (MSG::Level::VERBOSE, static_cast<int>(myalg->msg().level()));
}

TEST (AnaReentrantAlgorithmTest, setSubTool)
{
  AlgorithmWorkerData workerData;
  AnaReentrantAlgorithmConfig config;
  config.setName ("name");
  config.setType ("EL::UnitTestAlg6");
  ASSERT_SUCCESS (config.createPrivateTool ("toolHandle", "EL::UnitTestTool"));
  ASSERT_SUCCESS (config.setProperty ("toolHandle.propertyInt", 17));
  std::unique_ptr<AnaReentrantAlgorithm> alg;
  ASSERT_SUCCESS (config.makeAlgorithm (alg, workerData));
  UnitTestAlg6 *myalg = dynamic_cast<UnitTestAlg6*>(alg.get());
  ASSERT_NE (nullptr, myalg);
  ASSERT_NE (nullptr, &*myalg->m_toolHandle);
  ASSERT_EQ (17, myalg->m_toolHandle->getPropertyInt());
  ASSERT_EQ (nullptr, myalg->m_toolHandle->getSubtool());
}

TEST (AnaReentrantAlgorithmTest, setSubSubTool)
{
  AlgorithmWorkerData workerData;
  AnaReentrantAlgorithmConfig config;
  config.setName ("name");
  config.setType ("EL::UnitTestAlg6");
  ASSERT_SUCCESS (config.createPrivateTool ("toolHandle", "EL::UnitTestTool"));
  ASSERT_SUCCESS (config.createPrivateTool ("toolHandle.subtool", "EL::UnitTestTool"));
  ASSERT_SUCCESS (config.setProperty ("toolHandle.subtool.propertyInt", 17));
  std::unique_ptr<AnaReentrantAlgorithm> alg;
  ASSERT_SUCCESS (config.makeAlgorithm (alg, workerData));
  UnitTestAlg6 *myalg = dynamic_cast<UnitTestAlg6*>(alg.get());
  ASSERT_NE (nullptr, myalg);
  ASSERT_NE (nullptr, &*myalg->m_toolHandle);
  ASSERT_EQ (0, myalg->m_toolHandle->getPropertyInt());
  ASSERT_NE (nullptr, myalg->m_toolHandle->getSubtool());
  ASSERT_EQ (17, myalg->m_toolHandle->getSubtool()->getPropertyInt());
}

ATLAS_GOOGLE_TEST_MAIN
