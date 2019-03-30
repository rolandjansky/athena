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
#include "AthenaMonitoring/HistogramFiller/HistogramFactory.h"

using namespace std;
using namespace Monitored;

#define REGISTER_TEST_CASE(TEST_CASE_NAME) registerTestCase(&HistogramFactory::TEST_CASE_NAME, #TEST_CASE_NAME)

namespace test {
class HistogramFactory {
  // ==================== All test cases ====================
  private:
    list<function<void(void)>> allTestCases() {
      return {
        REGISTER_TEST_CASE(test_shouldReturnTH1FHistogram),
        REGISTER_TEST_CASE(test_shouldReturnTH1DHistogram),
        REGISTER_TEST_CASE(test_shouldReturnTH1IHistogram),
        REGISTER_TEST_CASE(test_shouldReturnTH2FHistogram),
        REGISTER_TEST_CASE(test_shouldReturnTH2DHistogram),
        REGISTER_TEST_CASE(test_shouldReturnTH2IHistogram),
        REGISTER_TEST_CASE(test_shouldReturnTProfileHistogram),
        REGISTER_TEST_CASE(test_shouldReturnTProfile2DHistogram),
        REGISTER_TEST_CASE(test_shouldReturnTEfficiencyHistogram),
        REGISTER_TEST_CASE(test_shouldThrowExceptionForUnknownHistogramType),
      };
    }

  // ==================== Test code ====================
  private:
    void beforeEach() {
      m_testObj.reset(new Monitored::HistogramFactory(m_histSvc, "HistogramFactory"));
    }

    void afterEach() {
      clearHistogramService();
    }

    void test_shouldReturnTH1FHistogram() {
      TH1F* const histogram = createHistogram<TH1F>("TH1F");
      VALUE(histogram) NOT_EXPECTED((TH1F*)nullptr);
    }

    void test_shouldReturnTH1DHistogram() {
      TH1D* const histogram = createHistogram<TH1D>("TH1D");
      VALUE(histogram) NOT_EXPECTED((TH1D*)nullptr);
    }

    void test_shouldReturnTH1IHistogram() {
      TH1I* const histogram = createHistogram<TH1I>("TH1I");
      VALUE(histogram) NOT_EXPECTED((TH1I*)nullptr);
    }

    void test_shouldReturnTH2FHistogram() {
      TH2F* const histogram = createHistogram<TH2F>("TH2F");
      VALUE(histogram) NOT_EXPECTED((TH2F*)nullptr);
    }

    void test_shouldReturnTH2DHistogram() {
      TH2D* const histogram = createHistogram<TH2D>("TH2D");
      VALUE(histogram) NOT_EXPECTED((TH2D*)nullptr);
    }

    void test_shouldReturnTH2IHistogram() {
      TH2I* const histogram = createHistogram<TH2I>("TH2I");
      VALUE(histogram) NOT_EXPECTED((TH2I*)nullptr);
    }

    void test_shouldReturnTProfileHistogram() {
      TProfile* const histogram = createHistogram<TProfile>("TProfile");
      VALUE(histogram) NOT_EXPECTED((TProfile*)nullptr);
    }

    void test_shouldReturnTProfile2DHistogram() {
      TProfile2D* const histogram = createHistogram<TProfile2D>("TProfile2D");
      VALUE(histogram) NOT_EXPECTED((TProfile2D*)nullptr);
    }

    void test_shouldReturnTEfficiencyHistogram() {
      TEfficiency* const histogram = createHistogram<TEfficiency>("TEfficiency");
      VALUE(histogram) NOT_EXPECTED((TEfficiency*)nullptr);
    }

    void test_shouldThrowExceptionForUnknownHistogramType() {
      try {
        createHistogram<TNamed>("UnknownType");
      } catch (const HistogramException& e) {
        return;
      }
      
      assert(false);
    }

  // ==================== Helper methods ====================
  private:
    TNamed* createHistogram(const string& histogramType) {
      HistogramDef histogramDef;
      histogramDef.type = histogramType;
      histogramDef.alias = histogramType;
      histogramDef.title = histogramType;
      histogramDef.xbins = 1;
      histogramDef.ybins = 1;

      return m_testObj->create(histogramDef);
    }

    template <class HistogramType> 
    HistogramType* createHistogram(const string& histogramType) {
      return dynamic_cast<HistogramType*>(createHistogram(histogramType));
    }

    void clearHistogramService() {
      for (string histName : m_histSvc->getHists()) {
        m_histSvc->deReg(histName);
      }

      for (string treeName : m_histSvc->getTrees()) {
        m_histSvc->deReg(treeName);
      }

      for (string graphName : m_histSvc->getGraphs()) {
        m_histSvc->deReg(graphName);
      }
    }

  // ==================== Initialization & run ====================
  public:
    HistogramFactory() 
      : m_histSvc("THistSvc", "TestGroup"), 
        m_log(Athena::getMessageSvc(), "HistogramFactory") {
      assert(m_histSvc.retrieve() == StatusCode::SUCCESS);
    }

    void runTests() {
      for (function<void(void)> testCase : allTestCases()) {
        testCase();
      }
    }

  // ==================== Test case registration ====================
  private:
    typedef void (HistogramFactory::*TestCase)(void);

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
    ServiceHandle<ITHistSvc> m_histSvc;
    MsgStream m_log;

    shared_ptr<Monitored::HistogramFactory> m_testObj;
};
}

int main() {
  ISvcLocator* pSvcLoc;

  if (!Athena_test::initGaudi("GenericMon.txt", pSvcLoc)) {
    throw runtime_error("This test can not be run: GenericMon.txt is missing");
  }

  test::HistogramFactory().runTests();

  return 0;
}
