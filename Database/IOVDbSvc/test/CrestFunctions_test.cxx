/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/
/*
 */
/**
 * @file IOVDbSvc/test/CrestFunctions_test.cxx
 * @author Shaun Roe
 * @date July, 2019
 * @brief Some tests for CrestFunctions 
 */

#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_MAIN
#define BOOST_TEST_MODULE TEST_IOVDBSVC


#include <boost/test/unit_test.hpp>
//


#include "../src/CrestFunctions.h"
#include "CoolKernel/ChannelId.h"
#include <string>

using namespace IOVDbNamespace;


//dont want unit test to depend on whether the CREST db is up or not
static const bool dontUseRealDatabase=true;

BOOST_AUTO_TEST_SUITE(CrestFunctionsTest)
  BOOST_AUTO_TEST_CASE(urlBase_test){
    const std::string baseUrl=urlBase();
    BOOST_TEST(baseUrl == "http://crest-01.cern.ch:8080");
  }
  
  BOOST_AUTO_TEST_CASE(extractHashFromJson_test){
    const std::string sampleJson{R"delim([{"tagName":"Indet_Align-description","since":0,"insertionTime":"2019-07-08T15:03:15.742+0000","payloadHash":"dd7a656e41d7b6426164fc411aa7ff51aa4bc047ab885a6e00b976b62291c18b"}])delim"};
    BOOST_TEST(extractHashFromJson(sampleJson) == "dd7a656e41d7b6426164fc411aa7ff51aa4bc047ab885a6e00b976b62291c18b");
    const std::string nonsense{"blubbyPinkBox333"};
    BOOST_TEST(extractHashFromJson(nonsense) == "");
  }
  
  BOOST_AUTO_TEST_CASE(getIovsForTag_test){
    const std::string tag{"Indet_Align-channelList"};
    const std::string referenceReply{"551e8b2807dea49dd91933ba963d8eff78b3441ae5836cd17cddad26ec14ecc3"};
    BOOST_TEST(getIovsForTag(tag, dontUseRealDatabase) == referenceReply);
  }
  
  BOOST_AUTO_TEST_CASE(getPayloadForHash_test){
    const std::string hash{"551e8b2807dea49dd91933ba963d8eff78b3441ae5836cd17cddad26ec14ecc3"};
    const std::string referenceReply{R"delim({"channel_list": ["0", "100", "101", "200", "201", "202", "203", "204", "205", "206", "207", "208", "209", "210", "211", "212", "213", "214", "215", "216", "217", "218", "219", "220", "221", "222", "223", "224", "225", "226", "227", "228", "229", "230"]})delim"};
    BOOST_TEST(getPayloadForHash(hash, dontUseRealDatabase) == referenceReply);
  }
  
  BOOST_AUTO_TEST_CASE(extractChannelListFromJson_test){
    std::string json{R"delim({"channel_list": ["0", "100", "101", "200", "201", "202", "203", "204", "205", "206", "207", "208", "209", "210", "211", "212", "213", "214", "215", "216", "217", "218", "219", "220", "221", "222", "223", "224", "225", "226", "227", "228", "229", "230"]})delim"};
    const std::vector<cool::ChannelId> expectedReply{0, 100, 101, 200, 201, 202, 203, 204, 205, 206, 207, 208, 209, 210, 211, 212, 213, 214, 215, 216, 217, 218, 219, 220, 221, 222, 223, 224, 225, 226, 227, 228, 229, 230};
    BOOST_TEST(extractChannelListFromJson(json) == expectedReply, boost::test_tools::per_element());
  }
  
  BOOST_AUTO_TEST_CASE(channelListForTag_test){
    const std::string folderTag{"Indet_Align"};
    const std::vector<cool::ChannelId> expectedReply{0, 100, 101, 200, 201, 202, 203, 204, 205, 206, 207, 208, 209, 210, 211, 212, 213, 214, 215, 216, 217, 218, 219, 220, 221, 222, 223, 224, 225, 226, 227, 228, 229, 230};
    BOOST_TEST(channelListForTag(folderTag, dontUseRealDatabase) == expectedReply, boost::test_tools::per_element());
  }
  
  BOOST_AUTO_TEST_CASE(getPayloadForTag_test){
    const std::string tag("Indet_Align-channelList");
    std::string json{R"delim({"channel_list": ["0", "100", "101", "200", "201", "202", "203", "204", "205", "206", "207", "208", "209", "210", "211", "212", "213", "214", "215", "216", "217", "218", "219", "220", "221", "222", "223", "224", "225", "226", "227", "228", "229", "230"]})delim"};
    BOOST_TEST(getPayloadForTag(tag, dontUseRealDatabase)  == json);
  }
  
  BOOST_AUTO_TEST_CASE(extractDescriptionFromJson_test){
    const std::string jsonDescr{R"delim({"node_description": "<timeStamp>run-lumi</timeStamp><addrHeader><address_header service_type=\"71\" clid=\"1170039409\" /></addrHeader><typeName>AlignableTransformContainer</typeName>"})delim"};
    const std::string expectedAnswer{R"delim(<timeStamp>run-lumi</timeStamp><addrHeader><address_header service_type="71" clid="1170039409" /></addrHeader><typeName>AlignableTransformContainer</typeName>)delim"};
    BOOST_TEST(extractDescriptionFromJson(jsonDescr) == expectedAnswer);
  }
  
  BOOST_AUTO_TEST_CASE(folderDescriptionForTag_test){
    const std::string folderTag{"Indet_Align"};
    const std::string expectedReply{R"delim(<timeStamp>run-lumi</timeStamp><addrHeader><address_header service_type="71" clid="1170039409" /></addrHeader><typeName>AlignableTransformContainer</typeName>)delim"};
    BOOST_TEST(folderDescriptionForTag(folderTag, dontUseRealDatabase) == expectedReply);
  }
  
BOOST_AUTO_TEST_SUITE_END()
