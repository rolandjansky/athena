/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

/**
 * @file InDetPhysValMonitoring/test/HistogramDefinitionSvc_test.cxx
 * @author Shaun Roe
 * @date May 2020
 * @brief Some tests for HistogramDefinitionSvc service 
 */

#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_MAIN
#define BOOST_TEST_MODULE TEST_IOVDBSVC

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Woverloaded-virtual"
#include <boost/test/unit_test.hpp>
#pragma GCC diagnostic pop

#include "IDPVM_GaudiFixtureBase.h"
#include "../InDetPhysValMonitoring/HistogramDefinitionSvc.h"
#include "../InDetPhysValMonitoring/SingleHistogramDefinition.h"
#include <utility> //std::pair

#include "CxxUtils/checker_macros.h"
ATLAS_NO_CHECK_FILE_THREAD_SAFETY; // This test uses global svcLoc and hDefSvc, and is not thread safe.

static const std::string testJobOptionsFile("IDPVM_Test.txt");

struct GaudiKernelFixture:public IDPVM_GaudiFixtureBase{
  GaudiKernelFixture(const std::string & joFilename):IDPVM_GaudiFixtureBase(joFilename){
    //nop, everything in base.
  }
};

BOOST_AUTO_TEST_SUITE(HistogramDefinitionSvcTest)
  GaudiKernelFixture g(testJobOptionsFile);
  const auto & svcLoc=g.svcLoc;
  ServiceHandle<IHistogramDefinitionSvc> hDefSvc ("HistogramDefinitionSvc", "test");

  BOOST_AUTO_TEST_CASE( sanityCheck ){
    const bool svcLocatorIsOk=(svcLoc != nullptr);
    BOOST_TEST(svcLocatorIsOk);
  }

  BOOST_AUTO_TEST_CASE(histogramDefinitionSvcRetrieved){
    BOOST_TEST (hDefSvc.retrieve().isSuccess());
  }

  BOOST_AUTO_TEST_CASE(interfaceID){
    InterfaceID testId("IHistogramDefinitionSvc",1,0);
    BOOST_TEST(hDefSvc->interfaceID()==testId);
  }
  
  BOOST_AUTO_TEST_CASE(publicMethods){
    //Histogram definitions are in the test file ../share/testHDef.xml and its inclusion,
    //../share/inc.xml
    /**
      This is a straightforward definition in the testHDef.xml file:
      <h id="002_Al_N_01" type="TH1F" title="[SRoe] Number of Selected Tracks">
        <x title="Num. tracks" n="200" lo="0" hi="1000"/>
        <y title="Entries"/>
      </h>
    **/
    const std::string hId{"002_Al_N_01"};
    const std::string hType{"TH1F"};
    const std::string hTitle{"[SRoe] Number of Selected Tracks"};
    const std::string xTitle{"Num. tracks"};
    const unsigned int nXbins{200};
    const float xLo{0.};
    const float xHi{1000.};
    const std::string yTitle{"Entries"};
    SingleHistogramDefinition referenceDefinition(hId, hType, hTitle, nXbins, xLo, xHi, xTitle, yTitle );
    //operator == is not defined on SingleHistogramDefinition, so make an ad-hoc lambda
    auto sameDefinition = [](const SingleHistogramDefinition &s1, const SingleHistogramDefinition &s2){
      bool sameTitles = (s1.titleDigest() == s2.titleDigest());
      bool sameNames = (s1.name == s2.name);
      bool sameBinning = (s1.nBinsX == s2.nBinsX) and (s1.xAxis == s2.xAxis);
      return sameTitles and sameNames and sameBinning;
    };
    BOOST_TEST(sameDefinition(hDefSvc->definition("002_Al_N_01"), referenceDefinition));
    // check facade methods which just pass through to the SingleHistogramDefinition InterfaceID
    BOOST_TEST(hDefSvc->histoType("002_Al_N_01") == hType);
    BOOST_TEST(hDefSvc->title("002_Al_N_01") == hTitle);
    BOOST_TEST(hDefSvc->nBinsX("002_Al_N_01") == 200);
    BOOST_TEST(hDefSvc->nBinsY("002_Al_N_01") == 0);
    BOOST_TEST(hDefSvc->nBinsZ("002_Al_N_01") == 0);
    const std::pair<float, float> xAxis{xLo,xHi};
    BOOST_TEST((hDefSvc->xLimits("002_Al_N_01") == xAxis));
    //not checking y axis, z axis
    BOOST_TEST(hDefSvc->xTitle("002_Al_N_01") == xTitle);
    BOOST_TEST(hDefSvc->yTitle("002_Al_N_01") == yTitle);
    BOOST_TEST(hDefSvc->zTitle("002_Al_N_01").empty());
    //what happens if you try to retrieve a non existent definition?
    BOOST_TEST(hDefSvc->definition("non_existent_definition").isValid() == false);
    BOOST_TEST(hDefSvc->definition("non_existent_definition").empty());
    // The following are just tests of the xml parsing capability, could be moved to 
    // another test module
    //
    // Does in-file entity definition work? e.g. <!ENTITY ETAMAX "2.5">, 
    // <x title="#eta" n="20" lo="-&ETAMAX;" hi="&ETAMAX;"/>
    const std::string definitionIdUsingEntity("track_fakerate_vs_eta");
    const std::pair<float, float> etaRange{-2.5,2.5};
    BOOST_TEST((hDefSvc->definition(definitionIdUsingEntity).xAxis == etaRange));
    //Does the XInclude capability work?
    const std::string xincludedDefinition("include");
    BOOST_TEST(hDefSvc->definition(xincludedDefinition).isValid());
    //Does file inclusion using entities work? (todo)
    //
    // What has not been checked: Non-existent file or XML parsing errors
  }
BOOST_AUTO_TEST_SUITE_END()

  
