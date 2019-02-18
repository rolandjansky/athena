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
#include <utility>
#include <vector>

#include "../src/IOVDbStringFunctions.h"
#include <stdexcept>



typedef std::pair<int, int> ChanRange;
BOOST_TEST_DONT_PRINT_LOG_VALUE( ChanRange )

BOOST_AUTO_TEST_SUITE(IOVDbStringFunctionsTest)
  BOOST_AUTO_TEST_CASE(spaceStrip){
    const std::string stringWithSpaces(" string to strip ");
    const std::string trimmedString=IOVDbNamespace::spaceStrip(stringWithSpaces);
    BOOST_TEST(trimmedString == "string to strip");
  }
  BOOST_AUTO_TEST_CASE(makeChannel){
    const int defaultValue=999;
    const std::string channel10{"10"};
    const std::string channelTooHigh("4294967295");//2^32 -1 
    const std::string empty("");
    const std::string nonsense("ghgkhuy");
    BOOST_TEST(IOVDbNamespace::makeChannel(channel10, defaultValue)==10);
    BOOST_TEST(IOVDbNamespace::makeChannel(empty,defaultValue) == defaultValue);
    BOOST_CHECK_THROW(IOVDbNamespace::makeChannel(channelTooHigh,defaultValue), std::out_of_range);
    BOOST_CHECK_THROW(IOVDbNamespace::makeChannel(nonsense,defaultValue), std::invalid_argument);
  }
  
  BOOST_AUTO_TEST_CASE(parseChannelSpec){
    const std::string specString{":3, 65:67,100:120, 130: "};
    const auto result = IOVDbNamespace::parseChannelSpec<int>(specString);
    std::vector<std::pair<int, int>> expected{{0,3},{65,67},{100,120},{130,2147483647}};
    BOOST_TEST(expected == result, boost::test_tools::per_element() );
  }
  
  BOOST_AUTO_TEST_CASE(iovFromTime_Run_or_LumiBlock_Strings){
    const std::string timeString{"301"};
    const std::string runString{"2341"};
    const std::string lumiString{"999"};
    const unsigned long long iovTime{301*1000000000LL};
    const unsigned long long iovRun{2341LL<<32LL};
    const unsigned long long iovLumi{999LL};
    BOOST_TEST(IOVDbNamespace::iovFromTimeString(timeString) == iovTime);
    BOOST_TEST(IOVDbNamespace::iovFromRunString(runString) == iovRun);
    BOOST_TEST(IOVDbNamespace::iovFromLumiBlockString(lumiString) == iovLumi);
  }
  
  BOOST_AUTO_TEST_CASE(parseClid){
    const std::string addrHeader="<addrHeader><address_header service_type=\"71\" clid=\"29079131\"/></addrHeader>";
    //note: there's no other sanity check here; a string will be parsed if it contains only clid="29079131"
    const std::string erroneousHeader="<addrHeader><address_header id=\"29079131\"/></addrHeader>";
    const std::string variantHeader="clid=\"29079131\"";
    const int expectedCLID=29079131;
    const int errorValue=-1;
    BOOST_TEST(IOVDbNamespace::parseClid(addrHeader) == expectedCLID);
    BOOST_TEST(IOVDbNamespace::parseClid(erroneousHeader) == errorValue);
    BOOST_TEST(IOVDbNamespace::parseClid(variantHeader) == expectedCLID);
  }
  
  BOOST_AUTO_TEST_CASE(parseTypename){
    const std::string header=R"delim(<timeStamp>run-lumi</timeStamp><key>EMFracClassify</key><addrHeader><address_header service_type="71" clid="29079131" /></addrHeader><typeName>CaloLocalHadCoeff</typeName>)delim";
    const std::string expectedTypename = "CaloLocalHadCoeff";
    BOOST_TEST(IOVDbNamespace::parseTypename(header) == expectedTypename);
  }
  
  BOOST_AUTO_TEST_CASE(deleteRegex){
    const std::string preDelete="this is a string";
    const std::string rx="is";
    const std::string expectedResult="th  a string";
    BOOST_TEST(IOVDbNamespace::deleteRegex(preDelete,rx) == expectedResult);
  }
  
  BOOST_AUTO_TEST_CASE(quote){
   const std::string inputString="Able was I, ere I saw Elba";
   const std::string expectedResult="\"Able was I, ere I saw Elba\"";
   BOOST_TEST(IOVDbNamespace::quote(inputString) == expectedResult);
  }
  
  BOOST_AUTO_TEST_CASE(sanitiseFilename){
    const std::string filePath="/this/is/full/Filename.txt";
    const std::string sanitisedPath="_this_is_full_Filename.txt";
    BOOST_TEST(IOVDbNamespace::sanitiseFilename(filePath) == sanitisedPath);
  }
  
  BOOST_AUTO_TEST_CASE(replaceNULL){
    const std::string stringContainingNULL("This is really NULL and void");
    const std::string stringWith_null("This is really null and void");
    BOOST_TEST(IOVDbNamespace::replaceNULL(stringContainingNULL) == stringWith_null);
  }
  
  BOOST_AUTO_TEST_CASE(sanitiseXml){
    std::string inputXml="<timeStamp>run-lumi</timeStamp><addrHeader><address_header service_type=\"71\" clid=\"1170039409\" /></addrHeader><typeName>piggy::AlignableTransformContainer</typeName>";
    std::string expectedResult="<timeStamp>run-lumi</timeStamp><addrHeader><address_header service_type=\\\"71\\\" clid=\\\"1170039409\\\" /></addrHeader><typeName>piggy::AlignableTransformContainer</typeName>";
    BOOST_TEST(IOVDbNamespace::sanitiseXml(inputXml) == expectedResult);
  }
  
  BOOST_AUTO_TEST_CASE(tagIsMagic){
    std::string inputTag="TagInfoMajor/AtlasLayerMat_v21_/GeoAtlas";
    BOOST_TEST(IOVDbNamespace::tagIsMagic(inputTag));
  }
  BOOST_AUTO_TEST_CASE(parseMagicTag){
    std::string inputTag="TagInfoMajor/AtlasLayerMat_v21_/GeoAtlas";
    const auto parsedTag=IOVDbNamespace::parseMagicTag(inputTag);
    BOOST_TEST(parsedTag[1] == "Major");
    BOOST_TEST(parsedTag[2] == "AtlasLayerMat_v21_");
    BOOST_TEST(parsedTag[3] == "GeoAtlas");
  }
  BOOST_AUTO_TEST_CASE(tag2PrefixTarget){
    std::string inputTag="TagInfoMajor/AtlasLayerMat_v21_/GeoAtlas";
    const auto parsedTag=IOVDbNamespace::parseMagicTag(inputTag);
    const auto prefixTarget=IOVDbNamespace::tag2PrefixTarget(parsedTag);
    BOOST_TEST(prefixTarget.first == "AtlasLayerMat_v21_");
    BOOST_TEST(prefixTarget.second == "GeoAtlas");
  }
  
  BOOST_AUTO_TEST_CASE(replaceServiceType71){
    std::string mutableXml = "<timeStamp>run-lumi</timeStamp><addrHeader><address_header service_type=\"71\" clid=\"1170039409\" /></addrHeader><typeName>piggy::AlignableTransformContainer</typeName>";
    const std::string expectedResult="<timeStamp>run-lumi</timeStamp><addrHeader><address_header service_type=\"256\" clid=\"1170039409\" /></addrHeader><typeName>piggy::AlignableTransformContainer</typeName>";
    BOOST_TEST(IOVDbNamespace::replaceServiceType71(mutableXml));
    BOOST_TEST(mutableXml == expectedResult);
  }
  
  BOOST_AUTO_TEST_CASE(parseLinkNames){
    const std::string linkList="A:X::B:C";
    const auto parsedLinks=IOVDbNamespace::parseLinkNames(linkList);
    const std::vector<std::string> expectedResult{"A", "X::B", "C"};
    BOOST_TEST( parsedLinks == expectedResult, boost::test_tools::per_element() );
  }
BOOST_AUTO_TEST_SUITE_END()