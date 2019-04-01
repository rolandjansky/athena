/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#undef NDEBUG
#include <cassert>
#include <iostream>
#include <list>
#include <functional>
#include <memory>

#include "TestTools/initGaudi.h"
#include "TestTools/expect.h"
#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/ITHistSvc.h"
#include "AthenaKernel/getMessageSvc.h"

#include "TH1.h"
#include "TH2.h"
#include "TProfile.h"
#include "TProfile2D.h"

#include "AthenaMonitoring/HistogramDef.h"

#include "mocks/MockGenericMonitoringTool.h"
#include "mocks/MockHistogramFactory.h"

#include "../src/HistogramFiller/LumiblockHistogramProvider.h"

using namespace std;
using namespace Monitored;

#define REGISTER_TEST_CASE(TEST_CASE_NAME) registerTestCase(&LumiblockHistogramProviderTestSuite::TEST_CASE_NAME, #TEST_CASE_NAME)

class LumiblockHistogramProviderTestSuite {
  // ==================== All registered test cases ====================
  private:
    list<function<void(void)>> registeredTestCases() {
      return {
        REGISTER_TEST_CASE(test_shouldRetriveLumiBlockNumberFromGMTool),
      };
    }

  // ==================== Test code ====================
  private:
    void beforeEach() {
      m_gmTool.reset(new MockGenericMonitoringTool());
      m_histogramFactory.reset(new MockHistogramFactory());
    }

    void afterEach() {
    }

    void test_shouldRetriveLumiBlockNumberFromGMTool() {
      HistogramDef histogramDef;
      histogramDef.opt = "kLBNHistoryDepth=10";
      bool invoked = false;

      m_gmTool->mock_lumiBlock = [&invoked]() {
        invoked = true;
        return 0;
      };

      LumiblockHistogramProvider testObj(m_gmTool.get(), m_histogramFactory, histogramDef);
      testObj.histogram();

      VALUE(invoked) EXPECTED(true);
    }

  // ==================== Helper methods ====================
  private:

  // ==================== Initialization & run ====================
  public:
    LumiblockHistogramProviderTestSuite() 
      : m_log(Athena::getMessageSvc(), "LumiblockHistogramProviderTestSuite") {
    }

    void run() {
      for (function<void(void)> testCase : registeredTestCases()) {
        testCase();
      }
    }

  // ==================== Test case registration ====================
  private:
    typedef void (LumiblockHistogramProviderTestSuite::*TestCase)(void);

    function<void(void)> registerTestCase(TestCase testCase, string testCaseName) {
      return [this, testCase, testCaseName]() {
        m_log << MSG::INFO << "Current test case: " << testCaseName << endmsg;
        beforeEach();
        invoke(testCase, this);
        afterEach();
      };
    }

  // ==================== Properties ====================
  private:
    MsgStream m_log;

    shared_ptr<MockGenericMonitoringTool> m_gmTool;
    shared_ptr<MockHistogramFactory> m_histogramFactory;
};

int main() {
  ISvcLocator* pSvcLoc;

  if (!Athena_test::initGaudi("GenericMon.txt", pSvcLoc)) {
    throw runtime_error("This test can not be run: GenericMon.txt is missing");
  }

  LumiblockHistogramProviderTestSuite().run();

  return 0;
}
