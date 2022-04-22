/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
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
  GaudiKernelFixture():GaudiKernelFixtureBase(){
    //nop, everything in base.
  }
};

struct IOVDbParserFixture{
  ServiceHandle<IMessageSvc> msgSvc;
  const std::string descriptionString1;
  const std::string descriptionString2;
  MsgStream log;
  IOVDbParser parser1;
  IOVDbParser parser2;
  IOVDbParserFixture():msgSvc("msgSvc","test"),
   descriptionString1{"extraText<timeStamp>run-lumi</timeStamp><addrHeader><address_header service_type=\"71\" clid=\"40774348\" /></addrHeader><typeName>AthenaAttributeList</typeName><key>/PIXEL/CablingMap</key>"},
   descriptionString2{"extraText<timeStamp>time</timeStamp><addrHeader><address_header service_type=\"71\" clid=\"40774348\" /></addrHeader><typeName>AthenaAttributeList</typeName>"},
   log(msgSvc.get(), "IOVDbParser_test"),
   parser1(descriptionString1, log),
   parser2(descriptionString2, log){
   }
};

//Basic tests that the service can be retrieved
BOOST_FIXTURE_TEST_SUITE(IOVDbParserTest , GaudiKernelFixture)
  BOOST_AUTO_TEST_CASE( SanityCheck ){
    BOOST_TEST(svcLoc!=nullptr);
  }
  //tests parsing of folder description
  BOOST_FIXTURE_TEST_SUITE(IOVDbParserMethods, IOVDbParserFixture)
    BOOST_AUTO_TEST_CASE(publicMethods){
      BOOST_TEST(parser1.isValid());//parser was created ok
      const std::string defaultValue{"time"};
      std::string returnValue{};
      BOOST_TEST(parser1.getKey("timeStamp", defaultValue, returnValue));
      BOOST_TEST(returnValue == "run-lumi");
      BOOST_TEST(parser2.getKey("timeStamp", "", returnValue));
      BOOST_TEST(returnValue == "time");
      BOOST_TEST(!parser1.getKey("nonsense", defaultValue, returnValue));
      BOOST_TEST(returnValue == defaultValue);
      //IOVDbParser::spaceStrip method was removed in master (cf. rel. 21.X)
      BOOST_TEST(parser1.folderName() == "extraText");//I dont think this is satisfactory!
      BOOST_TEST(parser1.hasKey());
      BOOST_TEST(parser1.key() == "/PIXEL/CablingMap");//key exists
      BOOST_TEST(!parser2.hasKey());
      BOOST_TEST(parser2.key() == "extraText");//key does not exist, should return foldername
      std::string overridingDescription="extraText2<timeStamp>time</timeStamp><addrHeader><address_header service_type=\"71\" clid=\"40774348\" /></addrHeader><typeName>AthenaAttributeList</typeName><key>/PIXEL/CablingMap</key>";
      IOVDbParser parserOverride(overridingDescription, log);
      BOOST_TEST(parser1.applyOverrides(parserOverride, log) == 4);
      std::string equivalentDescription="extraText2<timeStamp>time</timeStamp><addrHeader><address_header service_type=\"71\" clid=\"40774348\" /></addrHeader><typeName>AthenaAttributeList</typeName><key>/PIXEL/CablingMap</key>";
      IOVDbParser parser3(overridingDescription, log);
      IOVDbParser equivalentParser(equivalentDescription, log);
      if ( (not parser3.isValid()) or (not equivalentParser.isValid()) ){
        throw std::runtime_error("Invalid parser creation");
      }
      bool testEqualityOperator = (parser3 == equivalentParser);
      bool testEqualityFalse = (parser1 == parser3);
      
      BOOST_TEST(testEqualityOperator);
      BOOST_TEST(!testEqualityFalse);
      BOOST_TEST(parser1.toString() == "Folder:extraText, Attributes: [addrHeader:<address_header service_type=\"71\" clid=\"40774348\" />] [key:/PIXEL/CablingMap] [timeStamp:time] [typeName:AthenaAttributeList] ");
      BOOST_TEST(parser1.tag()=="");
      BOOST_TEST(parser1.eventStoreName()=="StoreGateSvc");
      BOOST_TEST(parser1.timebaseIs_nsOfEpoch());
      BOOST_TEST(parser1.cache()=="");
      BOOST_TEST(parser1.cachehint()==0);
      BOOST_TEST(!parser1.named());
      BOOST_TEST(parser1.addressHeader()=="<address_header service_type=\"71\" clid=\"40774348\" />");
      BOOST_TEST(parser1.symLinks().empty());
      BOOST_TEST(!parser1.noTagOverride());
      BOOST_TEST(parser1.classId(log)==40774348);
      BOOST_TEST(!parser1.onlyReadMetadata());
      BOOST_TEST(!parser1.extensible());
      BOOST_TEST(!parser1.overridesIov(log));
      BOOST_TEST(parser1.iovOverrideValue(log)==0);
      //checks for ATEAM-666
      //the following doesnt close the forceTimestamp element correctly
      //std::string invalidString1{"extraText<timeStamp>time</timeStamp><forceTimestamp>123456</forceTimestamp<addrHeader><address_header service_type=\"71\" clid=\"40774348\" /></addrHeader><typeName>AthenaAttributeList</typeName>"};
      std::string invalidString1{"<db>COOLONL_INDET/CONDBR2</db> /Indet/Onl/Beampos <key>/Indet/Beampos</key><forceTimestamp>1536151928</forceTimestamp"};
      IOVDbParser parser4(invalidString1, log);
      BOOST_TEST( parser4.isValid() == false);
      BOOST_TEST(parser4.getKey("forceTimestamp", "", returnValue) == false);
      BOOST_TEST(parser1.iovOverrideValue(log)==0);
      BOOST_TEST(returnValue == "");
    }
  BOOST_AUTO_TEST_SUITE_END()
BOOST_AUTO_TEST_SUITE_END()
