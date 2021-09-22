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
#include "CxxUtils/ubsan_suppress.h"

#include "TH1.h"
#include "TH2.h"
#include "TProfile.h"
#include "TProfile2D.h"
#include "TInterpreter.h"

#include "AthenaMonitoringKernel/HistogramDef.h"

#include "../src/HistogramFiller/HistogramFactory.h"

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
        REGISTER_TEST_CASE(test_shouldProperlyFormatPathForOnlineHistograms),
        REGISTER_TEST_CASE(test_shouldProperlyFormatPathForDefaultHistograms),
        REGISTER_TEST_CASE(test_shouldProperlyFormatPathForCustomHistograms),
        REGISTER_TEST_CASE(test_shouldProperlyFormatPathForOfflineHistograms),
        REGISTER_TEST_CASE(test_shouldProperlyFormatPathForTempOfflineHistograms),
        REGISTER_TEST_CASE(test_shouldSetXAxisLabelsFor1DHistogram),
        REGISTER_TEST_CASE(test_shouldSetXAndYAxisLabelsFor2DHistogram),
        REGISTER_TEST_CASE(test_shouldSetExtendAxesWhenkCanRebinIsSet),
        REGISTER_TEST_CASE(test_shouldNotSetExtendAxesWhenkCanRebinIsNotSet),
        REGISTER_TEST_CASE(test_shouldSetSumw2WhenSumw2IsSet),
        REGISTER_TEST_CASE(test_shouldNotSetSumw2WhenSumw2IsNotSet),
      };
    }

  // ==================== Test code ====================
  private:
    void beforeEach() {
      m_testObj.reset(new HistogramFactory(m_histSvc, "/HistogramFactoryTestSuite"));
    }

    void afterEach() {
      clearHistogramService();
    }

    void test_shouldRegisterAndReturnTH1FHistogram() {
      TH1F* const histogram = createHistogram<TH1F>("TH1F");
      VALUE(m_histSvc->exists("/HistogramFactoryTestSuite/TH1F")) EXPECTED(true);
      VALUE(histogram) NOT_EXPECTED(nullptr);
    }

    void test_shouldRegisterAndReturnTH1DHistogram() {
      TH1D* const histogram = createHistogram<TH1D>("TH1D");
      VALUE(m_histSvc->exists("/HistogramFactoryTestSuite/TH1D")) EXPECTED(true);
      VALUE(histogram) NOT_EXPECTED(nullptr);
    }

    void test_shouldRegisterAndReturnTH1IHistogram() {
      TH1I* const histogram = createHistogram<TH1I>("TH1I");
      VALUE(m_histSvc->exists("/HistogramFactoryTestSuite/TH1I")) EXPECTED(true);
      VALUE(histogram) NOT_EXPECTED(nullptr);
    }

    void test_shouldRegisterAndReturnTH2FHistogram() {
      TH2F* const histogram = createHistogram<TH2F>("TH2F");
      VALUE(m_histSvc->exists("/HistogramFactoryTestSuite/TH2F")) EXPECTED(true);
      VALUE(histogram) NOT_EXPECTED(nullptr);
    }

    void test_shouldRegisterAndReturnTH2DHistogram() {
      TH2D* const histogram = createHistogram<TH2D>("TH2D");
      VALUE(m_histSvc->exists("/HistogramFactoryTestSuite/TH2D")) EXPECTED(true);
      VALUE(histogram) NOT_EXPECTED(nullptr);
    }

    void test_shouldRegisterAndReturnTH2IHistogram() {
      TH2I* const histogram = createHistogram<TH2I>("TH2I");
      VALUE(m_histSvc->exists("/HistogramFactoryTestSuite/TH2I")) EXPECTED(true);
      VALUE(histogram) NOT_EXPECTED(nullptr);
    }

    void test_shouldRegisterAndReturnTProfileHistogram() {
      TProfile* const histogram = createHistogram<TProfile>("TProfile");
      VALUE(m_histSvc->exists("/HistogramFactoryTestSuite/TProfile")) EXPECTED(true);
      VALUE(histogram) NOT_EXPECTED(nullptr);
    }

    void test_shouldRegisterAndReturnTProfile2DHistogram() {
      TProfile2D* const histogram = createHistogram<TProfile2D>("TProfile2D");
      VALUE(m_histSvc->exists("/HistogramFactoryTestSuite/TProfile2D")) EXPECTED(true);
      VALUE(histogram) NOT_EXPECTED(nullptr);
    }

    void test_shouldRegisterAndReturnTEfficiencyHistogram() {
      TEfficiency* const efficiency = createEfficiency();
      VALUE(m_histSvc->existsEfficiency("/HistogramFactoryTestSuite/TEfficiency")) EXPECTED(true);
      VALUE(efficiency) NOT_EXPECTED(nullptr);
    }

    void test_shouldThrowExceptionForUnknownHistogramType() {
      try {
        createHistogram<TH1F>("UnknownType");
      } catch (const HistogramException&) {
        VALUE(m_histSvc->exists("/HistogramFactoryTestSuite/UnknownType")) EXPECTED(false);
        return;
      }
      
      assert(false);
    }

    void test_shouldProperlyFormatPathForOnlineHistograms() {
      auto possibleCases = {
        make_tuple("EXPERT", "/EXPERT/HistogramFactoryTestSuite/onlineHistAlias"), 
        make_tuple("SHIFT", "/SHIFT/HistogramFactoryTestSuite/onlineHistAlias"),
        make_tuple("DEBUG", "/DEBUG/HistogramFactoryTestSuite/onlineHistAlias"),
        make_tuple("RUNSTAT", "/RUNSTAT/HistogramFactoryTestSuite/onlineHistAlias"),
        make_tuple("EXPRESS", "/EXPRESS/HistogramFactoryTestSuite/onlineHistAlias"),
      };
  
      for (auto possibleCase : possibleCases) {
        const string onlinePath = get<0>(possibleCase);
        const string expectedPath = get<1>(possibleCase);

        HistogramDef histogramDef = defaultHistogramDef("TH1F");
        histogramDef.path = onlinePath;
        histogramDef.alias = "onlineHistAlias";
        m_testObj->create(histogramDef);
        VALUE(m_histSvc->exists(expectedPath)) EXPECTED(true);
      }
    }

    void test_shouldProperlyFormatPathForDefaultHistograms() {
      HistogramDef histogramDef = defaultHistogramDef("TH1F");
      histogramDef.path = "DEFAULT";
      histogramDef.alias = "/defaultAlias";
      m_testObj->create(histogramDef);
      VALUE(m_histSvc->exists("/HistogramFactoryTestSuite/defaultAlias")) EXPECTED(true);
    }

    void test_shouldProperlyFormatPathForCustomHistograms() {
      HistogramDef histogramDef = defaultHistogramDef("TH1F");
      histogramDef.path = "/custom/path/for/histogram";
      histogramDef.alias = "customAlias";
      m_testObj->create(histogramDef);
      VALUE(m_histSvc->exists("/HistogramFactoryTestSuite/custom/path/for/histogram/customAlias")) EXPECTED(true);
    }

    void test_shouldProperlyFormatPathForOfflineHistograms() {
      HistogramDef histogramDef = defaultHistogramDef("TH1F");
      histogramDef.path = "/custom/path/for/histogram";
      histogramDef.alias = "offlineAlias";
      histogramDef.tld = "/run_XXXXXX/lbYYY/";
      m_testObj->create(histogramDef);
      VALUE(m_histSvc->exists("/HistogramFactoryTestSuite/run_XXXXXX/lbYYY/custom/path/for/histogram/offlineAlias")) EXPECTED(true);
    }

    void test_shouldProperlyFormatPathForTempOfflineHistograms() {
      m_testObj.reset(new HistogramFactory(m_histSvc, "HistogramFactoryTestSuite"));
      HistogramDef histogramDef = defaultHistogramDef("TH1F");
      histogramDef.path = "/custom/path/for/histogram";
      histogramDef.alias = "offlineAlias";
      histogramDef.tld = "/run_XXXXXX/lbYYY/";
      m_testObj->create(histogramDef);
      VALUE(m_histSvc->exists("HistogramFactoryTestSuite/run_XXXXXX/lbYYY/custom/path/for/histogram/offlineAlias")) EXPECTED(true);
    }

    void test_shouldSetXAxisLabelsFor1DHistogram() {
      HistogramDef histogramDef = defaultHistogramDef("TH1F");
      histogramDef.alias = "labels1DTestAlias";
      histogramDef.xbins = 3;
      histogramDef.ybins = 0;
      histogramDef.xlabels = { "xlabel1", "xlabel2", "xlabel3" };
      histogramDef.ylabels = { "ylabel1" };
      TH1F* const histogram = dynamic_cast<TH1F*>(m_testObj->create(histogramDef));
 
      VALUE(histogram->GetXaxis()->GetNbins()) EXPECTED(3);
      VALUE(string(histogram->GetXaxis()->GetBinLabel(1))) EXPECTED("xlabel1");
      VALUE(string(histogram->GetXaxis()->GetBinLabel(2))) EXPECTED("xlabel2");
      VALUE(string(histogram->GetXaxis()->GetBinLabel(3))) EXPECTED("xlabel3");
      VALUE(histogram->GetYaxis()->GetNbins()) EXPECTED(1);
      VALUE(string(histogram->GetYaxis()->GetBinLabel(1))) EXPECTED("ylabel1");
    }

    void test_shouldSetXAndYAxisLabelsFor2DHistogram() {
      HistogramDef histogramDef = defaultHistogramDef("TH2F");
      histogramDef.alias = "labels2DTestAlias";
      histogramDef.xbins = 3;
      histogramDef.ybins = 3;
      histogramDef.xlabels = { "xlabel1", "xlabel2", "xlabel3" };
      histogramDef.ylabels = { "ylabel1", "ylabel2", "ylabel3" };
      TH2F* const histogram = dynamic_cast<TH2F*>(m_testObj->create(histogramDef));
 
      VALUE(histogram->GetXaxis()->GetNbins()) EXPECTED(3);
      VALUE(string(histogram->GetXaxis()->GetBinLabel(1))) EXPECTED("xlabel1");
      VALUE(string(histogram->GetXaxis()->GetBinLabel(2))) EXPECTED("xlabel2");
      VALUE(string(histogram->GetXaxis()->GetBinLabel(3))) EXPECTED("xlabel3");
      VALUE(histogram->GetYaxis()->GetNbins()) EXPECTED(3);
      VALUE(string(histogram->GetYaxis()->GetBinLabel(1))) EXPECTED("ylabel1");
      VALUE(string(histogram->GetYaxis()->GetBinLabel(2))) EXPECTED("ylabel2");
      VALUE(string(histogram->GetYaxis()->GetBinLabel(3))) EXPECTED("ylabel3");
    }

    void test_shouldSetExtendAxesWhenkCanRebinIsSet() {
      HistogramDef histogramDef = defaultHistogramDef("TH1F");
      histogramDef.alias = "allAxesRebinAlias";
      histogramDef.kCanRebin = true;
      TH1F* const histogram = dynamic_cast<TH1F*>(m_testObj->create(histogramDef));
 
      VALUE(histogram->CanExtendAllAxes()) EXPECTED(true);
    }

    void test_shouldNotSetExtendAxesWhenkCanRebinIsNotSet() {
      HistogramDef histogramDef = defaultHistogramDef("TH1F");
      histogramDef.alias = "noAxesRebinAlias";
      histogramDef.kCanRebin = false;
      TH1F* const histogram = dynamic_cast<TH1F*>(m_testObj->create(histogramDef));
 
      VALUE(histogram->CanExtendAllAxes()) EXPECTED(false);
    }

    void test_shouldSetSumw2WhenSumw2IsSet() {
      HistogramDef histogramDef = defaultHistogramDef("TH1F");
      histogramDef.alias = "Sumw2ActiveAlias";
      histogramDef.Sumw2 = true;
      TH1F* const histogram = dynamic_cast<TH1F*>(m_testObj->create(histogramDef));
 
      VALUE(histogram->GetSumw2N()) EXPECTED(3);
    }

    void test_shouldNotSetSumw2WhenSumw2IsNotSet() {
      HistogramDef histogramDef = defaultHistogramDef("TH1F");
      histogramDef.alias = "Sumw2InactiveAlias";
      histogramDef.Sumw2 = false;
      TH1F* const histogram = dynamic_cast<TH1F*>(m_testObj->create(histogramDef));
 
      VALUE(histogram->GetSumw2N()) EXPECTED(0);
    }

  // ==================== Helper methods ====================
  private:
    HistogramDef defaultHistogramDef(const string& histogramType) {
      HistogramDef result;

      result.path = "DEFAULT";
      result.type = histogramType;
      result.alias = histogramType;
      result.title = histogramType;
      result.xbins = 1;
      result.ybins = 1;
      result.zbins = 0;

      return result;
    }

    template <class HistogramType> 
    HistogramType* createHistogram(const string& histogramType) {
      HistogramDef histogramDef = defaultHistogramDef(histogramType);
      return dynamic_cast<HistogramType*>(m_testObj->create(histogramDef));
    }

    TEfficiency* createEfficiency() {
      HistogramDef histogramDef = defaultHistogramDef("TEfficiency");
      histogramDef.ybins = 0;
      histogramDef.zbins = 0;
      return dynamic_cast<TEfficiency*>(m_testObj->create(histogramDef));
    }

    void clearHistogramService() {
      for (string histName : m_histSvc->getHists()) {
        assert(m_histSvc->deReg(histName).isSuccess());
      }

      for (string treeName : m_histSvc->getTrees()) {
        assert(m_histSvc->deReg(treeName).isSuccess());
      }

      for (string graphName : m_histSvc->getGraphs()) {
        assert(m_histSvc->deReg(graphName).isSuccess());
      }

      for (string efficiencyName : m_histSvc->getEfficiencies()) {
        assert(m_histSvc->deReg(efficiencyName).isSuccess());
      }
    }

  // ==================== Initialization & run ====================
  public:
    HistogramFactoryTestSuite() 
      : m_histSvc("THistSvc", "HistogramFactoryTestSuite"), 
        m_log(Athena::getMessageSvc(), "HistogramFactoryTestSuite") {
      VALUE(m_histSvc.retrieve()) EXPECTED(StatusCode::SUCCESS);
    }

    void run() {
      for (function<void(void)> testCase : registeredTestCases()) {
        testCase();
      }
    }

  // ==================== Test case registration ====================
  private:
    typedef void (HistogramFactoryTestSuite::*TestCase)(void);

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
    ServiceHandle<ITHistSvc> m_histSvc;
    MsgStream m_log;

    shared_ptr<HistogramFactory> m_testObj;
};

int main() {
  CxxUtils::ubsan_suppress ( []() { TInterpreter::Instance(); } );
  ISvcLocator* pSvcLoc;

  if (!Athena_test::initGaudi("GenericMon.txt", pSvcLoc)) {
    throw runtime_error("This test can not be run: GenericMon.txt is missing");
  }

  HistogramFactoryTestSuite().run();

  return 0;
}
