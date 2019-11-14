/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/
/*
 */
/**
 * @file IOVDbSvc/test/IOVDbStringFunctions_test.cxx
 * @author Shaun Roe
 * @date Jan, 2019
 * @brief Some tests for IOVDbStringFunctions 
 */

#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_MAIN
#define BOOST_TEST_MODULE TEST_IOVDBSVC


#include <boost/test/unit_test.hpp>
//


#include "../src/Base64Codec.h"
#include "CoralBase/Blob.h"


BOOST_AUTO_TEST_SUITE(Base64CodecTest)
  BOOST_AUTO_TEST_CASE(encode){
    const unsigned char rawString[14]="Hello, world!";
    coral::Blob helloWorld(13);//omit the terminating null
    memcpy(helloWorld.startingAddress(),rawString,13);
    BOOST_TEST(IOVDbNamespace::base64Encode(helloWorld) == "SGVsbG8sIHdvcmxkIQ==");
  }
BOOST_AUTO_TEST_SUITE_END()