/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

/**
 * @file InDetPhysValMonitoring/test/InDetPlotBase_test.cxx
 * @author Shaun Roe
 * @date May 2020
 * @brief Some tests for InDetPlotBase 
 */

#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_MAIN
#define BOOST_TEST_MODULE TEST_IOVDBSVC

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Woverloaded-virtual"
#include <boost/test/unit_test.hpp>
#pragma GCC diagnostic pop

#include "../src/InDetPlotBase.h"
#include <random>
#include "TEfficiency.h"
#include "xAODTracking/TrackParticle.h"

#include "IDPVM_GaudiFixtureBase.h"
#include "../InDetPhysValMonitoring/HistogramDefinitionSvc.h"
#include "../InDetPhysValMonitoring/SingleHistogramDefinition.h"
#include <utility> //std::pair

static const std::string testJobOptionsFile("IDPVM_Test.txt");

struct GaudiKernelFixture:public IDPVM_GaudiFixtureBase{
  GaudiKernelFixture(const std::string & joFilename):IDPVM_GaudiFixtureBase(joFilename){
    //nop, everything in base.
  }
};

/**
 * @class InDetTestPlot
 * @brief Class holding plots to test the histogram definition service and subsequent filling
 */
class InDetTestPlot: public InDetPlotBase {
public:
  InDetTestPlot(InDetPlotBase* pParent, const std::string& dirName);
  void fill(const xAOD::TrackParticle& particle);
  // plot base has nop default implementation of this; we use it to book the histos
  void initializePlots();
  //expose members for testing
  TH1* pTest0(){return m_test;}
  TH1* pTest1(){return m_test1;}
  TH1* pTest2(){return m_test2;}
  TProfile * pTProfile(){return m_testProfile;}
  TH2* pTH2(){return m_test2D;}
  TEfficiency * pTEff(){return m_testEff;}
  TH1* pNonsense(){return m_nonsense;}
  int detailLevel() const {return m_iDetailLevel;}
private:
  TH1* m_test;
  TH1* m_test1;
  TH1* m_test2;
  TProfile* m_testProfile;
  TH2* m_test2D;
  TEfficiency* m_testEff;
  TH1* m_nonsense;
  
};

InDetTestPlot::InDetTestPlot(InDetPlotBase* pParent, const std::string& sDir) : InDetPlotBase(pParent, sDir),
  m_test(nullptr), m_test1(nullptr), m_test2(nullptr), m_testProfile(nullptr), m_test2D(nullptr), m_testEff(nullptr),
  m_nonsense(nullptr) {
}

void
InDetTestPlot::initializePlots() {
  ATH_MSG_INFO("Hello, I am a test plot");
  /**
   * This was the pre-existing way of booking a histogram without using the HistogramDefinitionSvc
   * const bool prependDirectory(false);
   * m_test  = Book1D("testHistoId","my title",nBinsX,xAxisStart,xAxisEnd, prependDirectory);
   **/

  /**
   *  This macro (in the base class) uses the variable name to infer the identifier in the
   *	hdef file by stripping off the 'm_' prefix; this is convenient *IF* you know what
   *  you're doing.
   **/
  IDPVM_BOOK(m_test); // books a histogram with hdef identifier of 'test'
  //
  /**
   * This is a two step method which retrieves the histogram definition explicitly and then
   * uses it. This allows the same definition to be used for multiple histograms
   **/
  SingleHistogramDefinition hd = retrieveDefinition("test");
  /** definition in testHdef.xml : 
  <h id="test" type="TH1F" title="Test Case: Visible E_{T} (GeV)">
    <x title="Visible E_{T} (GeV)" n="100" lo="0" hi="100"/>
    <y title="Num. entries"/>
  </h>
  **/
  hd.name = "test1";
  book(m_test1, hd);
  hd.name = "test2";
  book(m_test2, hd); // book two histograms with the same axes, title etc
  //
  /**
   * This is likely to be the most common use case for the HistogramDefinitionSvc;
   * a histogram is booked using the hdef identifier 'testProfile' and is handed
   * the appropriate pointer. In this case a TProfile example is used, but also works for TH1
   **/
  book(m_testProfile, "testProfile");
  book(m_test2D, "test2D");
  SingleHistogramDefinition hdEff = retrieveDefinition("testEfficiency");
  book(m_testEff, "testEfficiency");
  book(m_nonsense, "rubbish");
}

void
InDetTestPlot::fill(const xAOD::TrackParticle& /*particle*/) {
  std::random_device rd;
  std::mt19937 gen(rd());
  std::normal_distribution<float> d(50, 4);
  const float pt(d(gen));
  fillHisto(m_test, pt);
  std::normal_distribution<float> d1(70, 4);
  fillHisto(m_test1, d1(gen));
  const float p(rand() % 50);
  fillHisto(m_testProfile, p, 1);
  const float eta = (rand() % 6) - 3;
  const float ybin = (rand() % 20);
  fillHisto(m_test2D, eta, ybin);
  const bool passed(p > 25);
  fillHisto(m_testEff, p, passed);
  fillHisto(m_nonsense, pt);
}





BOOST_AUTO_TEST_SUITE(InDetPlotBaseTest)
 GaudiKernelFixture g(testJobOptionsFile);
  const auto & svcLoc=g.svcLoc;
  ServiceHandle<IHistogramDefinitionSvc> hDefSvc ("HistogramDefinitionSvc", "test");

  BOOST_AUTO_TEST_CASE(constructor){
    BOOST_CHECK_NO_THROW(InDetTestPlot p(nullptr, "IDPVM_Testing/"));
  }
  BOOST_AUTO_TEST_CASE(initialize){
    InDetTestPlot p(nullptr, "IDPVM_Testing/");
    BOOST_CHECK_NO_THROW(p.initializePlots());
    //check the expected histograms are booked by retrieving their names;
    const std::string test0Name("test");
    BOOST_TEST(p.pTest0()->GetName() == test0Name, "TH1 Booked using IDPVM_BOOK macro");
    const std::string test1Name("test1");
    BOOST_TEST(p.pTest1()->GetName() == test1Name, "TH1 Booked using retrieved histogram definition and 'book' method");
    const std::string test2Name("test2");
    BOOST_TEST(p.pTest2()->GetName() == test2Name, "TH1 Booked with different name using the same retrieved histogram definition");
    const std::string tProfileName("testProfile");
    BOOST_TEST(p.pTProfile()->GetName() == tProfileName, "TProfile Booked using 'book' method and identifier");
    const std::string tH2Name("test2D");
    BOOST_TEST(p.pTH2()->GetName() == tH2Name, "TH2 Booked using 'book' method and identifier");
    const std::string tEffName("testEfficiency");
    BOOST_TEST(p.pTEff()->GetName() == tEffName, "TEfficiency Booked using retrieved definition");
    BOOST_TEST(p.pNonsense() == nullptr, "Trying to book an undefined histogram results in nullptr");
    BOOST_CHECK_NO_THROW(p.finalize());
  }
  BOOST_AUTO_TEST_CASE(fill){
    InDetTestPlot p(nullptr, "IDPVM_Testing/");
    p.initializePlots();
    const xAOD::TrackParticle tp;
    //check the fill method is at least sane
    BOOST_CHECK_NO_THROW(p.fill(tp));
    //the underlying histograms now have one entry
    BOOST_TEST(p.pTest0()->GetEntries() == 1);
    for (int i{};i!=99;++i){
      p.fill(tp);
    }
    //..and after 99 more, num. entries = 100
    BOOST_TEST(p.pTest0()->GetEntries() == 100);
    p.finalize();
  }
  BOOST_AUTO_TEST_CASE(baseclassMethods){
    InDetTestPlot p(nullptr, "IDPVM_Testing/");
    BOOST_CHECK_NO_THROW(p.setDetailLevel(100));
    BOOST_CHECK(p.detailLevel() == 100);
    p.initializePlots();
    //We tried to book 6 histograms...
    const auto allHistograms = p.retrieveBookedHistograms();
    BOOST_TEST(allHistograms.size() == 5);//one histogram was nonsense, so only 5 booked
    //...and one Efficiency plot
    const auto allEfficiencies = p.retrieveBookedEfficiencies();
    BOOST_TEST(allEfficiencies.size() == 1);
    p.finalize();
  }
  
BOOST_AUTO_TEST_SUITE_END()
