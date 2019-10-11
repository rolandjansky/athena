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

#include "../src/HistogramFiller/HistogramFillerRebinable1D.h"

#include "mocks/MockHistogramProvider.h"
#include "mocks/MockMonitoredVariable.h"

using namespace std;
using namespace Monitored;

#define REGISTER_TEST_CASE(TEST_CASE_NAME) registerTestCase(&HistogramFillerRebinable1DTestSuite::TEST_CASE_NAME, #TEST_CASE_NAME)

class HistogramFillerRebinable1DTestSuite {
  // ==================== All registered test cases ====================
  private:
    list<function<void(void)>> registeredTestCases() {
      return {
        REGISTER_TEST_CASE(test_shouldKeepNumberOfBeansForValueInHistogramsRange),
        REGISTER_TEST_CASE(test_shouldDoubleNumberOfBeansForBoundaryValueOf3),

        REGISTER_TEST_CASE(test_shouldDoubleNumberOfBeansForValueSlightlySmallerThan5),
        REGISTER_TEST_CASE(test_shouldQuadrupleNumberOfBeansForBoundaryValueOf5),
        REGISTER_TEST_CASE(test_shouldQuadrupleNumberOfBeansForValueSlightlyBiggerThan5),

        REGISTER_TEST_CASE(test_shouldQuadrupleNumberOfBeansForValueSlightlySmallerThan9),
        REGISTER_TEST_CASE(test_shouldOctupleNumberOfBeansForBoundaryValueOf9),
        REGISTER_TEST_CASE(test_shouldOctupleNumberOfBeansForValueSlightlyBiggerThan9),
      };
    }

  // ==================== Test code ====================
  private:
    void beforeEach() {
        m_histogramDef.opt = "kAddBinsDynamically";
        m_histogramProvider.reset(new MockHistogramProvider());
        m_monitoredVariable.reset(new MockMonitoredVariable(""));
        m_histogram.reset(new TH1D("MockHistogram", "Mock Histogram", 8, 1.0, 3.0));
        m_testObj.reset(new HistogramFillerRebinable1D(m_histogramDef, m_histogramProvider));

        m_testObj->setMonitoredVariables({ *m_monitoredVariable });
        m_histogramProvider->mock_histogram = [this]() { return m_histogram.get(); };
    }

    void afterEach() {
    }

    void test_shouldKeepNumberOfBeansForValueInHistogramsRange() {
      m_monitoredVariable->mock_getVectorRepresentation = []() -> vector<double> {
        return { 2.9 };
      };

      VALUE(m_histogram->GetXaxis()->GetNbins()) EXPECTED(8);
      VALUE(m_histogram->GetXaxis()->GetXmin()) EXPECTED(1.0);
      VALUE(m_histogram->GetXaxis()->GetXmax()) EXPECTED(3.0);

      m_testObj->fill();

      VALUE(m_histogram->GetXaxis()->GetNbins()) EXPECTED(8);
      VALUE(m_histogram->GetXaxis()->GetXmin()) EXPECTED(1.0);
      VALUE(m_histogram->GetXaxis()->GetXmax()) EXPECTED(3.0);
      VALUE(m_histogram->GetBinContent(8)) EXPECTED(1.0);
    }

    void test_shouldDoubleNumberOfBeansForBoundaryValueOf3() {
      m_monitoredVariable->mock_getVectorRepresentation = []() -> vector<double> {
        return { 3.0 };
      };

      VALUE(m_histogram->GetXaxis()->GetNbins()) EXPECTED(8);
      VALUE(m_histogram->GetXaxis()->GetXmin()) EXPECTED(1.0);
      VALUE(m_histogram->GetXaxis()->GetXmax()) EXPECTED(3.0);

      m_testObj->fill();

      VALUE(m_histogram->GetXaxis()->GetNbins()) EXPECTED(16);
      VALUE(m_histogram->GetXaxis()->GetXmin()) EXPECTED(1.0);
      VALUE(m_histogram->GetXaxis()->GetXmax()) EXPECTED(5.0);
      VALUE(m_histogram->GetBinContent(9)) EXPECTED(1.0);
    }

    void test_shouldDoubleNumberOfBeansForValueSlightlySmallerThan5() {
      m_monitoredVariable->mock_getVectorRepresentation = []() -> vector<double> {
        return { 4.9 };
      };

      VALUE(m_histogram->GetXaxis()->GetNbins()) EXPECTED(8);
      VALUE(m_histogram->GetXaxis()->GetXmin()) EXPECTED(1.0);
      VALUE(m_histogram->GetXaxis()->GetXmax()) EXPECTED(3.0);

      m_testObj->fill();

      VALUE(m_histogram->GetXaxis()->GetNbins()) EXPECTED(16);
      VALUE(m_histogram->GetXaxis()->GetXmin()) EXPECTED(1.0);
      VALUE(m_histogram->GetXaxis()->GetXmax()) EXPECTED(5.0);
      VALUE(m_histogram->GetBinContent(16)) EXPECTED(1.0);
    }

    void test_shouldQuadrupleNumberOfBeansForBoundaryValueOf5() {
      m_monitoredVariable->mock_getVectorRepresentation = []() -> vector<double> {
        return { 5.0 };
      };

      VALUE(m_histogram->GetXaxis()->GetNbins()) EXPECTED(8);
      VALUE(m_histogram->GetXaxis()->GetXmin()) EXPECTED(1.0);
      VALUE(m_histogram->GetXaxis()->GetXmax()) EXPECTED(3.0);

      m_testObj->fill();

      VALUE(m_histogram->GetXaxis()->GetNbins()) EXPECTED(32);
      VALUE(m_histogram->GetXaxis()->GetXmin()) EXPECTED(1.0);
      VALUE(m_histogram->GetXaxis()->GetXmax()) EXPECTED(9.0);
      VALUE(m_histogram->GetBinContent(17)) EXPECTED(1.0);
    }

    void test_shouldQuadrupleNumberOfBeansForValueSlightlyBiggerThan5() {
      m_monitoredVariable->mock_getVectorRepresentation = []() -> vector<double> {
        return { 5.1 };
      };

      VALUE(m_histogram->GetXaxis()->GetNbins()) EXPECTED(8);
      VALUE(m_histogram->GetXaxis()->GetXmin()) EXPECTED(1.0);
      VALUE(m_histogram->GetXaxis()->GetXmax()) EXPECTED(3.0);

      m_testObj->fill();

      VALUE(m_histogram->GetXaxis()->GetNbins()) EXPECTED(32);
      VALUE(m_histogram->GetXaxis()->GetXmin()) EXPECTED(1.0);
      VALUE(m_histogram->GetXaxis()->GetXmax()) EXPECTED(9.0);
      VALUE(m_histogram->GetBinContent(17)) EXPECTED(1.0);
    }

    void test_shouldQuadrupleNumberOfBeansForValueSlightlySmallerThan9() {
      m_monitoredVariable->mock_getVectorRepresentation = []() -> vector<double> {
        return { 8.9 };
      };

      VALUE(m_histogram->GetXaxis()->GetNbins()) EXPECTED(8);
      VALUE(m_histogram->GetXaxis()->GetXmin()) EXPECTED(1.0);
      VALUE(m_histogram->GetXaxis()->GetXmax()) EXPECTED(3.0);

      m_testObj->fill();

      VALUE(m_histogram->GetXaxis()->GetNbins()) EXPECTED(32);
      VALUE(m_histogram->GetXaxis()->GetXmin()) EXPECTED(1.0);
      VALUE(m_histogram->GetXaxis()->GetXmax()) EXPECTED(9.0);
      VALUE(m_histogram->GetBinContent(32)) EXPECTED(1.0);
    }

    void test_shouldOctupleNumberOfBeansForBoundaryValueOf9() {
      m_monitoredVariable->mock_getVectorRepresentation = []() -> vector<double> {
        return { 9.0 };
      };

      VALUE(m_histogram->GetXaxis()->GetNbins()) EXPECTED(8);
      VALUE(m_histogram->GetXaxis()->GetXmin()) EXPECTED(1.0);
      VALUE(m_histogram->GetXaxis()->GetXmax()) EXPECTED(3.0);

      m_testObj->fill();

      VALUE(m_histogram->GetXaxis()->GetNbins()) EXPECTED(64);
      VALUE(m_histogram->GetXaxis()->GetXmin()) EXPECTED(1.0);
      VALUE(m_histogram->GetXaxis()->GetXmax()) EXPECTED(17.0);
      VALUE(m_histogram->GetBinContent(33)) EXPECTED(1.0);
    }

    void test_shouldOctupleNumberOfBeansForValueSlightlyBiggerThan9() {
      m_monitoredVariable->mock_getVectorRepresentation = []() -> vector<double> {
        return { 9.1 };
      };

      VALUE(m_histogram->GetXaxis()->GetNbins()) EXPECTED(8);
      VALUE(m_histogram->GetXaxis()->GetXmin()) EXPECTED(1.0);
      VALUE(m_histogram->GetXaxis()->GetXmax()) EXPECTED(3.0);

      m_testObj->fill();

      VALUE(m_histogram->GetXaxis()->GetNbins()) EXPECTED(64);
      VALUE(m_histogram->GetXaxis()->GetXmin()) EXPECTED(1.0);
      VALUE(m_histogram->GetXaxis()->GetXmax()) EXPECTED(17.0);
      VALUE(m_histogram->GetBinContent(33)) EXPECTED(1.0);
    }

  // ==================== Helper methods ====================
  private:

  // ==================== Initialization & run ====================
  public:
    HistogramFillerRebinable1DTestSuite() 
      : m_log(Athena::getMessageSvc(), "HistogramFillerRebinable1DTestSuite") {
    }

    void run() {
      for (function<void(void)> testCase : registeredTestCases()) {
        testCase();
      }
    }

  // ==================== Test case registration ====================
  private:
    typedef void (HistogramFillerRebinable1DTestSuite::*TestCase)(void);

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
    shared_ptr<MockMonitoredVariable> m_monitoredVariable;
    shared_ptr<TH1D> m_histogram;
    
    shared_ptr<HistogramFillerRebinable1D> m_testObj;
};

int main() {
  ISvcLocator* pSvcLoc;

  if (!Athena_test::initGaudi("GenericMon.txt", pSvcLoc)) {
    throw runtime_error("This test can not be run: GenericMon.txt is missing");
  }

  HistogramFillerRebinable1DTestSuite().run();

  return 0;
}
