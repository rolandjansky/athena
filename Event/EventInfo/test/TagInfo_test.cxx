/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/
/*
 */
/**
 * @file IOVDbSvc/test/BasicFolder_test.cxx
 * @author Shaun Roe
 * @date August, 2019
 * @brief Some tests for TagInfo class in the Boost test framework
 */

#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_MAIN
#define BOOST_TEST_MODULE TEST_EVENTINFO

#include <boost/test/unit_test.hpp>
#include "CxxUtils/checker_macros.h"
ATLAS_NO_CHECK_FILE_THREAD_SAFETY;
//
#include "EventInfo/TagInfo.h"
#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/ServiceHandle.h"
#include "GaudiKernel/IMessageSvc.h"
#include "GaudiKernel/StatusCode.h"
#include <algorithm>
#include <string>


BOOST_AUTO_TEST_SUITE(TagInfoTest)
  BOOST_AUTO_TEST_CASE(Constructor){
    BOOST_CHECK_NO_THROW(TagInfo());
  }
  BOOST_AUTO_TEST_CASE(EmptyObjectGetMethods){
    TagInfo t; 
    ServiceHandle<IMessageSvc> msgSvc("msgSvc","test");
    MsgStream log(msgSvc.get(), "EventInfo_TagInfo_test");
    BOOST_TEST_MESSAGE("Testing an empty TagInfo object first");
    //tagInfo
    BOOST_TEST(t.tagInfoTag() == "","Tag of empty TagInfo should be empty");
    //printTags 
    BOOST_CHECK_NO_THROW(t.printTags(log));//how to check output?
    std::string returnVal{"nonsense"};
    std::string searchForTag{"randomName"};
    //findTag
    BOOST_CHECK_NO_THROW(t.findTag(searchForTag, returnVal));
    BOOST_TEST(returnVal.empty(),"'findTag' on empty TagInfo returns empty string");
    TagInfo::NameTagPairVec vectorOfTagPairs{};
    //getTags
    BOOST_CHECK_NO_THROW(t.getTags(vectorOfTagPairs));
    BOOST_TEST(vectorOfTagPairs.empty(),"'getTags' on empty TagInfo returns empty vector");
    //findInputTag
    BOOST_CHECK_NO_THROW(t.findInputTag(searchForTag,returnVal));
    BOOST_TEST(returnVal.empty(),"'findInputTag' on empty TagInfo returns empty string");
    //getInputTags
    BOOST_CHECK_NO_THROW(t.getInputTags(vectorOfTagPairs));
    BOOST_TEST(returnVal.empty(),"'getInputTags' on empty TagInfo returns empty vector");
  }
  BOOST_AUTO_TEST_CASE(SetAndGetTagTests){
    //Start to fill the TagInfo object
    TagInfo t;
    std::string returnValue{};
    //add Tag for TagInfo object itself
    BOOST_TEST_MESSAGE("Adding tag for TagInfo object");
    const std::string reflexiveTag{"TagForTagInfoObject"};
    BOOST_CHECK_NO_THROW(t.setTagInfoTag(reflexiveTag));
    BOOST_TEST(t.tagInfoTag() == reflexiveTag,"The overall tag for the TagInfo object was set and retrieved");
    //add a single tag and check
    std::string singleName{"tagName"};
    std::string singleReference{"tagReference"};
    std::string updatedReference{"tagUpdate"};
    TagInfo::NameTagPair singleAddition{singleName,singleReference};
    BOOST_TEST(t.addTag(singleAddition) == StatusCode::SUCCESS,"Adding a single tag succeeds");
    t.findTag(singleName,returnValue);
    BOOST_TEST(returnValue == singleReference,"A single added tag can be retrieved");
    TagInfo::NameTagPair updatedNamePair{singleName,updatedReference};
    BOOST_TEST(t.addTag(updatedNamePair) == StatusCode::FAILURE,"Trying to add a duplicate tag name fails");//adding the same name should fail
    const bool forceOverride(true);
    BOOST_TEST(t.addTag(updatedNamePair, forceOverride) == StatusCode::SUCCESS, "Adding a duplicate tag name succeeds if 'override' is true");//...unless override is set
    t.findTag(singleName,returnValue);
    BOOST_TEST(returnValue == updatedReference,"A given tag name was successfully overridden");
    TagInfo::NameTagPairVec returnTagPairs;
    t.getTags(returnTagPairs);
    BOOST_TEST(returnTagPairs.size() == 1);
    BOOST_TEST((returnTagPairs.at(0) == updatedNamePair));
    //add multiple tags
    //add tags, deliberately disordered
    TagInfo::NameTagPairVec insertionTagPairs{{"tag0","reference0"},{"tag1", "reference1"},{"ztag","zreference"},{"atag", "areference"}};
    std::for_each(insertionTagPairs.begin(), insertionTagPairs.end(),[&t](auto &x){t.addTag(x);});
    auto allInsertedTags=insertionTagPairs;
    allInsertedTags.push_back(updatedNamePair); //allTags now contains all the tags we inserted (not in the same order, though)
    TagInfo::NameTagPairVec returnMultipleTagPairs;
    t.getTags(returnMultipleTagPairs);//these are sorted in the return vector
    BOOST_TEST(returnMultipleTagPairs.size() == allInsertedTags.size(),"Returned number of pairs matches the inserted number of pairs");
    std::sort(allInsertedTags.begin(), allInsertedTags.end());//note: I did not have to define a comparison operator
    BOOST_TEST(returnMultipleTagPairs == allInsertedTags);
  }
  BOOST_AUTO_TEST_CASE(SetAndGetInputTagTests){
    //Start to fill the TagInfo object with input tags
    TagInfo t;
    std::string returnValue{};
    //add a single input tag and check
    std::string singleName{"tagName"};
    std::string singleReference{"tagReference"};
    std::string updatedReference{"tagUpdate"};
    TagInfo::NameTagPair singleAddition{singleName,singleReference};
    BOOST_TEST(t.addInputTag(singleAddition) == StatusCode::SUCCESS,"Adding a single input tag succeeds");
    t.findInputTag(singleName,returnValue);
    BOOST_TEST(returnValue == singleReference,"A single added input tag can be retrieved");
    TagInfo::NameTagPair updatedNamePair{singleName,updatedReference};
    BOOST_TEST(t.addInputTag(updatedNamePair) == StatusCode::FAILURE,"Trying to add a duplicate input tag name fails");//adding the same name should fail
    const bool forceOverride(true);
    BOOST_TEST(t.addInputTag(updatedNamePair, forceOverride) == StatusCode::SUCCESS, "Adding a duplicate input tag name succeeds if 'override' is true");//...unless override is set
    t.findInputTag(singleName,returnValue);
    BOOST_TEST(returnValue == updatedReference,"A given tag name was successfully overridden");
    TagInfo::NameTagPairVec returnTagPairs;
    t.getInputTags(returnTagPairs);
    BOOST_TEST(returnTagPairs.size() == 1);
    BOOST_TEST((returnTagPairs.at(0) == updatedNamePair));
    //add multiple tags
    //add tags, deliberately disordered
    TagInfo::NameTagPairVec insertionTagPairs{{"tag0","reference0"},{"tag1", "reference1"},{"ztag","zreference"},{"atag", "areference"}};
    std::for_each(insertionTagPairs.begin(), insertionTagPairs.end(),[&t](auto &x){t.addInputTag(x);});
    auto allInsertedTags=insertionTagPairs;
    allInsertedTags.push_back(updatedNamePair); //allTags now contains all the tags we inserted (not in the same order, though)
    TagInfo::NameTagPairVec returnMultipleTagPairs;
    t.getInputTags(returnMultipleTagPairs);//these are sorted in the return vector
    BOOST_TEST(returnMultipleTagPairs.size() == allInsertedTags.size(),"Returned number of pairs matches the inserted number of pairs");
    std::sort(allInsertedTags.begin(), allInsertedTags.end());//note: I did not have to define a comparison operator
    BOOST_TEST(returnMultipleTagPairs == allInsertedTags,"The returned input tags match the inserted input tags");
  }
BOOST_AUTO_TEST_SUITE_END()
