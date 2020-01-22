/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

/// @author Nils Krumnack



//
// includes
//

#include <AsgTools/AnaToolHandle.h>
#include <AsgTools/MessageCheck.h>
#include <AsgTools/IMessagePrinter.h>
#include <AsgTools/MessagePrinterMock.h>
#include <AsgTools/MessagePrinterOverlay.h>
#include <AsgTesting/UnitTest.h>
#include <cmath>
#include <gtest/gtest.h>
#include <gtest/gtest-spi.h>
#include <gmock/gmock.h>
#include <EventLoop/Algorithm.h>

#ifdef ROOTCORE
#include <xAODRootAccess/Init.h>
#endif

#pragma GCC diagnostic ignored "-Wdeprecated-declarations"

using namespace testing;

//
// unit test
//

struct MyAlgorithm : public EL::Algorithm
{
  void message_info (const std::string& message) const
  {
    ANA_MSG_INFO (message);
  }

  void message_debug (const std::string& message) const
  {
    ANA_MSG_DEBUG (message);
  }
};

TEST (AlgorithmMessage, message_name)
{
  std::unique_ptr<MyAlgorithm> alg (new MyAlgorithm);
  
  asg::MessagePrinterMock message_printer;
  asg::MessagePrinterOverlay overlay (&message_printer);
  EXPECT_CALL (message_printer, print (MSG::INFO, "MyName", MatchesRegex (".*message 3")))
    .Times (1);
  EXPECT_CALL (message_printer, print (MSG::INFO, "MyName", MatchesRegex (".*message 2")))
    .Times (1);
  EXPECT_CALL (message_printer, print (MSG::INFO, "UnnamedAlgorithm", MatchesRegex (".*message 1")))
    .Times (1);

  alg->message_info ("message 1");
  alg->SetName ("MyName");
  alg->message_info ("message 2");

  std::unique_ptr<MyAlgorithm> alg2 (static_cast<MyAlgorithm*>(alg->Clone()));
  alg2->message_info ("message 3");
}

TEST (AlgorithmMessage, message_level)
{
  std::unique_ptr<MyAlgorithm> alg (new MyAlgorithm);
  
  asg::MessagePrinterMock message_printer;
  asg::MessagePrinterOverlay overlay (&message_printer);
  EXPECT_CALL (message_printer, print (MSG::DEBUG, "MyName", MatchesRegex (".*message 4")))
    .Times (1);
  EXPECT_CALL (message_printer, print (MSG::DEBUG, "MyName", MatchesRegex (".*message 3")))
    .Times (1);

  alg->SetName ("MyName");
  alg->message_debug ("message 1");
  std::unique_ptr<MyAlgorithm> alg1 (static_cast<MyAlgorithm*>(alg->Clone()));
  alg1->message_debug ("message 2");

  alg->setMsgLevel (MSG::DEBUG);
  alg->message_debug ("message 3");

  std::unique_ptr<MyAlgorithm> alg2 (static_cast<MyAlgorithm*>(alg->Clone()));
  alg2->message_debug ("message 4");
}

ATLAS_GOOGLE_TEST_MAIN
