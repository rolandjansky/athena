/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/
/*
 */
/**
 * @file IOVDbSvc/test/ReadFromFileMetaData_test.cxx
 * @author Shaun Roe
 * @date Jan, 2019
 * @brief Some tests for the ReadFromFileMetaData class in the Boost framework
 */

#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_MAIN
#define BOOST_TEST_MODULE TEST_IOVDBSVC

#include <boost/test/unit_test.hpp>
//
#include "AthenaKernel/IOVTime.h"
#include "CoolKernel/ValidityKey.h"
#include "IOVDbDataModel/IOVMetaDataContainer.h"
#include "../src/ReadFromFileMetaData.h"
//
//
#include "GaudiKernelFixtureBase.h"

struct GaudiKernelFixture:public GaudiKernelFixtureBase{
  GaudiKernelFixture():GaudiKernelFixtureBase(__FILE__){
    //nop, everything in base.
  }
};


struct IOVMetaDataContainerFixture{
  const std::string folderName;
  const std::string  folderDescription;
  IOVMetaDataContainer metaDataContainer;
  IOVMetaDataContainerFixture():folderName{"/key1"}, 
   folderDescription{"<timeStamp>run-event</timeStamp><addrHeader><address_header service_type=\"71\" clid=\"1238547719\" /></addrHeader><typeName>CondAttrListCollection</typeName>"},
   metaDataContainer{folderName, folderDescription}{
    //nop
  }
  ~IOVMetaDataContainerFixture(){ /**nop **/}
};

BOOST_FIXTURE_TEST_SUITE(ReadFromFileMetaDataTest , GaudiKernelFixture)
  IOVMetaDataContainerFixture container;
  BOOST_AUTO_TEST_CASE(ReadFromFileMetaData){
    const IOVTime timeInterval(10, 15);
    const bool useEpochTime(false);
    BOOST_CHECK_NO_THROW(IOVDbNamespace::ReadFromFileMetaData r(&container.metaDataContainer, timeInterval, useEpochTime));
    BOOST_CHECK_NO_THROW(IOVDbNamespace::ReadFromFileMetaData r(&container.metaDataContainer, cool::ValidityKeyMax, useEpochTime));
  }
  BOOST_AUTO_TEST_CASE(PublicMethods){
    const IOVTime timeInterval(10, 15);
    const bool useEpochTime(false);
    IOVDbNamespace::ReadFromFileMetaData readObject(&container.metaDataContainer, timeInterval, useEpochTime);
    BOOST_TEST(readObject.isValid());
    BOOST_TEST(readObject.folderType() == IOVDbNamespace::AttrListColl);
    BOOST_TEST(readObject.attrListCollection() !=nullptr);
    BOOST_TEST(readObject.attributeList() == nullptr);
    BOOST_TEST(readObject.numberOfObjects() == 0);
    BOOST_TEST(readObject.stringAddress().empty());
    BOOST_TEST(readObject.poolPayloadRequested() == false);
    BOOST_TEST(readObject.range() == IOVRange(IOVTime(0,0), IOVTime(2147483647,4294967295))); //why?
  }
BOOST_AUTO_TEST_SUITE_END()