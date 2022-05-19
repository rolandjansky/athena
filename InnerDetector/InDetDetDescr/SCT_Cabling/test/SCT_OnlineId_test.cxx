/*
   Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
 */
 
 
/**
 * @file SCT_Cabling/test/SCT_OnlineId_test.cxx
 * @author Shaun Roe
 * @date May 2020
 * @brief Some tests for TrkExToolsStringUtility 
 */
 


#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_MAIN
#define BOOST_TEST_MODULE TEST_SCT_CABLING_SCT_ONLINEID

#include "CxxUtils/checker_macros.h"
ATLAS_NO_CHECK_FILE_THREAD_SAFETY;

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Woverloaded-virtual"
#include <boost/test/unit_test.hpp>
#pragma GCC diagnostic pop

#include "SCT_Cabling/SCT_OnlineId.h"
#include <cstdint>
#include <memory>
#include <atomic>

namespace utf = boost::unit_test;




BOOST_AUTO_TEST_SUITE(SCT_OnlineIdTest)

  /* 
    Test the probe
  */
  BOOST_AUTO_TEST_CASE(SCT_OnlineIdConstructors){

    BOOST_CHECK_NO_THROW([[maybe_unused]] SCT_OnlineId s);
    std::uint32_t onlineId{1};
    BOOST_CHECK_NO_THROW([[maybe_unused]] SCT_OnlineId s(onlineId));
    std::uint32_t rodId{1};
    std::uint32_t fibre{2};
    BOOST_CHECK_NO_THROW([[maybe_unused]] SCT_OnlineId s(rodId, fibre));
  
  }
  
  BOOST_AUTO_TEST_CASE(SCT_OnlineIdDefaultMethods){
    //default constructed Id should be invalid
    SCT_OnlineId s;
    BOOST_CHECK(not s.is_valid());
    BOOST_CHECK(s.index() == SCT_OnlineId::INVALID_INDEX);
    
    BOOST_CHECK(s.rod() == SCT_OnlineId::INVALID_ROD);
    
    BOOST_CHECK(s.fibre() == SCT_OnlineId::INVALID_FIBRE);
    BOOST_CHECK(unsigned(s) == SCT_OnlineId::INVALID_ONLINE_ID);
    BOOST_CHECK(not s.rodIdInRange(s.rod()));
    //incrementing an invalid identifier should still be invalid
    BOOST_CHECK(not (++s).is_valid());
    BOOST_CHECK(not (s++).is_valid());
  }
  BOOST_AUTO_TEST_CASE(SCT_OnlineIdValidlyConstructedMethods){
    //try an invalid one first
    SCT_OnlineId invalid(20,107);
    BOOST_CHECK(not invalid.is_valid());
    //construct with valid rod id and fibre number
    SCT_OnlineId s(0x210000,1); 
    BOOST_CHECK(s.is_valid());
    BOOST_CHECK(s.index() == 1);
    BOOST_CHECK(s.rod() == 0x210000);
    BOOST_CHECK(s.fibre() == 1);
    BOOST_CHECK(unsigned(s) == 18939904);
    //construct from an unsigned int
    SCT_OnlineId t(18939904);
    BOOST_CHECK(s == t);
    BOOST_CHECK(s.rodIdInRange(s.rod()));
    //increment gives you the next fibre...
    BOOST_CHECK((++s).fibre() == 2);
    //..but it actually increments the bit in position 24
    BOOST_CHECK(unsigned(s) == 35717120);
    //..while the rod remains the same
    BOOST_CHECK(s.rod() == 0x210000);
    //What if the fibre is near the end?
    SCT_OnlineId lastFibre(0x210000,94);
    ++lastFibre; //now its the last fibre, still ok
    BOOST_CHECK(lastFibre.fibre() == 95);
    // cppcheck-suppress postfixOperator; deliberate here, for testing
    lastFibre++; //beyond the number of valid fibres for a ROD, not ok
    BOOST_CHECK(not lastFibre.is_valid());
    //and if I ask for the fibre?
    BOOST_CHECK(lastFibre.fibre() == SCT_OnlineId::INVALID_FIBRE);
  }
  
BOOST_AUTO_TEST_SUITE_END()
