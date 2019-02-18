/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/
/*
 */
/**
 * @file IOVDbSvc/test/IOVDbConn_Boost_test.cxx
 * @author Shaun Roe
 * @date Jan, 2019
 * @brief Some tests for IOVDbCoolFunctions in the Boost framework
 */

#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_MAIN
#define BOOST_TEST_MODULE TEST_IOVDBSVC

#include <boost/test/unit_test.hpp>
//
#include "CoolKernel/ChannelSelection.h"
#include "CoolKernel/ValidityKey.h"
#include "AthenaKernel/IOVTime.h"

#include "CoralBase/AttributeList.h"
#include "CoralBase/AttributeListSpecification.h"
#include "CoralBase/Attribute.h"
#include "CoralBase/AttributeSpecification.h"

#include "../src/IOVDbCoolFunctions.h"

BOOST_AUTO_TEST_SUITE(CoolFunctionsTest)
  BOOST_AUTO_TEST_CASE(inRange){
    typedef std::pair<int,int> Pair_t;
    Pair_t theRange(-1,2);
    int valueInRange{};
    int valueOutOfRange{4};
    BOOST_TEST(IOVDbNamespace::inRange(valueInRange,theRange));
    BOOST_TEST(!IOVDbNamespace::inRange(valueOutOfRange,theRange));
    std::vector<Pair_t> allOutOfRange{{11,12},{0,3},{7,9}};
    std::vector<Pair_t> oneInRange{{9,11},{8,9},{0,3}};
    BOOST_TEST(!IOVDbNamespace::inRange(10, allOutOfRange));
    BOOST_TEST(IOVDbNamespace::inRange(10, oneInRange));
  }
  //
  BOOST_AUTO_TEST_CASE(attrList2Spec){
    //need a coral::AttributeList argument
      auto pSpec=new coral::AttributeListSpecification;
      pSpec->extend<int>("myInt");
      pSpec->extend<std::string>("PoolRef");
      coral::AttributeList attrList(*pSpec, true);
      coral::AttributeList attrListCopy(IOVDbNamespace::attrList2Spec(attrList), true);
      bool returnedSpecIsSameAsOriginal = (attrList==attrListCopy);
      BOOST_TEST(returnedSpecIsSameAsOriginal);
  }
  //
  BOOST_AUTO_TEST_CASE(attributeSize){
    //need a coral::Attribute argument
    auto pSpec=new coral::AttributeListSpecification;
    pSpec->extend<int>("myInt");
    pSpec->extend<std::string>("PoolRef");
    coral::AttributeList attrList(*pSpec, true);
    attrList[0].setValue(1);
    attrList[1].setValue(std::string("hello"));
    BOOST_TEST(IOVDbNamespace::attributeSize(attrList[0])==4);
    BOOST_TEST(IOVDbNamespace::attributeSize(attrList[1])==5);
  }
  //
  BOOST_AUTO_TEST_CASE(typeSizeIsKnown){
    //need a coral::Attribute argument
    auto pSpec=new coral::AttributeListSpecification;
    pSpec->extend<int>("myInt");
    pSpec->extend<std::string>("PoolRef");
    coral::AttributeList attrList(*pSpec, true);
    
    BOOST_TEST(IOVDbNamespace::typeSizeIsKnown(attrList[0]));
    BOOST_TEST(IOVDbNamespace::typeSizeIsKnown(attrList[1]));
  }
  //
  BOOST_AUTO_TEST_CASE(attributeListSize){
    //needs coral::AttributeList argument
    auto pSpec=new coral::AttributeListSpecification;
    pSpec->extend<int>("myInt");
    pSpec->extend<std::string>("PoolRef");
    coral::AttributeList attrList(*pSpec, true);
    attrList[0].setValue(1);
    attrList[1].setValue(std::string("hello"));
    BOOST_TEST(IOVDbNamespace::attributeListSize(attrList) ==9);
    
  }
  //
  BOOST_AUTO_TEST_CASE(countSelectedChannels){
    //need a std::vector<cool::ChannelId> and a ChannelSelection argument
    std::vector<cool::ChannelId> someChannels{0,1,2,56,57,58,59,100};
    cool::ChannelSelection select3(58,100);//make a selection which is 58 to 100 inclusive
    BOOST_TEST(IOVDbNamespace::countSelectedChannels(someChannels,select3) == 3);
  }
  //
  BOOST_AUTO_TEST_CASE(makeEpochOrRunLumi){
    //needs a cool::ValidityKey argument (and a bool 'timeIsEpoch')
    cool::ValidityKey coolValidityKey{500};
    auto anIovTimeInEpoch=IOVDbNamespace::makeEpochOrRunLumi(coolValidityKey, true);
    auto anIovTimeInRunLumi=IOVDbNamespace::makeEpochOrRunLumi(coolValidityKey, false);
    BOOST_TEST_MESSAGE(anIovTimeInEpoch);
    BOOST_TEST_MESSAGE(anIovTimeInRunLumi);
  }
  
   
BOOST_AUTO_TEST_SUITE_END()
