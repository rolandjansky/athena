/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

/**
 * @file InDetEtaDependentCuts/test/InDetEtaDependentCutsSvc_test.cxx
 * @author Shaun Roe
 * @date June, 2020
 * @brief Some tests for InDetEtaDependentCutsSvc 
 */
 
#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_MAIN
#define BOOST_TEST_MODULE TEST_ETADEPENDENTCUTSSVC


#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Woverloaded-virtual"
#include <boost/test/unit_test.hpp>
#pragma GCC diagnostic pop

#include "IDEDCSvc_GaudiFixtureBase.h"
#include "../InDetEtaDependentCuts/InDetEtaDependentCutsSvc.h"

#include "CxxUtils/checker_macros.h"
ATLAS_NO_CHECK_FILE_THREAD_SAFETY; // This is unit test and a static variable is used in IDEDC_GaudiFixtureBase.

namespace utf = boost::unit_test;

static const std::string testJobOptionsFile("InDetEtaDependentCutsTestJobOpts.txt");

struct GaudiKernelFixture:public IDEDC_GaudiFixtureBase{
  GaudiKernelFixture(const std::string & joFilename):IDEDC_GaudiFixtureBase(joFilename){
    //nop, everything in base.
  }
};

using namespace InDet;

BOOST_AUTO_TEST_SUITE(EtaDependentCutsSvcTest)
  const GaudiKernelFixture g(testJobOptionsFile);
  const auto & svcLoc=g.svcLoc;

  BOOST_AUTO_TEST_CASE( sanityCheck ){
    const bool svcLocatorIsOk=(svcLoc != nullptr);
    BOOST_TEST(svcLocatorIsOk);
  }
  BOOST_AUTO_TEST_CASE( retrieveSvc ){
    ServiceHandle<IInDetEtaDependentCutsSvc> idEDCSvc ("InDet::InDetEtaDependentCutsSvc", "test");
    BOOST_TEST (idEDCSvc.retrieve().isSuccess());
  }
  BOOST_AUTO_TEST_CASE( interfaceID ){
    InterfaceID testId("IInDetEtaDependentCutsSvc",1,0);
    ServiceHandle<IInDetEtaDependentCutsSvc> idEDCSvc ("InDet::InDetEtaDependentCutsSvc", "test");
    BOOST_TEST(idEDCSvc->interfaceID()==testId);
  }
  
  BOOST_AUTO_TEST_CASE(publicMethods){
    ServiceHandle<IInDetEtaDependentCutsSvc> idEDCSvc ("InDet::InDetEtaDependentCutsSvc", "test");
    //for default constructed service, and choosing a nominal (valid) eta
    const double eta=2.0;
    const double invalidEta{9.0};
    //double  getMaxEta()
    BOOST_TEST(idEDCSvc->getMaxEta() == 4.0); 
    //double  getMinPtAtEta           (const double eta)
    BOOST_TEST(idEDCSvc->getMinPtAtEta(eta) == 900.);
    //check it throws if eta is out of bounds (will also print error)
    BOOST_CHECK_THROW(idEDCSvc->getMinPtAtEta(invalidEta), std::out_of_range);
    //double  getMaxZImpactAtEta      (const double eta)
    BOOST_TEST(idEDCSvc->getMaxZImpactAtEta(eta) == 200.);
    //double  getMaxPrimaryImpactAtEta(const double eta);
    BOOST_TEST(idEDCSvc->getMaxPrimaryImpactAtEta(eta) == 2.0);
    //int     getMinSiHitsAtEta       (const double eta);
    BOOST_TEST(idEDCSvc->getMinSiHitsAtEta(eta) == 9);
    //int     getMinSiNotSharedAtEta  (const double eta);
    BOOST_TEST(idEDCSvc->getMinSiNotSharedAtEta(eta) == 7);
    //int     getMaxSharedAtEta       (const double eta);
    BOOST_TEST(idEDCSvc->getMaxSharedAtEta(eta) == 2);
    //int     getMinPixelHitsAtEta    (const double eta);
    BOOST_TEST(idEDCSvc->getMinPixelHitsAtEta(eta) == 1);
    //int     getMaxSiHolesAtEta      (const double eta);
    BOOST_TEST(idEDCSvc->getMaxSiHolesAtEta(eta) == 2);
    //int     getMaxPixelHolesAtEta   (const double eta);
    BOOST_TEST(idEDCSvc->getMaxPixelHolesAtEta(eta) == 1);
    //int     getMaxSctHolesAtEta     (const double eta);
    BOOST_TEST(idEDCSvc->getMaxSctHolesAtEta(eta) == 2);
    //int     getMaxDoubleHolesAtEta  (const double eta) const override final;
    BOOST_TEST(idEDCSvc->getMaxDoubleHolesAtEta(eta) == 1);
    //return values on argument-changing methods
    std::vector<int> intCutReturnValue{};
    std::vector<double> doubleCutReturnValue{};
    const auto dblCutEnum = InDet::CutName::etaWidthBrem;
    const auto intCutEnum = InDet::CutName::minClusters;
    //
    BOOST_CHECK_NO_THROW(idEDCSvc->getValue(dblCutEnum, doubleCutReturnValue));
    BOOST_TEST(doubleCutReturnValue.at(0) == 0.2); //only has one entry by default
    BOOST_CHECK_NO_THROW(idEDCSvc->getValue(intCutEnum, intCutReturnValue));
    BOOST_TEST(intCutReturnValue.at(0) == 9); //only has one entry by default
    //try to return an integer cut from a double enum  index (will print an ERROR message)
    BOOST_CHECK_NO_THROW(idEDCSvc->getValue(dblCutEnum, intCutReturnValue));//doesn't throw
    //
    BOOST_TEST(intCutReturnValue.at(0) == 9);//i.e. should remain unchanged
    
  }
BOOST_AUTO_TEST_SUITE_END()
