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

#define REGISTER_TEST_CASE(TEST_CASE_NAME) registerTestCase(&HistogramFactoryTestSuite::TEST_CASE_NAME, #TEST_CASE_NAME)

class HistogramFactoryTestSuite {
  // ==================== All registered test cases ====================
  private:
    list<function<void(void)>> registeredTestCases() {
      return {
        REGISTER_TEST_CASE(test_shouldRegisterAndReturnTH1FHistogram),
        REGISTER_TEST_CASE(test_shouldRegisterAndReturnTH1DHistogram),
        REGISTER_TEST_CASE(test_shouldRegisterAndReturnTH1IHistogram),
        REGISTER_TEST_CASE(test_shouldRegisterAndReturnTH2FHistogram),
        REGISTER_TEST_CASE(test_shouldRegisterAndReturnTH2DHistogram),
        REGISTER_TEST_CASE(test_shouldRegisterAndReturnTH2IHistogram),
        REGISTER_TEST_CASE(test_shouldRegisterAndReturnTProfileHistogram),
        REGISTER_TEST_CASE(test_shouldRegisterAndReturnTProfile2DHistogram),
        REGISTER_TEST_CASE(test_shouldRegisterAndReturnTEfficiencyHistogram),
        REGISTER_TEST_CASE(test_shouldThrowExceptionForUnknownHistogramType),
      };
    }

  // ==================== Test code ====================
  private:
    void beforeEach() {
      m_testObj.reset(new HistogramFactory(m_histSvc, "HistogramFactoryTestSuite"));
    }

    void afterEach() {
      clearHistogramService();
    }

    void test_shouldRegisterAndReturnTH1FHistogram() {
      TH1F* const histogram = createHistogram<TH1F>("TH1F");
      VALUE(m_histSvc->exists("/HistogramFactoryTestSuite/TH1F")) EXPECTED(true);
      VALUE(histogram) NOT_EXPECTED((TH1F*)nullptr);
    }

    void test_shouldRegisterAndReturnTH1DHistogram() {
      TH1D* const histogram = createHistogram<TH1D>("TH1D");
      VALUE(m_histSvc->exists("/HistogramFactoryTestSuite/TH1D")) EXPECTED(true);
      VALUE(histogram) NOT_EXPECTED((TH1D*)nullptr);
    }

    void test_shouldRegisterAndReturnTH1IHistogram() {
      TH1I* const histogram = createHistogram<TH1I>("TH1I");
      VALUE(m_histSvc->exists("/HistogramFactoryTestSuite/TH1I")) EXPECTED(true);
      VALUE(histogram) NOT_EXPECTED((TH1I*)nullptr);
    }

    void test_shouldRegisterAndReturnTH2FHistogram() {
      TH2F* const histogram = createHistogram<TH2F>("TH2F");
      VALUE(m_histSvc->exists("/HistogramFactoryTestSuite/TH2F")) EXPECTED(true);
      VALUE(histogram) NOT_EXPECTED((TH2F*)nullptr);
    }

    void test_shouldRegisterAndReturnTH2DHistogram() {
      TH2D* const histogram = createHistogram<TH2D>("TH2D");
      VALUE(m_histSvc->exists("/HistogramFactoryTestSuite/TH2D")) EXPECTED(true);
      VALUE(histogram) NOT_EXPECTED((TH2D*)nullptr);
    }

    void test_shouldRegisterAndReturnTH2IHistogram() {
      TH2I* const histogram = createHistogram<TH2I>("TH2I");
      VALUE(m_histSvc->exists("/HistogramFactoryTestSuite/TH2I")) EXPECTED(true);
      VALUE(histogram) NOT_EXPECTED((TH2I*)nullptr);
    }

    void test_shouldRegisterAndReturnTProfileHistogram() {
      TProfile* const histogram = createHistogram<TProfile>("TProfile");
      VALUE(m_histSvc->exists("/HistogramFactoryTestSuite/TProfile")) EXPECTED(true);
      VALUE(histogram) NOT_EXPECTED((TProfile*)nullptr);
    }

    void test_shouldRegisterAndReturnTProfile2DHistogram() {
      TProfile2D* const histogram = createHistogram<TProfile2D>("TProfile2D");
      VALUE(m_histSvc->exists("/HistogramFactoryTestSuite/TProfile2D")) EXPECTED(true);
      VALUE(histogram) NOT_EXPECTED((TProfile2D*)nullptr);
    }

    void test_shouldRegisterAndReturnTEfficiencyHistogram() {
      TEfficiency* const graph = createHistogram<TEfficiency>("TEfficiency");
      // VALUE(m_histSvc->exists("/HistogramFactoryTestSuite/TEfficiency")) EXPECTED(true);
      VALUE(graph) NOT_EXPECTED((TEfficiency*)nullptr);
    }

    void test_shouldThrowExceptionForUnknownHistogramType() {
      try {
        createHistogram("UnknownType");
      } catch (const HistogramException&) {
        VALUE(m_histSvc->exists("/HistogramFactoryTestSuite/UnknownType")) EXPECTED(false);
        return;
      }
      
      assert(false);
    }

  // ==================== Helper methods ====================
  private:
    TNamed* createHistogram(const string& histogramType) {
      HistogramDef histogramDef;

      histogramDef.path = "DEFAULT";
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
    HistogramFactoryTestSuite() 
      : m_histSvc("THistSvc", "HistogramFactoryTestSuite"), 
        m_log(Athena::getMessageSvc(), "HistogramFactoryTestSuite") {
      assert(m_histSvc.retrieve() == StatusCode::SUCCESS);
    }

    void run() {
      for (function<void(void)> testCase : registeredTestCases()) {
        testCase();
      }
    }

  // ==================== Test case registration ====================
  private:
    typedef void (HistogramFactoryTestSuite::*TestCase)(void);

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

    shared_ptr<HistogramFactory> m_testObj;
};

int main() {
  ISvcLocator* pSvcLoc;

  if (!Athena_test::initGaudi("GenericMon.txt", pSvcLoc)) {
    throw runtime_error("This test can not be run: GenericMon.txt is missing");
  }

  HistogramFactoryTestSuite().run();

  return 0;
}
