/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/
/*
 */
/**
 * @file IOVDbSvc/test/Json2Cool_test.cxx
 * @author Shaun Roe
 * @date May, 2019
 * @brief Some tests for Json2Cool class in the Boost framework
 */

#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_MAIN
#define BOOST_TEST_MODULE TEST_IOVDBSVC


#include <boost/test/unit_test.hpp>

#include <boost/test/tools/output_test_stream.hpp>

#include "../src/Json2Cool.h"
#include "../src/BasicFolder.h"

#include "CoralBase/AttributeList.h"
#include "CoralBase/AttributeListSpecification.h"
#include "CoralBase/Attribute.h"
#include "CoralBase/AttributeSpecification.h"
#include "CoolKernel/StorageType.h"
//
#include "CoolKernel/RecordSpecification.h"
#include "CoolKernel/Record.h"

#include <istream>
#include <string>
#include <sstream>

using namespace IOVDbNamespace;
using namespace cool;
const auto pixelJson=R"foo({"node_description" : "<timeStamp>run-lumi</timeStamp><addrHeader><address_header service_type=\"71\" clid=\"1238547719\" /></addrHeader><typeName>CondAttrListCollection</typeName>", "folder_payloadspec": "stave: Int32, eta: Int32, mag: Float, base: Float, free: Float", "iov" : [0, 4294967295], "data_array" : [{ "100" : [ 0, 0, 0, 0, 0]},{ "200" : [ 1, 0, 0.3, 0, 0]}]})foo";


BOOST_AUTO_TEST_SUITE(Json2CoolTest)
  BOOST_AUTO_TEST_CASE(Constructor){
    BasicFolder b;
    std::istringstream initializerStream(pixelJson);
    BOOST_CHECK_NO_THROW(Json2Cool j(initializerStream,b));
    BOOST_CHECK(b.empty() == false);
  }
  BOOST_AUTO_TEST_CASE(convertedProperties){
    auto pSpec=new coral::AttributeListSpecification;
    pSpec->extend<int>("stave");
    pSpec->extend<int>("eta");
    pSpec->extend<float>("mag");
    pSpec->extend<float>("base");
    pSpec->extend<float>("free");
    coral::AttributeList attrList(*pSpec, true);
    attrList[0].setValue(1);
    attrList[1].setValue(0);
    attrList[2].setValue(0.3f);
    attrList[3].setValue(0.f);
    attrList[4].setValue(0.f);
    BasicFolder b;
    std::istringstream initializerStream(pixelJson);
    BOOST_CHECK_NO_THROW(Json2Cool j(initializerStream,b));
    BOOST_CHECK(b.getPayload(200) == attrList);
    const std::pair<cool::ValidityKey, cool::ValidityKey> refIov(0, 4294967295);
    BOOST_CHECK(b.iov() == refIov);
  }
  
  BOOST_AUTO_TEST_CASE(parsePayloadSpec){
    const std::string testSpecString="crate: UChar, ROB: Int32, BCIDOffset: Int16, AName: String255";
    auto referenceSpec = new cool::RecordSpecification();
    referenceSpec->extend("crate", StorageType::UChar);
    referenceSpec->extend("ROB", StorageType::Int32);
    referenceSpec->extend("BCIDOffset", StorageType::Int16);
    referenceSpec->extend("AName", StorageType::String255);
    auto returnedSpec = Json2Cool::parsePayloadSpec(testSpecString);
    BOOST_CHECK(*(returnedSpec) == *static_cast<const cool::IRecordSpecification*>(referenceSpec));
  }
  BOOST_AUTO_TEST_CASE(createAttributeList){
    auto referenceSpec = new cool::RecordSpecification();
    referenceSpec->extend("crate", StorageType::UChar);
    referenceSpec->extend("ROB", StorageType::Int32);
    referenceSpec->extend("BCIDOffset", StorageType::Int16);
    referenceSpec->extend("AName", StorageType::String255);
    const std::string jsonValues="[1,2,3,\"purple\"]";
    std::istringstream initializerStream(jsonValues);
    nlohmann::json j;
    initializerStream >>j;
    auto record=Json2Cool::createAttributeList(referenceSpec, j);
    cool::Record reference(*referenceSpec);
    BOOST_CHECK(record.size() == reference.size());
    //
    auto & att0=const_cast<coral::Attribute&>(reference.attributeList()[0]);
    unsigned char set0(1);
    att0.setValue<unsigned char>(set0);
    //
    auto & att1=const_cast<coral::Attribute&>(reference.attributeList()[1]);
    att1.setValue<int>(2);
    //
    auto & att2=const_cast<coral::Attribute&>(reference.attributeList()[2]);
    short set2(3);
    att2.setValue<short>(set2);
    //
    auto & att3=const_cast<coral::Attribute&>(reference.attributeList()[3]);
    att3.setValue<std::string>("purple");
    //
    BOOST_CHECK(reference == record);
  }

BOOST_AUTO_TEST_SUITE_END()

