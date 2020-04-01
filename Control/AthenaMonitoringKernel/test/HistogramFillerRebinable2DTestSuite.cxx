/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

/**
 * Unit test for 2D rebinable histograms
 *
 * Only specific features to 2D histograms (rebinning of Y-axis) is tested here.
 * The main functionality of the rebinning is tested in the "1DTestSuite".
 */

#undef NDEBUG
#include <cassert>
#include <iostream>
#include <list>
#include <functional>

#include "TestTools/initGaudi.h"
#include "TestTools/expect.h"
#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/ITHistSvc.h"
#include "AthenaKernel/getMessageSvc.h"

#include "TH2D.h"

#include "../src/HistogramFiller/HistogramFillerRebinable.h"

#include "mocks/MockHistogramProvider.h"
#include "mocks/MockMonitoredVariable.h"

using namespace std;
using namespace Monitored;

#define REGISTER_TEST_CASE(TEST_CASE_NAME) registerTestCase(&HistogramFillerRebinable2DTestSuite::TEST_CASE_NAME, #TEST_CASE_NAME)

class HistogramFillerRebinable2DTestSuite {
  // ==================== All registered test cases ====================
  private:
    list<function<void(void)>> registeredTestCases() {
      return {
        REGISTER_TEST_CASE(test_shouldKeepNumberOfBinsForValueInHistogramsRange),
        REGISTER_TEST_CASE(test_shouldDoubleNumberOfBinsForValueOutsideRange),
      };
    }

  // ==================== Test code ====================
  private:
    void beforeEach() {
        m_histogramDef.kAddBinsDynamically = true;
        m_histogramProvider.reset(new MockHistogramProvider());
        m_var1.reset(new MockMonitoredVariable(""));
        m_var2.reset(new MockMonitoredVariable(""));
        m_histogram.reset(new TH2D("MockHistogram", "Mock Histogram", 8, 1.0, 3.0, 5, 1.0, 5.0));
        m_testObj.reset(new HistogramFillerRebinable2D(m_histogramDef, m_histogramProvider));

        m_testObj->setMonitoredVariables({ *m_var1, *m_var2 });
        m_histogramProvider->mock_histogram = [this]() { return m_histogram.get(); };
    }

    void afterEach() {
    }

    void test_shouldKeepNumberOfBinsForValueInHistogramsRange() {
      m_var1->mock_getVectorRepresentation = []() -> vector<double> { return {2.9}; };
      m_var2->mock_getVectorRepresentation = []() -> vector<double> { return {4.9}; };

      auto check = [&](){
        VALUE(m_histogram->GetXaxis()->GetNbins()) EXPECTED(8);
        VALUE(m_histogram->GetXaxis()->GetXmin()) EXPECTED(1.0);
        VALUE(m_histogram->GetXaxis()->GetXmax()) EXPECTED(3.0);
        VALUE(m_histogram->GetYaxis()->GetNbins()) EXPECTED(5);
        VALUE(m_histogram->GetYaxis()->GetXmin()) EXPECTED(1.0);
        VALUE(m_histogram->GetYaxis()->GetXmax()) EXPECTED(5.0);
      };
      check();
      m_testObj->fill();
      check();
      VALUE(m_histogram->GetBinContent(8,5)) EXPECTED(1.0);
    }

    void test_shouldDoubleNumberOfBinsForValueOutsideRange() {
      m_var1->mock_getVectorRepresentation = []() -> vector<double> { return {3.0}; };
      m_var2->mock_getVectorRepresentation = []() -> vector<double> { return {5.0}; };

      m_testObj->fill();

      VALUE(m_histogram->GetXaxis()->GetNbins()) EXPECTED(16);
      VALUE(m_histogram->GetXaxis()->GetXmin()) EXPECTED(1.0);
      VALUE(m_histogram->GetXaxis()->GetXmax()) EXPECTED(5.0);
      VALUE(m_histogram->GetYaxis()->GetNbins()) EXPECTED(10);
      VALUE(m_histogram->GetYaxis()->GetXmin()) EXPECTED(1.0);
      VALUE(m_histogram->GetYaxis()->GetXmax()) EXPECTED(9.0);
      VALUE(m_histogram->GetBinContent(9,6)) EXPECTED(1.0);
    }


   // ==================== Initialization & run ====================
  public:
    HistogramFillerRebinable2DTestSuite()
      : m_log(Athena::getMessageSvc(), "HistogramFillerRebinable2DTestSuite") {
    }

    void run() {
      for (function<void(void)> testCase : registeredTestCases()) {
        testCase();
      }
    }

  // ==================== Test case registration ====================
  private:
    typedef void (HistogramFillerRebinable2DTestSuite::*TestCase)(void);

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

    HistogramDef m_histogramDef;
    shared_ptr<MockHistogramProvider> m_histogramProvider;
    shared_ptr<MockMonitoredVariable> m_var1, m_var2;
    shared_ptr<TH2D> m_histogram;
    
    shared_ptr<HistogramFillerRebinable2D> m_testObj;
};

int main() {
  ISvcLocator* pSvcLoc;

  if (!Athena_test::initGaudi("GenericMon.txt", pSvcLoc)) {
    throw runtime_error("This test can not be run: GenericMon.txt is missing");
  }

  HistogramFillerRebinable2DTestSuite().run();

  return 0;
}
