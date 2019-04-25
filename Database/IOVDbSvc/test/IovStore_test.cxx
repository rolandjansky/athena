/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

/**
 * @file IOVDbSvc/test/IovStore_test.cxx
 * @author Shaun Roe
 * @date Jan, 2019
 * @brief Some tests for IovStore 
 */

#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_MAIN
#define BOOST_TEST_MODULE TEST_IOVDBSVC


#include <boost/test/unit_test.hpp>
//
#include <utility>
#include <vector>

#include "../src/IovStore.h"
#include <stdexcept>
namespace utf = boost::unit_test;

using namespace IOVDbNamespace;

BOOST_AUTO_TEST_SUITE(IovStore_test)

  BOOST_AUTO_TEST_CASE(Constructors){
    BOOST_CHECK_NO_THROW(IovStore x);
    IovStore::Iov_t withBounds(0,100);
    BOOST_CHECK_NO_THROW(IovStore x(withBounds));
  }
  BOOST_AUTO_TEST_CASE(PublicMethods){
    IovStore defaultStore;
    IovStore::Iov_t defaultBounds(0,0);
    bool defaultObjectHasDefaultBounds = (defaultStore.getCacheBounds() == defaultBounds);
    BOOST_TEST(defaultObjectHasDefaultBounds);
    IovStore::Iov_t bounds(0,100);
    IovStore storeWithBounds(bounds);
    bool boundsConstructorGivesCorrectBounds = (storeWithBounds.getCacheBounds() == bounds);
    BOOST_TEST(boundsConstructorGivesCorrectBounds);
    IovStore::Iov_t newBounds(10,200);
    storeWithBounds.setCacheBounds(newBounds);
    bool settingNewBoundsGivesCorrectAnswer = (storeWithBounds.getCacheBounds() == newBounds);
    BOOST_TEST(settingNewBoundsGivesCorrectAnswer);
    IovStore::Iov_t lowerCrossingIov(5,15);
    IovStore::Iov_t upperCrossingIov(190,210);
    IovStore::Iov_t withinBoundsIov(20,80);
    IovStore::Iov_t crossingBothBoundsIov(3,300);
    storeWithBounds.addIov(lowerCrossingIov);
    typedef std::pair<unsigned int, unsigned int> intPair;
    intPair zero(0,0);
    intPair count0(1,0);
    bool correctCount0 = (storeWithBounds.numberOfIovsOnBoundaries() == count0);
    BOOST_TEST(correctCount0);
    storeWithBounds.addIov(upperCrossingIov);
    intPair count1(1,1);
    auto count2=count1;
    bool correctCount1 = (storeWithBounds.numberOfIovsOnBoundaries() == count1);
    BOOST_TEST(correctCount1);
    storeWithBounds.addIov(withinBoundsIov);
    bool correctCount2 = (storeWithBounds.numberOfIovsOnBoundaries() == count2);
    BOOST_TEST(correctCount2);
    storeWithBounds.addIov(crossingBothBoundsIov);
    intPair count3(2,2);
    bool correctCount3 = (storeWithBounds.numberOfIovsOnBoundaries() == count3);
    BOOST_TEST(correctCount3);
    auto span=storeWithBounds.getMinimumStraddlingSpan();
    IovStore::Iov_t correctSpan(5,210);
    //bool minimumSpanIsCorrect=(correctSpan==span);
    BOOST_TEST(correctSpan.first == span.first);
    BOOST_TEST(correctSpan.second == span.second);
    storeWithBounds.clear();
    auto resetToZeroCorrectly=(storeWithBounds.numberOfIovsOnBoundaries() == zero);
    BOOST_TEST(resetToZeroCorrectly);
  }
  
BOOST_AUTO_TEST_SUITE_END()