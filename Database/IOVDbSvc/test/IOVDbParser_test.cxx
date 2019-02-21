/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/
/*
 */
/**
 * @file IOVDbSvc/test/IOVDbParser_test.cxx
 * @author Shaun Roe
 * @date Jan, 2019
 * @brief Some tests for IOVDbConn in the Boost framework
 */

#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_MAIN
#define BOOST_TEST_MODULE TEST_IOVDBSVC

#include <boost/test/unit_test.hpp>
//
#include "GaudiKernel/ServiceHandle.h"
#include "GaudiKernel/IMessageSvc.h"
#include "GaudiKernel/MsgStream.h"
//
#include "GaudiKernelFixtureBase.h"

#include "../src/IOVDbParser.h"

BOOST_TEST_DONT_PRINT_LOG_VALUE( IOVDbParser )// '<<' not implemented

struct GaudiKernelFixture:public GaudiKernelFixtureBase{
  GaudiKernelFixture():GaudiKernelFixtureBase(__FILE__){
    //nop, everything in base.
  }
};

struct IOVDbParserFixture{
  ServiceHandle<IMessageSvc> msgSvc;
  const std::string descriptionString;
  MsgStream log;
  IOVDbParser parser;
  IOVDbParserFixture():msgSvc("msgSvc","test"),
   descriptionString{"extraText<timeStamp>run-lumi</timeStamp><addrHeader><address_header service_type=\"71\" clid=\"40774348\" /></addrHeader><typeName>AthenaAttributeList</typeName><key>/PIXEL/CablingMap</key>"},
   log(msgSvc.get(), "IOVDbParser_test"),
   parser(descriptionString, log){
  }
};

//Basic tests that the service can be retrieved
BOOST_FIXTURE_TEST_SUITE(IOVDbParserTest , GaudiKernelFixture)
  BOOST_AUTO_TEST_CASE( SanityCheck ){
    BOOST_TEST(gaudiIsInitialised);
    BOOST_TEST(svcLoc!=nullptr);
  }
  //tests parsing of folder description
  BOOST_FIXTURE_TEST_SUITE(IOVDbParserMethods, IOVDbParserFixture)
    BOOST_AUTO_TEST_CASE(publicMethods){
      BOOST_TEST(parser.isValid());//parser was created ok
      const std::string defaultValue{"time"};
      std::string returnValue{};
      BOOST_TEST(parser.getKey("timeStamp", defaultValue, returnValue));
      BOOST_TEST(returnValue == "run-lumi");
      BOOST_TEST(!parser.getKey("nonsense", defaultValue, returnValue));
      BOOST_TEST(returnValue == defaultValue);
      BOOST_TEST(parser.spaceStrip(" a spaced string ") == "a spaced string");
      BOOST_TEST(parser.folderName() == "extraText");//I dont think this is satisfactory!
      std::string overridingDescription="extraText2<timeStamp>time</timeStamp><addrHeader><address_header service_type=\"71\" clid=\"40774348\" /></addrHeader><typeName>AthenaAttributeList</typeName><key>/PIXEL/CablingMap</key>";
      IOVDbParser parserOverride(overridingDescription, log);
      BOOST_TEST(parser.applyOverrides(parserOverride, log) == 4);
      std::string equivalentDescription="extraText2<timeStamp>time</timeStamp><addrHeader><address_header service_type=\"71\" clid=\"40774348\" /></addrHeader><typeName>AthenaAttributeList</typeName><key>/PIXEL/CablingMap</key>";
      IOVDbParser parser1(overridingDescription, log);
      IOVDbParser equivalentParser(equivalentDescription, log);
      if ( (not parser1.isValid()) or (not equivalentParser.isValid()) ){
        throw std::runtime_error("Invalid parser creation");
      }
      bool testEqualityOperator = (parser1 == equivalentParser);
      bool testEqualityFalse = (parser == parser1);
      BOOST_TEST(testEqualityOperator);
      BOOST_TEST(!testEqualityFalse);
      BOOST_TEST(parser.toString() == "Folder:extraText, Attributes: [addrHeader:<address_header service_type=\"71\" clid=\"40774348\" />] [key:/PIXEL/CablingMap] [timeStamp:time] [typeName:AthenaAttributeList] ");
    }
  BOOST_AUTO_TEST_SUITE_END()
BOOST_AUTO_TEST_SUITE_END()