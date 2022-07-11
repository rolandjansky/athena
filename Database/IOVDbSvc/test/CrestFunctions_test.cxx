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
    const std::string tag{"LARIdentifierFebRodMap-RUN2-000"};
    const std::string referenceReply{"99331506eefbe6783a8d5d5bc8b9a44828a325adfcaac32f62af212e9642db71"};
    BOOST_TEST(getIovsForTag(tag, dontUseRealDatabase) == referenceReply);
  }
  
  BOOST_AUTO_TEST_CASE(getPayloadForHash_test){
    const std::string hash{"b8c82f0d2c3443a8c0b72c69d79e57205ba11a55ab7107d3e666294a3607f09d"};
    const std::string referenceReply{R"delim({"data":{"0":["[DB=B2E3B2B6-B76C-DF11-A505-000423D5ADDA][CNT=CollectionTree(LArTTCell_P/LArTTCellMapAtlas)][CLID=DF8C509C-A91A-40B5-B76C-5B57EEE21EC3][TECH=00000202][OID=00000003-00000000]"]}})delim"};
    BOOST_TEST(getPayloadForHash(hash, dontUseRealDatabase) == referenceReply);
  }
  
  BOOST_AUTO_TEST_CASE(extractChannelListFromJson_test){
    std::string json{R"delim({"format":"TagMetaSetDto","resources":[{"tagName":"LARBadChannelsOflBadChannels-RUN2-UPD4-21","description":"{\"dbname\":\"CONDBR2\",\"nodeFullpath\":\"/LAR/BadChannelsOfl/BadChannels\",\"schemaName\":\"COOLOFL_LAR\"}","chansize":8,"colsize":5,"tagInfo":"{\"channel_list\":[{\"0\":\"\"},{\"1\":\"\"},{\"2\":\"\"},{\"3\":\"\"},{\"4\":\"\"},{\"5\":\"\"},{\"6\":\"\"},{\"7\":\"\"}],\"node_description\":\"<timeStamp>run-lumi</timeStamp><addrHeader><address_header service_type=\\\"71\\\" clid=\\\"1238547719\\\" /></addrHeader><typeName>CondAttrListCollection</typeName>\",\"payload_spec\":\"ChannelSize:UInt32,StatusWordSize:UInt32,Endianness:UInt32,Version:UInt32,Blob:Blob64k\"}","insertionTime":"2022-05-26T16:40:32+0000"}],"size":1,"datatype":"tagmetas","format":null,"page":null,"filter":null})delim"};
    const std::vector<cool::ChannelId> expectedReply{0, 1, 2, 3, 4, 5, 6, 7};
    BOOST_TEST(extractChannelListFromJson(json) == expectedReply, boost::test_tools::per_element());
  }
  
  BOOST_AUTO_TEST_CASE(channelListForTag_test){
    const std::string folderTag{"LARBadChannelsOflBadChannels-RUN2-UPD4-21"};
    const std::vector<cool::ChannelId> expectedReply{0, 1, 2, 3, 4, 5, 6, 7};
    BOOST_TEST(channelListForTag(folderTag, dontUseRealDatabase) == expectedReply, boost::test_tools::per_element());
  }
  
  BOOST_AUTO_TEST_CASE(getPayloadForTag_test){
    const std::string tag("LARIdentifierLArTTCellMapAtlas-RUN2-HadFcalFix2");
    const std::string referenceReply{R"delim({"data":{"0":["[DB=B2E3B2B6-B76C-DF11-A505-000423D5ADDA][CNT=CollectionTree(LArTTCell_P/LArTTCellMapAtlas)][CLID=DF8C509C-A91A-40B5-B76C-5B57EEE21EC3][TECH=00000202][OID=00000003-00000000]"]}})delim"};
    BOOST_TEST(getPayloadForTag(tag, dontUseRealDatabase)  == referenceReply);
  }
  
  BOOST_AUTO_TEST_CASE(extractDescriptionFromJson_test){
    const std::string jsonDescr{R"delim({"format":"TagMetaSetDto","resources":[{"tagName":"LARAlign-RUN2-UPD4-03","description":"{\"dbname\":\"CONDBR2\",\"nodeFullpath\":\"/LAR/Align\",\"schemaName\":\"COOLONL_LAR\"}","chansize":1,"colsize":1,"tagInfo":"{\"channel_list\":[{\"0\":\"\"}],\"node_description\":\"<timeStamp>run-lumi</timeStamp><addrHeader><address_header service_type=\\\"71\\\" clid=\\\"254546453\\\" /></addrHeader><typeName>DetCondKeyTrans</typeName>\",\"payload_spec\":\"PoolRef:String4k\"}","insertionTime":"2022-05-26T12:10:38+0000"}],"size":1,"datatype":"tagmetas","format":null,"page":null,"filter":null})delim"};
    const std::string expectedAnswer{R"delim(<timeStamp>run-lumi</timeStamp><addrHeader><address_header service_type=\\"71\\" clid=\\"254546453\\" /></addrHeader><typeName>DetCondKeyTrans</typeName>)delim"};
    BOOST_TEST_MESSAGE(expectedAnswer);
    BOOST_TEST_MESSAGE(extractDescriptionFromJson(jsonDescr));
    BOOST_TEST(extractDescriptionFromJson(jsonDescr) == expectedAnswer);
  }
  
  BOOST_AUTO_TEST_CASE(folderDescriptionForTag_test){
    const std::string folderTag{"LARAlign-RUN2-UPD4-03"};
    const std::string expectedReply{R"delim(<timeStamp>run-lumi</timeStamp><addrHeader><address_header service_type=\\"71\\" clid=\\"254546453\\" /></addrHeader><typeName>DetCondKeyTrans</typeName>)delim"};
    BOOST_TEST(folderDescriptionForTag(folderTag, dontUseRealDatabase) == expectedReply);
  }
  
  BOOST_AUTO_TEST_CASE(resolveCrestTag_test){
    const std::string globalTag{"CREST-RUN12-SDR-25-MC"};
    const std::string folderName{"/LAR/Align"};
    const std::string expectedReply{"LARAlign-RUN2-UPD4-03"};
    BOOST_TEST(resolveCrestTag(globalTag, folderName,"",dontUseRealDatabase) == expectedReply);
  }
  
BOOST_AUTO_TEST_SUITE_END()
