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
     ServiceHandle<IInDetEtaDependentCutsSvc> idEDCSvc ("InDet::InDetEtaDependentCutsSvc", "test");
     InterfaceID testId("IInDetEtaDependentCutsSvc",1,0);
     BOOST_TEST(idEDCSvc->interfaceID()==testId);
   }
   BOOST_AUTO_TEST_CASE(publicMethods){
     ServiceHandle<IInDetEtaDependentCutsSvc> idEDCSvc ("InDet::InDetEtaDependentCutsSvc", "test");
     BOOST_TEST(idEDCSvc->getMaxEta() == 4.0);
   }
BOOST_AUTO_TEST_SUITE_END()
