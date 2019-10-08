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

#include "AthenaMonitoringKernel/HistogramDef.h"

#include "mocks/MockHistogramFactory.h"

#include "../src/HistogramFiller/StaticHistogramProvider.h"

using namespace std;
using namespace Monitored;

#define REGISTER_TEST_CASE(TEST_CASE_NAME) registerTestCase(&StaticHistogramProviderTestSuite::TEST_CASE_NAME, #TEST_CASE_NAME)

class StaticHistogramProviderTestSuite {
  // ==================== All registered test cases ====================
  private:
    list<function<void(void)>> registeredTestCases() {
      return {
        REGISTER_TEST_CASE(test_shouldCreateAndReturnJustOneHistogram),
      };
    }

  // ==================== Test code ====================
  private:
    void beforeEach() {
      m_histogramFactory.reset(new MockHistogramFactory());
    }

    void afterEach() {
    }

    void test_shouldCreateAndReturnJustOneHistogram() {
      TNamed histogram;
      HistogramDef histogramDef;
      m_histogramFactory->mock_create = [&histogram, &histogramDef](const HistogramDef& def) mutable {
        VALUE(&def) EXPECTED(&histogramDef);
        return &histogram;
      };

      StaticHistogramProvider testObj(m_histogramFactory, histogramDef);
      TNamed* firstResult = testObj.histogram();
      TNamed* secondResult = testObj.histogram();

      VALUE(firstResult) EXPECTED(&histogram);
      VALUE(secondResult) EXPECTED(&histogram);
    }

  // ==================== Helper methods ====================
  private:

  // ==================== Initialization & run ====================
  public:
    StaticHistogramProviderTestSuite() 
      : m_log(Athena::getMessageSvc(), "StaticHistogramProviderTestSuite") {
    }

    void run() {
      for (function<void(void)> testCase : registeredTestCases()) {
        testCase();
      }
    }

  // ==================== Test case registration ====================
  private:
    typedef void (StaticHistogramProviderTestSuite::*TestCase)(void);

    function<void(void)> registerTestCase(TestCase testCase, const string& testCaseName) {
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

    shared_ptr<MockHistogramFactory> m_histogramFactory;
};

int main() {
  ISvcLocator* pSvcLoc;

  if (!Athena_test::initGaudi("GenericMon.txt", pSvcLoc)) {
    throw runtime_error("This test can not be run: GenericMon.txt is missing");
  }

  StaticHistogramProviderTestSuite().run();

  return 0;
}
