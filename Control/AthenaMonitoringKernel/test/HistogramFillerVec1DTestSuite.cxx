/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#include <list>
#include <functional>
#include <memory>

#include "TestTools/initGaudi.h"
#include "TestTools/expect.h"
#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/ITHistSvc.h"
#include "AthenaKernel/getMessageSvc.h"

#include "TH1.h"

#include "AthenaMonitoringKernel/MonitoredCollection.h"
#include "../src/HistogramFiller/VecHistogramFiller1D.h"

#include "mocks/MockHistogramDef.h"
#include "mocks/MockHistogramProvider.h"

using namespace std;
using namespace Monitored;

#define REGISTER_TEST_CASE(TEST_CASE_NAME, KVECUO) registerTestCase(&HistogramFillerVec1DTestSuite::TEST_CASE_NAME, #TEST_CASE_NAME, KVECUO)

class HistogramFillerVec1DTestSuite {
  // ==================== All registered test cases ====================
  private:
    list<function<void(void)>> registeredTestCases() {
      return {
        REGISTER_TEST_CASE(test_fillWithVector, false),
        REGISTER_TEST_CASE(test_fillWithShortVector, false),
        REGISTER_TEST_CASE(test_fillWithVectorUO, true),
        REGISTER_TEST_CASE(test_fillWithShortVectorUO, true)
      };
    }

  // ==================== Test code ====================
  private:
    void beforeEach(bool kVecUO) {
        if (kVecUO) {
            m_histogramDef.kVecUO = true;
        } else {
            m_histogramDef.kVec = true;
        }
        m_histogramProvider.reset(new MockHistogramProvider());
        m_histogram.reset(new TH1D("MockHistogram", "Mock Histogram", 5, 0.0, 5.0));
        m_testObj.reset(new VecHistogramFiller1D(m_histogramDef, m_histogramProvider));

        m_histogramProvider->mock_histogram = [this]() { return m_histogram.get(); };
    }

    void afterEach() {
    }

    void test_fillWithVector() {

      using Coll = vector<double>;
      Coll values({1., 2., 3., 4., 5.});
      auto var = Monitored::Collection("values", values);

      HistogramFiller::VariablesPack vars({&var});

      VALUE(m_histogram->GetXaxis()->GetNbins()) EXPECTED(5);
      VALUE(m_histogram->GetXaxis()->GetXmin()) EXPECTED(0.0);
      VALUE(m_histogram->GetXaxis()->GetXmax()) EXPECTED(5.0);

      m_testObj->fill(vars);

      for (unsigned i: {0, 6}) {
          VALUE(m_histogram->GetBinContent(i)) EXPECTED(0.0);
      }
      for (unsigned i = 0; i != values.size(); ++ i) {
        VALUE(m_histogram->GetBinContent(i+1)) EXPECTED(values[i]);
      }
    }

    void test_fillWithShortVector() {

      using Coll = vector<double>;
      Coll values({1., 2., 3.});
      auto var = Monitored::Collection("values", values);

      HistogramFiller::VariablesPack vars({&var});

      VALUE(m_histogram->GetXaxis()->GetNbins()) EXPECTED(5);
      VALUE(m_histogram->GetXaxis()->GetXmin()) EXPECTED(0.0);
      VALUE(m_histogram->GetXaxis()->GetXmax()) EXPECTED(5.0);

      m_testObj->fill(vars);

      for (unsigned i: {0, 4, 5, 6}) {
          VALUE(m_histogram->GetBinContent(i)) EXPECTED(0.0);
      }
      for (unsigned i = 0; i != values.size(); ++ i) {
        VALUE(m_histogram->GetBinContent(i+1)) EXPECTED(values[i]);
      }
    }

    void test_fillWithVectorUO() {

      using Coll = vector<double>;
      Coll values({1., 2., 3., 4., 5., 6., 7.});
      auto var = Monitored::Collection("values", values);

      HistogramFiller::VariablesPack vars({&var});

      VALUE(m_histogram->GetXaxis()->GetNbins()) EXPECTED(5);
      VALUE(m_histogram->GetXaxis()->GetXmin()) EXPECTED(0.0);
      VALUE(m_histogram->GetXaxis()->GetXmax()) EXPECTED(5.0);

      m_testObj->fill(vars);

      for (unsigned i = 0; i != values.size(); ++ i) {
        VALUE(m_histogram->GetBinContent(i)) EXPECTED(values[i]);
      }
    }

    void test_fillWithShortVectorUO() {

      using Coll = vector<double>;
      Coll values({1., 2., 3., 4., 5.});
      auto var = Monitored::Collection("values", values);

      HistogramFiller::VariablesPack vars({&var});

      VALUE(m_histogram->GetXaxis()->GetNbins()) EXPECTED(5);
      VALUE(m_histogram->GetXaxis()->GetXmin()) EXPECTED(0.0);
      VALUE(m_histogram->GetXaxis()->GetXmax()) EXPECTED(5.0);

      m_testObj->fill(vars);

      for (unsigned i: {5, 6}) {
          VALUE(m_histogram->GetBinContent(i)) EXPECTED(0.0);
      }
      for (unsigned i = 0; i != values.size(); ++ i) {
        VALUE(m_histogram->GetBinContent(i)) EXPECTED(values[i]);
      }
    }

  // ==================== Helper methods ====================
  private:

  // ==================== Initialization & run ====================
  public:
    HistogramFillerVec1DTestSuite()
      : m_log(Athena::getMessageSvc(), "HistogramFillerVec1DTestSuite") {
    }

    void run() {
      for (function<void(void)> testCase : registeredTestCases()) {
        testCase();
      }
    }

  // ==================== Test case registration ====================
  private:
    typedef void (HistogramFillerVec1DTestSuite::*TestCase)(void);

    function<void(void)> registerTestCase(TestCase testCase, const string& testCaseName, bool kVecUO) {
      return [this, testCase, testCaseName, kVecUO]() {
        m_log << MSG::INFO << "Current test case: " << testCaseName << endmsg;
        beforeEach(kVecUO);
        invoke(testCase, this);
        afterEach();
      };
    }

  // ==================== Properties ====================
  private:
    MsgStream m_log;

    MockHistogramDef m_histogramDef;
    shared_ptr<MockHistogramProvider> m_histogramProvider;
    shared_ptr<TH1D> m_histogram;

    shared_ptr<VecHistogramFiller1D> m_testObj;
};

int main() {
  ISvcLocator* pSvcLoc;

  if (!Athena_test::initGaudi("GenericMon.txt", pSvcLoc)) {
    throw runtime_error("This test can not be run: GenericMon.txt is missing");
  }

  HistogramFillerVec1DTestSuite().run();

  return 0;
}
