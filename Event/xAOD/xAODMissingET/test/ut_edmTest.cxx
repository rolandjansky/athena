/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

/* 
 * This test is no thread safe
 * but we are fine with it
 */

#include "CxxUtils/checker_macros.h"
ATLAS_NO_CHECK_FILE_THREAD_SAFETY;

#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_MAIN
#define BOOST_TEST_MODULE TEST_XAOD_MET_EDM
#include <boost/test/unit_test.hpp>

#include "xAODMissingET/MissingET.h"
#include "xAODMissingET/MissingETContainer.h"
#include "xAODMissingET/MissingETAuxContainer.h"
//if you need to test private members
// #define private public //ugly hacking way to do this...
// #define protected public
// #include "METReconstruction/METAssociationTool.h"
// #undef private
// #undef protected

using namespace xAOD;

struct perTestSetup
{
  perTestSetup()   { 
    BOOST_TEST_MESSAGE("starting test" );
  }
 
  ~perTestSetup()
  {
    BOOST_TEST_MESSAGE("ending test");
  }
};

BOOST_FIXTURE_TEST_SUITE(Test_Edm , perTestSetup)

BOOST_AUTO_TEST_CASE( edmTest ){
  MissingET* m0 = new MissingET( 10., 10.,sqrt(200.),"MET0");
  MissingET* m1 = new MissingET(  5.,-10.,sqrt(125.),"MET1");

  MissingETAuxContainer aux;
  MissingETContainer metColl;
  metColl.setStore( &aux );

  metColl.push_back(m0);
  BOOST_REQUIRE(metColl.size() == 1);

  metColl.push_back(m1);

  // printf("xAODMissingET::edm_test: have %i objects in container\n",
  //        (int)metColl.size());

  BOOST_REQUIRE( metColl.size() == 2);
  BOOST_REQUIRE( metColl.find("MET1") != metColl.end() );

  //  MissingETContainer::const_iterator fm0(metColl.find("MET1"));

  ////  BOOST_REQUIRE(  !(fm0 == metColl.end()) );
  // if ( fm0 == metColl.end() )
  //   { printf("xAODMissingET::edm_test: cannot find object MET1\n"); }
  // else
  //   { printf("xAODMissingET::edm_test: found object MET1\n"); }

}

BOOST_AUTO_TEST_SUITE_END() //Test_Edm
