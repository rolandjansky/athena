/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

/**
 * @author Shaun Roe
 * @brief Some tests for SCT_CalibUtilities in the Boost framework
 */

#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_MAIN
#define BOOST_TEST_MODULE TEST_SCT_CALIBUTILITIES

#include <boost/test/unit_test.hpp>
#include "src/SCT_CalibUtilities.h"


BOOST_AUTO_TEST_SUITE(SCT_CalibUtilitiesTest)
  BOOST_AUTO_TEST_CASE(normalizeList_test){
    const std::string testInput{"   my  test list  with spaces  "};
    const std::string expectedResponse{"my test list with spaces"};
    const auto answer{SCT_CalibAlgs::normalizeList(testInput)};
    BOOST_TEST(answer == expectedResponse);
  }
BOOST_AUTO_TEST_SUITE_END()
