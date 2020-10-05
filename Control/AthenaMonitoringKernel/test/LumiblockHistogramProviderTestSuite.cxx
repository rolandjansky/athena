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

#include "mocks/MockGenericMonitoringTool.h"
#include "mocks/MockHistogramFactory.h"

#include "../src/HistogramFiller/LumiblockHistogramProvider.h"
#include "../src/HistogramFiller/OfflineHistogramProvider.h"

using namespace std;
using namespace Monitored;

#define REGISTER_TEST_CASE(TEST_CASE_NAME) registerTestCase(&LumiblockHistogramProviderTestSuite::TEST_CASE_NAME, #TEST_CASE_NAME)

class LumiblockHistogramProviderTestSuite {
  // ==================== All registered test cases ====================
  private:
    list<function<void(void)>> registeredTestCases() {
      return {
        REGISTER_TEST_CASE(test_shouldThrowExceptionWhen_kLBNHistoryDepth_isNonPositive),
        REGISTER_TEST_CASE(test_shouldNotThrowExceptionWhen_kLBNHistoryDepth_isDefinedAsNumber),
        REGISTER_TEST_CASE(test_shouldCreateNewHistogramWithUpdatedAlias),
        REGISTER_TEST_CASE(test_shouldCreateNewHistogramWithUpdatedLumiBlock)
      };
    }

  // ==================== Test code ====================
  private:
    void beforeEach() {
      m_gmTool.reset(new MockGenericMonitoringTool());
      m_histogramFactory.reset(new MockHistogramFactory());
    }

    void afterEach() {
    }

    void test_shouldThrowExceptionWhen_kLBNHistoryDepth_isNonPositive() {
      try {
        HistogramDef histogramDef;
        histogramDef.kLBNHistoryDepth = -1;
        LumiblockHistogramProvider testObj(m_gmTool.get(), m_histogramFactory, histogramDef);
        testObj.histogram();
      } catch (HistogramException&) {
        return;
      }
      assert(false);
    }

    void test_shouldNotThrowExceptionWhen_kLBNHistoryDepth_isDefinedAsNumber() {
      HistogramDef histogramDef;
      histogramDef.kLBNHistoryDepth = 12345;
      LumiblockHistogramProvider testObj(m_gmTool.get(), m_histogramFactory, histogramDef);
    }

    void test_shouldCreateNewHistogramWithUpdatedAlias() {
      auto expectedFlow = {
        make_tuple(0, "test alias_LB0_2"),
        make_tuple(1, "test alias_LB0_2"),
        make_tuple(2, "test alias_LB0_2"),
        make_tuple(3, "test alias_LB3_5"),
        make_tuple(4, "test alias_LB3_5"),
        make_tuple(5, "test alias_LB3_5"),
        make_tuple(6, "test alias_LB6_8"),
        make_tuple(7, "test alias_LB6_8"),
        make_tuple(8, "test alias_LB6_8"),
        make_tuple(9, "test alias_LB9_11"),
      };

      TH1F histogram("h", "h", 1, 0, 1);
      HistogramDef histogramDef;
      histogramDef.alias = "test alias";
      histogramDef.kLBNHistoryDepth = 3;

      m_gmTool->histSvc().mock_always_empty = false; // the moc actually keeps track of registered histograms
      LumiblockHistogramProvider testObj(m_gmTool.get(), m_histogramFactory, histogramDef);

      for (auto input : expectedFlow) {
        const unsigned lumiBlock = get<0>(input);
        const string expectedAlias = get<1>(input);

        m_gmTool->mock_lumiBlock = [&]() { return lumiBlock; };
        m_histogramFactory->mock_create = [&](const HistogramDef& def) mutable {
          VALUE(def.alias) EXPECTED(expectedAlias);
	  m_log << MSG::INFO << "Registering: " << def.alias << endmsg;
	  m_gmTool->histSvc().regHist(m_histogramFactory->getFullName(def), &histogram).ignore();
          return &histogram;
        };
	m_histogramFactory->mock_remove = [&](const Monitored::HistogramDef& def) {
	  m_log << MSG::INFO << "Deregistering: " << def.alias << endmsg;
	  m_gmTool->histSvc().deReg(m_histogramFactory->getFullName(def)).ignore();
	  return nullptr;
	};


        TNamed* const result = testObj.histogram();
        VALUE(result) EXPECTED(&histogram);
      }
      VALUE( m_gmTool->histSvc().mock_registered.size() ) EXPECTED ( 1 );
    }

    void test_shouldCreateNewHistogramWithUpdatedLumiBlock() {
      auto expectedFlow = {
        make_tuple(100, 100000, "/run_100000/lowStat_LB101-120/"),
        make_tuple(125, 200000, "/run_200000/lowStat_LB121-140/"),
      };

      TNamed histogram;
      HistogramDef histogramDef;
      histogramDef.convention = "OFFLINE:lowStat";
      histogramDef.runmode = HistogramDef::RunMode::Offline;
      histogramDef.runperiod = HistogramDef::RunPeriod::LowStat;

      OfflineHistogramProvider testObj(m_gmTool.get(), m_histogramFactory, histogramDef);

      for (auto input : expectedFlow) {
        const unsigned lumiBlock = get<0>(input);
        const unsigned runNumber = get<1>(input);
        const string expectedTld = get<2>(input);

        m_gmTool->mock_lumiBlock = [lumiBlock]() { return lumiBlock; };
        m_gmTool->mock_runNumber = [runNumber]() { return runNumber; };
        m_histogramFactory->mock_create = [&histogram, expectedTld](const HistogramDef& def) mutable {
          VALUE(def.tld) EXPECTED(expectedTld);
          return &histogram;
        };

        TNamed* const result = testObj.histogram();
        VALUE(result) EXPECTED(&histogram);
      }
    }

  // ==================== Helper methods ====================
  private:

  // ==================== Initialization & run ====================
  public:
    LumiblockHistogramProviderTestSuite()
      : m_log(Athena::getMessageSvc(), "LumiblockHistogramProviderTestSuite") {
    }

    void run() {
      for (function<void(void)> testCase : registeredTestCases()) {
        testCase();
      }
    }

  // ==================== Test case registration ====================
  private:
    typedef void (LumiblockHistogramProviderTestSuite::*TestCase)(void);

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
    shared_ptr<MockHistogramFactory> m_histogramFactory;
};

int main() {
  ISvcLocator* pSvcLoc;

  if (!Athena_test::initGaudi("GenericMon.txt", pSvcLoc)) {
    throw runtime_error("This test can not be run: GenericMon.txt is missing");
  }

  LumiblockHistogramProviderTestSuite().run();

  return 0;
}
