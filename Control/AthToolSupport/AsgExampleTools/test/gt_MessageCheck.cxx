//
// Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
//
//        
//                  Author: Nils Krumnack
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

// Please feel free to contact me (nils.erik.krumnack@cern.ch) for bug
// reports, feature suggestions, praise and complaints.


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

#ifdef XAOD_STANDALONE
#include <xAODRootAccess/Init.h>
#endif

#pragma GCC diagnostic ignored "-Wdeprecated-declarations"

using namespace testing;

//
// unit test
//

namespace asg
{
  ANA_MSG_HEADER (msgTest)
  ANA_MSG_SOURCE (msgTest, "AsgToolsMessageTest")

  ANA_MSG_HEADER (msgTest2)
  ANA_MSG_SOURCE (msgTest2, "AsgToolsMessageTest")

  TEST (MessageCheck, message_default)
  {
    EXPECT_EQ (MSG::INFO, msgTest2::msg().level());
  }

  TEST (MessageCheck, message)
  {
    for (MSG::Level level : {MSG::INFO, MSG::ERROR, MSG::DEBUG})
    {
      SCOPED_TRACE(level);

      MessagePrinterMock message_printer;
      MessagePrinterOverlay overlay (&message_printer);
      EXPECT_CALL (message_printer, print (MSG::ERROR, "AsgToolsMessageTest", MatchesRegex (".*error text")))
        .Times (1);
      EXPECT_CALL (message_printer, print (MSG::INFO, "AsgToolsMessageTest", MatchesRegex (".*info text")))
        .Times (level<=MSG::INFO);
      EXPECT_CALL (message_printer, print (MSG::DEBUG, "AsgToolsMessageTest", MatchesRegex (".*debug text")))
        .Times (level<=MSG::DEBUG);

      msgTest::msg().setLevel (level);
      EXPECT_EQ (level, msgTest::msg().level());

      using namespace msgTest;
      ANA_MSG_ERROR ("error text");
      ANA_MSG_INFO ("info text");
      ANA_MSG_DEBUG ("debug text");
    }
  }
}

ATLAS_GOOGLE_TEST_MAIN
