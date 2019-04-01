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
#include "AthenaMonitoring/HistogramFiller/HistogramFillerFactory.h"

#include "mocks/MockGenericMonitoringTool.h"
#include "mocks/MockITHistSvc.h"

using namespace std;
using namespace Monitored;

#define REGISTER_TEST_CASE(TEST_CASE_NAME) registerTestCase(&HistogramFillerFactoryTestSuite::TEST_CASE_NAME, #TEST_CASE_NAME)

class HistogramFillerFactoryTestSuite {
  // ==================== All registered test cases ====================
  private:
    list<function<void(void)>> registeredTestCases() {
      return {
        REGISTER_TEST_CASE(test_dummy),
      };
    }

  // ==================== Test code ====================
  private:
    void beforeEach() {
      m_gmTool.reset(new MockGenericMonitoringTool());
    }

    void afterEach() {
    }

    void test_dummy() {
      HistogramDef histogramDef;
      histogramDef.type = "TH1F";

      HistogramFillerFactory testObj(m_gmTool.get(), "HistogramFillerFactoryTestSuite");

      testObj.create(histogramDef);
    }

  // ==================== Helper methods ====================
  private:

  // ==================== Initialization & run ====================
  public:
    HistogramFillerFactoryTestSuite() 
      : m_log(Athena::getMessageSvc(), "HistogramFillerFactoryTestSuite") {
    }

    void run() {
      for (function<void(void)> testCase : registeredTestCases()) {
        testCase();
      }
    }

  // ==================== Test case registration ====================
  private:
    typedef void (HistogramFillerFactoryTestSuite::*TestCase)(void);

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
};

int main() {
  ISvcLocator* pSvcLoc;

  if (!Athena_test::initGaudi("GenericMon.txt", pSvcLoc)) {
    throw runtime_error("This test can not be run: GenericMon.txt is missing");
  }

  HistogramFillerFactoryTestSuite().run();

  return 0;
}
