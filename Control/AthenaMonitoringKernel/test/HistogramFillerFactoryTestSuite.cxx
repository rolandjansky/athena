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

#include "AthenaMonitoringKernel/HistogramDef.h"

#include "../src/HistogramFiller/StaticHistogramProvider.h"
#include "../src/HistogramFiller/LumiblockHistogramProvider.h"
#include "../src/HistogramFiller/HistogramFiller1D.h"
#include "../src/HistogramFiller/HistogramFillerEfficiency.h"
#include "../src/HistogramFiller/CumulativeHistogramFiller1D.h"
#include "../src/HistogramFiller/VecHistogramFiller1D.h"
#include "../src/HistogramFiller/HistogramFillerRebinable.h"
#include "../src/HistogramFiller/HistogramFillerProfile.h"
#include "../src/HistogramFiller/HistogramFiller2D.h"
#include "../src/HistogramFiller/HistogramFiller2DProfile.h"

#include "../src/HistogramFiller/HistogramFillerFactory.h"

#include "mocks/MockGenericMonitoringTool.h"
#include "mocks/MockITHistSvc.h"
#include "mocks/MockHistogramDef.h"

using namespace std;
using namespace Monitored;

class HistogramProviderGetter : public HistogramFiller {
  public: 
    HistogramProviderGetter(const HistogramFiller& hf) 
      : HistogramFiller(hf) {}

    virtual unsigned fill() const { return 0; }
    virtual HistogramFiller* clone() const { return nullptr; }

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
        REGISTER_TEST_CASE(test_shouldCreateStaticVecHistogramFiller1D),
        REGISTER_TEST_CASE(test_shouldCreateStaticHistogramFillerRebinable1D),
        REGISTER_TEST_CASE(test_shouldCreateStaticHistogramFiller2D),
        REGISTER_TEST_CASE(test_shouldCreateStaticHistogramFillerProfile),
        REGISTER_TEST_CASE(test_shouldCreateStaticHistogramFiller2DProfile),
        REGISTER_TEST_CASE(test_shouldCreateStaticHistogramFillerEfficiency),
        REGISTER_TEST_CASE(test_shouldCreateLumiblockHistogramFiller1D),
        REGISTER_TEST_CASE(test_shouldCreateLumiblockCumulativeHistogramFiller1D),
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
      m_histDef.reset(new MockHistogramDef());
    }

    void afterEach() {
    }

    void test_shouldCreateStaticHistogramFiller1D() {
      m_histDef->type = "TH1F";
      performCreateFillerAndVerify<HistogramFiller1D, StaticHistogramProvider>();
    }

    void test_shouldCreateStaticCumulativeHistogramFiller1D() {
      m_histDef->kCumulative = true;
      performCreateFillerAndVerify<CumulativeHistogramFiller1D, StaticHistogramProvider>();
    }

    void test_shouldCreateStaticVecHistogramFiller1D() {
      m_histDef->kVec = true;
      performCreateFillerAndVerify<VecHistogramFiller1D, StaticHistogramProvider>();
    }

    void test_shouldCreateStaticHistogramFillerRebinable1D() {
      m_histDef->kAddBinsDynamically = true;
      m_histDef->type = "TH1F";
      performCreateFillerAndVerify<HistogramFillerRebinable1D, StaticHistogramProvider>();
    }

    void test_shouldCreateStaticHistogramFiller2D() {
      m_histDef->type = "TH2D";
      performCreateFillerAndVerify<HistogramFiller2D, StaticHistogramProvider>();
    }

    void test_shouldCreateStaticHistogramFillerProfile() {
      m_histDef->type = "TProfile";
      performCreateFillerAndVerify<HistogramFillerProfile, StaticHistogramProvider>();
    }

    void test_shouldCreateStaticHistogramFiller2DProfile() {
      m_histDef->type = "TProfile2D";
      performCreateFillerAndVerify<HistogramFiller2DProfile, StaticHistogramProvider>();
    }

    void test_shouldCreateStaticHistogramFillerEfficiency() {
      m_histDef->type = "TEfficiency";
      performCreateFillerAndVerify<HistogramFillerEfficiency, StaticHistogramProvider>();
    }

    void test_shouldCreateLumiblockHistogramFiller1D() {
      m_histDef->kLBNHistoryDepth = 10;
      performCreateFillerAndVerify<HistogramFiller1D, LumiblockHistogramProvider>();
    }

    void test_shouldCreateLumiblockCumulativeHistogramFiller1D() {
      m_histDef->kCumulative = true;
      m_histDef->kLBNHistoryDepth = 10;
      performCreateFillerAndVerify<CumulativeHistogramFiller1D, LumiblockHistogramProvider>();
    }

    void test_shouldCreateLumiblockVecHistogramFiller1D() {
      m_histDef->kVec = true;
      m_histDef->kLBNHistoryDepth = 10;
      performCreateFillerAndVerify<VecHistogramFiller1D, LumiblockHistogramProvider>();
    }

    void test_shouldCreateLumiblockHistogramFillerRebinable1D() {
      m_histDef->kAddBinsDynamically = true;
      m_histDef->kLBNHistoryDepth = 10;
      performCreateFillerAndVerify<HistogramFillerRebinable1D, LumiblockHistogramProvider>();
    }

    void test_shouldCreateLumiblockHistogramFiller2D() {
      m_histDef->type = "TH2D";
      m_histDef->kLBNHistoryDepth = 10;
      performCreateFillerAndVerify<HistogramFiller2D, LumiblockHistogramProvider>();
    }

    void test_shouldCreateLumiblockHistogramFillerProfile() {
      m_histDef->type = "TProfile";
      m_histDef->kLBNHistoryDepth = 10;
      performCreateFillerAndVerify<HistogramFillerProfile, LumiblockHistogramProvider>();
    }

    void test_shouldCreateLumiblockHistogramFiller2DProfile() {
      m_histDef->type = "TProfile2D";
      m_histDef->kLBNHistoryDepth = 10;
      performCreateFillerAndVerify<HistogramFiller2DProfile, LumiblockHistogramProvider>();
    }

    void test_shouldCreateLumiblockHistogramFillerEfficiency() {
      m_histDef->type = "TEfficiency";
      m_histDef->kLBNHistoryDepth = 10;
      performCreateFillerAndVerify<HistogramFillerEfficiency, LumiblockHistogramProvider>();
    }


  // ==================== Helper methods ====================
  private:
    template<class FillerType, class ProviderType>
    void performCreateFillerAndVerify() {
      HistogramFillerFactory testObj(m_gmTool.get(), "HistogramFillerFactoryTestSuite");
      HistogramFiller* const result = testObj.create(*m_histDef);
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
    shared_ptr<HistogramDef> m_histDef;
};

int main() {
  ISvcLocator* pSvcLoc;

  if (!Athena_test::initGaudi("GenericMon.txt", pSvcLoc)) {
    throw runtime_error("This test can not be run: GenericMon.txt is missing");
  }

  HistogramFillerFactoryTestSuite().run();

  return 0;
}
