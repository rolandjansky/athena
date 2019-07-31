/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/
/*
 */
/**
 * @file IOVDbSvc/test/IOVDbSvcCurl_test.cxx
 * @author Shaun Roe
 * @date May, 2019
 * @brief Some tests for IOVDbSvcCurl 
 */

#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_MAIN
#define BOOST_TEST_MODULE TEST_IOVDBSVC


#include <boost/test/unit_test.hpp>
//

#include "../src/IOVDbSvcCurl.h"

using namespace IOVDbNamespace;

BOOST_AUTO_TEST_SUITE(IOVDbSvcCurlTest)
  BOOST_AUTO_TEST_CASE(constructor){
    BOOST_CHECK_NO_THROW(IOVDbSvcCurl("https://home.cern"));
  }
  BOOST_AUTO_TEST_CASE(getCernHomePage){
    IOVDbSvcCurl myCurlObject("https://home.cern");
    BOOST_CHECK_NO_THROW(myCurlObject.get());
    const auto & cernHome=myCurlObject.get();
    BOOST_CHECK(cernHome.substr(0,15) == "<!DOCTYPE html>");
  }
  BOOST_AUTO_TEST_CASE(getNonexistentPage){
    IOVDbSvcCurl myCurlObject("https://hgafsdjafgda");
    BOOST_CHECK_THROW(myCurlObject.get(), CurlException);
  }
  BOOST_AUTO_TEST_CASE(getCrestPage){
    IOVDbSvcCurl myCurlObject("http://crest-undertow.web.cern.ch/crestapi/iovs?tagname=Indet_Align-channelList");
    const auto crestObj=myCurlObject.get();
    BOOST_CHECK(crestObj != "");
    BOOST_TEST_MESSAGE(crestObj);
  }

BOOST_AUTO_TEST_SUITE_END()
