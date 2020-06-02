/*
   Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
 */
 
 
/**
 * @file TrkExTools/test/TrkExToolsStringUtility_test.cxx
 * @author Shaun Roe
 * @date May 2020
 * @brief Some tests for TrkExToolsStringUtility 
 */
 


#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_MAIN
#define BOOST_TEST_MODULE TEST_TRKEXTOOLSSTRINGUTILITY

#include "CxxUtils/checker_macros.h"
ATLAS_NO_CHECK_FILE_THREAD_SAFETY;

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Woverloaded-virtual"
#include <boost/test/unit_test.hpp>
#pragma GCC diagnostic pop
//

#include "../src/TrkExToolsStringUtility.h"

BOOST_AUTO_TEST_SUITE(TrkExToolsStringUtilityTest)
  /* 
    std::string_view
    getToolSuffix(const std::string_view fullToolName);
  */
  BOOST_AUTO_TEST_CASE(getToolSuffixTest){
    using TrkExTools::getToolSuffix;
    BOOST_TEST(getToolSuffix("myOwner.ATool") == std::string("ATool"));
    BOOST_TEST(getToolSuffix("") == std::string(""));
    BOOST_TEST(getToolSuffix("myToolNoOwner") == std::string("myToolNoOwner"));
  }
  
  /* 
    std::vector<std::string>
    extractToolNames(const std::vector<std::string> & toolNameVector);
  */
  BOOST_AUTO_TEST_CASE(extractToolNamesTest){
    using TrkExTools::extractToolNames;
    const std::vector<std::string> goodListOfTools{"p1.t1", "p2.t2", "p3.t3"};
    const std::vector<std::string> listOfSuffices{"t1", "t2", "t3"};
    BOOST_TEST(extractToolNames(goodListOfTools) == listOfSuffices, boost::test_tools::per_element());
  }
  
  /* 
    bool
    validToolName(const std::string & toolName);
  */
  BOOST_AUTO_TEST_CASE(validToolNameTest){
    using TrkExTools::validToolName;
    const std::string empty{};
    const std::string aValidString{"myToolName"};
    const std::string invalidString{"2BorNot2B"};
    const std::string invalidString2{"B>BorNot2B"};
    // Example from https://acode-browser1.usatlas.bnl.gov/lxr/source/athena/MuonSpectrometer/MuonConfig/python/MuonRecToolsConfig.py#0229
    // giving rise to ATLASRECTS-5500
    const std::string muonExample{"Trk::EnergyLossUpdator"};
    const std::string muonExampleWithTypo{"Trk:EnergyLossUpdator"};
    BOOST_TEST(validToolName(aValidString) == true);
    BOOST_TEST(validToolName(empty) == false);
    BOOST_TEST(validToolName(invalidString) == false);
    BOOST_TEST(validToolName(invalidString2) == false);
    BOOST_TEST(validToolName(muonExample) == true);
    BOOST_TEST(validToolName(muonExampleWithTypo) == false);
  }
  /*
    std::string
    possibleToolNameError(const std::vector<std::string> & toolNameVector);
  */
  BOOST_AUTO_TEST_CASE(possibleToolNameErrorTest){
    using TrkExTools::possibleToolNameError;
    const std::vector<std::string> listOfValidTools{"t1", "t2", "t3"};
    BOOST_TEST(possibleToolNameError(listOfValidTools).empty());
    const std::vector<std::string> listHasEmptyName{"", "t2", "t3"};
    BOOST_TEST(possibleToolNameError(listHasEmptyName).empty() == false);
    const std::vector<std::string> listHasInvalidName{"1t", "t2", "t3"};
    BOOST_TEST(possibleToolNameError(listHasInvalidName).empty() == false);
    const std::vector<std::string> listHasDuplicatedName{"t3", "t2", "t3"};
    BOOST_TEST(possibleToolNameError(listHasDuplicatedName).empty() == true);
  }
  
  /*
    unsigned int
    numberOfUniqueEntries(const std::vector<std::string> & nameVector);
  */
  BOOST_AUTO_TEST_CASE(numberOfUniqueEntriesTest){
    using TrkExTools::numberOfUniqueEntries;
    const std::vector<std::string> emptyVector{};
    const std::vector<std::string> threeValidTools{"t1", "t2", "t3"};
    const std::vector<std::string> twoUniqueNamesOutOfThree{"t3", "t2", "t3"};
    BOOST_TEST(numberOfUniqueEntries(emptyVector) == 0u);
    BOOST_TEST(numberOfUniqueEntries(threeValidTools) == 3u);
    BOOST_TEST(numberOfUniqueEntries(twoUniqueNamesOutOfThree) == 2u);
  }
BOOST_AUTO_TEST_SUITE_END()  
