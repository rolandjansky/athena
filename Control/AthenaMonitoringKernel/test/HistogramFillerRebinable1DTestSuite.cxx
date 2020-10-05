/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
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

#include "AthenaMonitoringKernel/MonitoredScalar.h"
#include "../src/HistogramFiller/HistogramFillerRebinable.h"

#include "mocks/MockHistogramProvider.h"

using namespace std;
using namespace Monitored;

#define REGISTER_TEST_CASE(TEST_CASE_NAME) registerTestCase(&HistogramFillerRebinable1DTestSuite::TEST_CASE_NAME, #TEST_CASE_NAME)

class HistogramFillerRebinable1DTestSuite {
  // ==================== All registered test cases ====================
  private:
    list<function<void(void)>> registeredTestCases() {
      return {
        REGISTER_TEST_CASE(test_shouldKeepNumberOfBinsForValueInHistogramsRange),
        REGISTER_TEST_CASE(test_shouldDoubleNumberOfBinsForBoundaryValueOf3),

        REGISTER_TEST_CASE(test_shouldDoubleNumberOfBinsForValueSlightlySmallerThan5),
        REGISTER_TEST_CASE(test_shouldQuadrupleNumberOfBinsForBoundaryValueOf5),
        REGISTER_TEST_CASE(test_shouldQuadrupleNumberOfBinsForValueSlightlyBiggerThan5),

        REGISTER_TEST_CASE(test_shouldQuadrupleNumberOfBinsForValueSlightlySmallerThan9),
        REGISTER_TEST_CASE(test_shouldOctupleNumberOfBinsForBoundaryValueOf9),
        REGISTER_TEST_CASE(test_shouldOctupleNumberOfBinsForValueSlightlyBiggerThan9),
      };
    }

  // ==================== Test code ====================
  private:
    void beforeEach() {
        m_histogramDef.kAddBinsDynamically = true;
        m_histogramProvider.reset(new MockHistogramProvider());
        m_histogram.reset(new TH1D("MockHistogram", "Mock Histogram", 8, 1.0, 3.0));
        m_testObj.reset(new HistogramFillerRebinable1D(m_histogramDef, m_histogramProvider));

        m_histogramProvider->mock_histogram = [this]() { return m_histogram.get(); };
    }

    void afterEach() {
    }

    void test_shouldKeepNumberOfBinsForValueInHistogramsRange() {
      Monitored::Scalar<double> var("var", 2.9);
      HistogramFiller::VariablesPack vars({&var});


      VALUE(m_histogram->GetXaxis()->GetNbins()) EXPECTED(8);
      VALUE(m_histogram->GetXaxis()->GetXmin()) EXPECTED(1.0);
      VALUE(m_histogram->GetXaxis()->GetXmax()) EXPECTED(3.0);

      m_testObj->fill(vars);

      VALUE(m_histogram->GetXaxis()->GetNbins()) EXPECTED(8);
      VALUE(m_histogram->GetXaxis()->GetXmin()) EXPECTED(1.0);
      VALUE(m_histogram->GetXaxis()->GetXmax()) EXPECTED(3.0);
      VALUE(m_histogram->GetBinContent(8)) EXPECTED(1.0);
    }

    void test_shouldDoubleNumberOfBinsForBoundaryValueOf3() {
      Monitored::Scalar<double> var("var", 3.0);
      HistogramFiller::VariablesPack vars({&var});

      VALUE(m_histogram->GetXaxis()->GetNbins()) EXPECTED(8);
      VALUE(m_histogram->GetXaxis()->GetXmin()) EXPECTED(1.0);
      VALUE(m_histogram->GetXaxis()->GetXmax()) EXPECTED(3.0);

      m_testObj->fill(vars);

      VALUE(m_histogram->GetXaxis()->GetNbins()) EXPECTED(16);
      VALUE(m_histogram->GetXaxis()->GetXmin()) EXPECTED(1.0);
      VALUE(m_histogram->GetXaxis()->GetXmax()) EXPECTED(5.0);
      VALUE(m_histogram->GetBinContent(9)) EXPECTED(1.0);
    }

    void test_shouldDoubleNumberOfBinsForValueSlightlySmallerThan5() {
      Monitored::Scalar<double> var("var", 4.9);
      HistogramFiller::VariablesPack vars({&var});

      VALUE(m_histogram->GetXaxis()->GetNbins()) EXPECTED(8);
      VALUE(m_histogram->GetXaxis()->GetXmin()) EXPECTED(1.0);
      VALUE(m_histogram->GetXaxis()->GetXmax()) EXPECTED(3.0);

      m_testObj->fill(vars);

      VALUE(m_histogram->GetXaxis()->GetNbins()) EXPECTED(16);
      VALUE(m_histogram->GetXaxis()->GetXmin()) EXPECTED(1.0);
      VALUE(m_histogram->GetXaxis()->GetXmax()) EXPECTED(5.0);
      VALUE(m_histogram->GetBinContent(16)) EXPECTED(1.0);
    }

    void test_shouldQuadrupleNumberOfBinsForBoundaryValueOf5() {
      Monitored::Scalar<double> var("var", 5.0);
      HistogramFiller::VariablesPack vars({&var});

      VALUE(m_histogram->GetXaxis()->GetNbins()) EXPECTED(8);
      VALUE(m_histogram->GetXaxis()->GetXmin()) EXPECTED(1.0);
      VALUE(m_histogram->GetXaxis()->GetXmax()) EXPECTED(3.0);

      m_testObj->fill(vars);

      VALUE(m_histogram->GetXaxis()->GetNbins()) EXPECTED(32);
      VALUE(m_histogram->GetXaxis()->GetXmin()) EXPECTED(1.0);
      VALUE(m_histogram->GetXaxis()->GetXmax()) EXPECTED(9.0);
      VALUE(m_histogram->GetBinContent(17)) EXPECTED(1.0);
    }

    void test_shouldQuadrupleNumberOfBinsForValueSlightlyBiggerThan5() {
      Monitored::Scalar<double> var("var", 5.1);
      HistogramFiller::VariablesPack vars({&var});

      VALUE(m_histogram->GetXaxis()->GetNbins()) EXPECTED(8);
      VALUE(m_histogram->GetXaxis()->GetXmin()) EXPECTED(1.0);
      VALUE(m_histogram->GetXaxis()->GetXmax()) EXPECTED(3.0);

      m_testObj->fill(vars);

      VALUE(m_histogram->GetXaxis()->GetNbins()) EXPECTED(32);
      VALUE(m_histogram->GetXaxis()->GetXmin()) EXPECTED(1.0);
      VALUE(m_histogram->GetXaxis()->GetXmax()) EXPECTED(9.0);
      VALUE(m_histogram->GetBinContent(17)) EXPECTED(1.0);
    }

    void test_shouldQuadrupleNumberOfBinsForValueSlightlySmallerThan9() {
      Monitored::Scalar<double> var("var", 8.9);
      HistogramFiller::VariablesPack vars({&var});

      VALUE(m_histogram->GetXaxis()->GetNbins()) EXPECTED(8);
      VALUE(m_histogram->GetXaxis()->GetXmin()) EXPECTED(1.0);
      VALUE(m_histogram->GetXaxis()->GetXmax()) EXPECTED(3.0);

      m_testObj->fill(vars);

      VALUE(m_histogram->GetXaxis()->GetNbins()) EXPECTED(32);
      VALUE(m_histogram->GetXaxis()->GetXmin()) EXPECTED(1.0);
      VALUE(m_histogram->GetXaxis()->GetXmax()) EXPECTED(9.0);
      VALUE(m_histogram->GetBinContent(32)) EXPECTED(1.0);
    }

    void test_shouldOctupleNumberOfBinsForBoundaryValueOf9() {
      Monitored::Scalar<double> var("var", 9.0);
      HistogramFiller::VariablesPack vars({&var});

      VALUE(m_histogram->GetXaxis()->GetNbins()) EXPECTED(8);
      VALUE(m_histogram->GetXaxis()->GetXmin()) EXPECTED(1.0);
      VALUE(m_histogram->GetXaxis()->GetXmax()) EXPECTED(3.0);

      m_testObj->fill(vars);

      VALUE(m_histogram->GetXaxis()->GetNbins()) EXPECTED(64);
      VALUE(m_histogram->GetXaxis()->GetXmin()) EXPECTED(1.0);
      VALUE(m_histogram->GetXaxis()->GetXmax()) EXPECTED(17.0);
      VALUE(m_histogram->GetBinContent(33)) EXPECTED(1.0);
    }

    void test_shouldOctupleNumberOfBinsForValueSlightlyBiggerThan9() {
      Monitored::Scalar<double> var("var", 9.1);
      HistogramFiller::VariablesPack vars({&var});

      VALUE(m_histogram->GetXaxis()->GetNbins()) EXPECTED(8);
      VALUE(m_histogram->GetXaxis()->GetXmin()) EXPECTED(1.0);
      VALUE(m_histogram->GetXaxis()->GetXmax()) EXPECTED(3.0);

      m_testObj->fill(vars);

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
