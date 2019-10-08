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

#include "../src/HistogramFiller/StaticHistogramProvider.h"
#include "../src/HistogramFiller/LumiblockHistogramProvider.h"
#include "../src/HistogramFiller/HistogramFiller1D.h"
#include "../src/HistogramFiller/HistogramFillerEfficiency.h"
#include "../src/HistogramFiller/CumulativeHistogramFiller1D.h"
#include "../src/HistogramFiller/VecHistogramFiller1D.h"
#include "../src/HistogramFiller/VecHistogramFiller1DWithOverflows.h"
#include "../src/HistogramFiller/HistogramFillerRebinable1D.h"
#include "../src/HistogramFiller/HistogramFillerProfile.h"
#include "../src/HistogramFiller/HistogramFiller2D.h"
#include "../src/HistogramFiller/HistogramFiller2DProfile.h"

#include "../src/HistogramFiller/HistogramFillerFactory.h"

#include "mocks/MockGenericMonitoringTool.h"
#include "mocks/MockITHistSvc.h"

using namespace std;
using namespace Monitored;

class HistogramProviderGetter : public HistogramFiller {
  public: 
    HistogramProviderGetter(const HistogramFiller& hf) 
      : HistogramFiller(hf) {}

    virtual unsigned fill() { return 0; }
    virtual HistogramFiller* clone() { return nullptr; }

    std::shared_ptr<IHistogramProvider> histogramProvider() { return m_histogramProvider; }
};

#define REGISTER_TEST_CASE(TEST_CASE_NAME) registerTestCase(&HistogramFillerFactoryTestSuite::TEST_CASE_NAME, #TEST_CASE_NAME)

class HistogramFillerFactoryTestSuite {
  // ==================== All registered test cases ====================
  private:
    list<function<void(void)>> registeredTestCases() {
      return {
        REGISTER_TEST_CASE(test_shouldCreateStaticHistogramFiller1D),
        REGISTER_TEST_CASE(test_shouldCreateStaticCumulativeHistogramFiller1D),
        REGISTER_TEST_CASE(test_shouldCreateStaticVecHistogramFiller1DWithOverflows),
        REGISTER_TEST_CASE(test_shouldCreateStaticVecHistogramFiller1D),
        REGISTER_TEST_CASE(test_shouldCreateStaticHistogramFillerRebinable1D),
        REGISTER_TEST_CASE(test_shouldCreateStaticHistogramFiller2D),
        REGISTER_TEST_CASE(test_shouldCreateStaticHistogramFillerProfile),
        REGISTER_TEST_CASE(test_shouldCreateStaticHistogramFiller2DProfile),
        REGISTER_TEST_CASE(test_shouldCreateStaticHistogramFillerEfficiency),
        REGISTER_TEST_CASE(test_shouldCreateLumiblockHistogramFiller1D),
        REGISTER_TEST_CASE(test_shouldCreateLumiblockCumulativeHistogramFiller1D),
        REGISTER_TEST_CASE(test_shouldCreateLumiblockVecHistogramFiller1DWithOverflows),
        REGISTER_TEST_CASE(test_shouldCreateLumiblockVecHistogramFiller1D),
        REGISTER_TEST_CASE(test_shouldCreateLumiblockHistogramFillerRebinable1D),
        REGISTER_TEST_CASE(test_shouldCreateLumiblockHistogramFiller2D),
        REGISTER_TEST_CASE(test_shouldCreateLumiblockHistogramFillerProfile),
        REGISTER_TEST_CASE(test_shouldCreateLumiblockHistogramFiller2DProfile),
        REGISTER_TEST_CASE(test_shouldCreateLumiblockHistogramFillerEfficiency),
      };
    }

  // ==================== Test code ====================
  private:
    void beforeEach() {
      m_gmTool.reset(new MockGenericMonitoringTool());
    }

    void afterEach() {
    }

    void test_shouldCreateStaticHistogramFiller1D() {
      performCreateFillerAndVerify<HistogramFiller1D, StaticHistogramProvider>("TH1F", "");
    }

    void test_shouldCreateStaticCumulativeHistogramFiller1D() {
      performCreateFillerAndVerify<CumulativeHistogramFiller1D, StaticHistogramProvider>("TH1F", "kCumulative");
    }

    void test_shouldCreateStaticVecHistogramFiller1DWithOverflows() {
      performCreateFillerAndVerify<VecHistogramFiller1DWithOverflows, StaticHistogramProvider>("TH1F", "kVecUO");
    }

    void test_shouldCreateStaticVecHistogramFiller1D() {
      performCreateFillerAndVerify<VecHistogramFiller1D, StaticHistogramProvider>("TH1F", "kVec");
    }

    void test_shouldCreateStaticHistogramFillerRebinable1D() {
      performCreateFillerAndVerify<HistogramFillerRebinable1D, StaticHistogramProvider>("TH1F", "kAddBinsDynamically");
    }

    void test_shouldCreateStaticHistogramFiller2D() {
      performCreateFillerAndVerify<HistogramFiller2D, StaticHistogramProvider>("TH2D", "");
    }

    void test_shouldCreateStaticHistogramFillerProfile() {
      performCreateFillerAndVerify<HistogramFillerProfile, StaticHistogramProvider>("TProfile", "");
    }

    void test_shouldCreateStaticHistogramFiller2DProfile() {
      performCreateFillerAndVerify<HistogramFiller2DProfile, StaticHistogramProvider>("TProfile2D", "");
    }

    void test_shouldCreateStaticHistogramFillerEfficiency() {
      performCreateFillerAndVerify<HistogramFillerEfficiency, StaticHistogramProvider>("TEfficiency", "");
    }

    void test_shouldCreateLumiblockHistogramFiller1D() {
      performCreateFillerAndVerify<HistogramFiller1D, LumiblockHistogramProvider>("TH1F", "kLBNHistoryDepth=10");
    }

    void test_shouldCreateLumiblockCumulativeHistogramFiller1D() {
      performCreateFillerAndVerify<CumulativeHistogramFiller1D, LumiblockHistogramProvider>("TH1F", "kCumulative, kLBNHistoryDepth=10");
    }

    void test_shouldCreateLumiblockVecHistogramFiller1DWithOverflows() {
      performCreateFillerAndVerify<VecHistogramFiller1DWithOverflows, LumiblockHistogramProvider>("TH1F", "kVecUO, kLBNHistoryDepth=10");
    }

    void test_shouldCreateLumiblockVecHistogramFiller1D() {
      performCreateFillerAndVerify<VecHistogramFiller1D, LumiblockHistogramProvider>("TH1F", "kVec, kLBNHistoryDepth=10");
    }

    void test_shouldCreateLumiblockHistogramFillerRebinable1D() {
      performCreateFillerAndVerify<HistogramFillerRebinable1D, LumiblockHistogramProvider>("TH1F", "kAddBinsDynamically, kLBNHistoryDepth=10");
    }

    void test_shouldCreateLumiblockHistogramFiller2D() {
      performCreateFillerAndVerify<HistogramFiller2D, LumiblockHistogramProvider>("TH2D", "kLBNHistoryDepth=10");
    }

    void test_shouldCreateLumiblockHistogramFillerProfile() {
      performCreateFillerAndVerify<HistogramFillerProfile, LumiblockHistogramProvider>("TProfile", "kLBNHistoryDepth=10");
    }

    void test_shouldCreateLumiblockHistogramFiller2DProfile() {
      performCreateFillerAndVerify<HistogramFiller2DProfile, LumiblockHistogramProvider>("TProfile2D", "kLBNHistoryDepth=10");
    }

    void test_shouldCreateLumiblockHistogramFillerEfficiency() {
      performCreateFillerAndVerify<HistogramFillerEfficiency, LumiblockHistogramProvider>("TEfficiency", "kLBNHistoryDepth=10");
    }


  // ==================== Helper methods ====================
  private:
    template<class FillerType, class ProviderType>
    void performCreateFillerAndVerify(string histogramType, string options) {
      HistogramDef histogramDef;
      histogramDef.type = histogramType;
      histogramDef.opt = options;
      histogramDef.xbins = 1;
      histogramDef.ybins = 1;

      HistogramFillerFactory testObj(m_gmTool.get(), "HistogramFillerFactoryTestSuite");

      HistogramFiller* const result = testObj.create(histogramDef);
      VALUE(dynamic_cast<FillerType*>(result)) NOT_EXPECTED(nullptr);
      HistogramProviderGetter providerGetter(*result);
      IHistogramProvider* const provider = providerGetter.histogramProvider().get();
      VALUE(dynamic_cast<ProviderType*>(provider)) NOT_EXPECTED(nullptr);
    }

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
