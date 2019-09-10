/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/
/*
 */
/**
 * @file IOVDbSvc/test/BasicFolder_test.cxx
 * @author Shaun Roe
 * @date May, 2019
 * @brief Some tests for BasicFolder class in the Boost framework
 */

#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_MAIN
#define BOOST_TEST_MODULE TEST_IOVDBSVC


#include <boost/test/unit_test.hpp>
//
#include "CoralBase/AttributeList.h"
#include "CoralBase/AttributeListSpecification.h"
#include "CoralBase/Attribute.h"
#include "CoralBase/AttributeSpecification.h"

#include "../src/BasicFolder.h"
#include <string>

using namespace IOVDbNamespace;
BOOST_AUTO_TEST_SUITE(BasicFolderTest)
  BOOST_AUTO_TEST_CASE(constructor){
    BOOST_CHECK_NO_THROW(BasicFolder());
  }
  BOOST_AUTO_TEST_CASE(emptyAfterConstruction){
    BasicFolder b;
    BOOST_CHECK(b.empty());
    BOOST_CHECK(b.isVectorPayload() ==false);
  }
  BOOST_AUTO_TEST_CASE(addPayloadByChannelNumber){
    BasicFolder b;
    //need a coral::AttributeList argument
    auto pSpec=new coral::AttributeListSpecification;
    pSpec->extend<int>("myInt");
    pSpec->extend<std::string>("PoolRef");
    coral::AttributeList attrList(*pSpec, true);
    attrList[0].setValue(1);
    attrList[1].setValue(std::string("hello"));
    BOOST_CHECK_NO_THROW(b.addChannelPayload(0,attrList));
    BOOST_CHECK(b.empty() == false);
    BOOST_CHECK(b.getPayload(0) == attrList);
  }
  
  BOOST_AUTO_TEST_CASE(addPayloadWithChannelName){
    BasicFolder b;
    //need a coral::AttributeList argument
    auto pSpec=new coral::AttributeListSpecification;
    pSpec->extend<int>("myInt");
    pSpec->extend<std::string>("PoolRef");
    coral::AttributeList attrList(*pSpec, true);
    attrList[0].setValue(1);
    attrList[1].setValue(std::string("hello"));
    BOOST_CHECK_NO_THROW(b.addChannelPayload(0,"uniqueChannel",attrList));
    BOOST_CHECK(b.empty() == false);
    BOOST_CHECK(b.getPayload("uniqueChannel") == attrList);
  }
  BOOST_AUTO_TEST_CASE(setVectorPayloadFlag){
    BasicFolder b;
    BOOST_CHECK_NO_THROW(b.setVectorPayloadFlag(true));
    BOOST_CHECK(b.isVectorPayload() == true);
  }
  BOOST_AUTO_TEST_CASE(addVectorPayloadByChannelNumber){
    BasicFolder b;
    b.setVectorPayloadFlag(true);
    //need a coral::AttributeList argument
    auto pSpec=new coral::AttributeListSpecification;
    pSpec->extend<int>("myInt");
    pSpec->extend<std::string>("PoolRef");
    coral::AttributeList attrList(*pSpec, true);
    std::vector<coral::AttributeList> vectorPayload{
     coral::AttributeList(*pSpec, true),
     coral::AttributeList(*pSpec, true),
     coral::AttributeList(*pSpec, true)
    };
    //set values
    vectorPayload[0][0].setValue(1);
    vectorPayload[0][1].setValue(std::string("hello"));
    //
    vectorPayload[1][0].setValue(2);
    vectorPayload[1][1].setValue(std::string("bonjour"));
    //
    vectorPayload[2][0].setValue(3);
    vectorPayload[2][1].setValue(std::string("ni hao"));
    //
    BOOST_CHECK_NO_THROW(b.addChannelPayload(0,vectorPayload));
    BOOST_CHECK(b.empty() == false);
    BOOST_TEST(b.getVectorPayload(0) == vectorPayload);
  }
  
  BOOST_AUTO_TEST_CASE(addVectorPayloadByName){
    BasicFolder b;
    b.setVectorPayloadFlag(true);
    //need a coral::AttributeList argument
    auto pSpec=new coral::AttributeListSpecification;
    pSpec->extend<int>("myInt");
    pSpec->extend<std::string>("PoolRef");
    coral::AttributeList attrList(*pSpec, true);
    std::vector<coral::AttributeList> vectorPayload{
     coral::AttributeList(*pSpec, true),
     coral::AttributeList(*pSpec, true),
     coral::AttributeList(*pSpec, true)
    };
    //set values
    vectorPayload[0][0].setValue(1);
    vectorPayload[0][1].setValue(std::string("hello"));
    //
    vectorPayload[1][0].setValue(2);
    vectorPayload[1][1].setValue(std::string("bonjour"));
    //
    vectorPayload[2][0].setValue(3);
    vectorPayload[2][1].setValue(std::string("ni hao"));
    //
    BOOST_CHECK_NO_THROW(b.addChannelPayload(0,"yummy",vectorPayload));
    BOOST_CHECK(b.empty() == false);
    BOOST_TEST(b.getVectorPayload("yummy") == vectorPayload);
  }
  
  BOOST_AUTO_TEST_CASE(checkChannelIds){
    BasicFolder b;
    //need a coral::AttributeList argument
    auto pSpec=new coral::AttributeListSpecification;
    pSpec->extend<int>("myInt");
    pSpec->extend<std::string>("PoolRef");
    coral::AttributeList attrList(*pSpec, true);
    attrList[0].setValue(1);
    attrList[1].setValue(std::string("hello"));
    b.addChannelPayload(100,attrList);
    b.addChannelPayload(200,attrList);
    const std::vector<cool::ChannelId> refChannelIds{100,200};
    BOOST_TEST(b.channelIds() == refChannelIds, boost::test_tools::per_element() );
  }
  
  BOOST_AUTO_TEST_CASE(setAndGetIov){
    BasicFolder b;
    const std::pair<cool::ValidityKey, cool::ValidityKey> refIov(56,200);
    BOOST_CHECK_NO_THROW(b.setIov(refIov));
    BOOST_CHECK(b.iov() == refIov);
  }
  

BOOST_AUTO_TEST_SUITE_END()
