/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/
 

 
#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_MAIN
#define BOOST_TEST_MODULE TEST_BITCALCULATOR
 

 
#include <boost/test/unit_test.hpp>
#include "BarcodeServices/BitCalculator.h"
 
 
namespace utf = boost::unit_test; 
using namespace Barcode;
 
BOOST_AUTO_TEST_SUITE(BitCalculatorTest)
 
  BOOST_AUTO_TEST_CASE( Construction ){
    BOOST_TEST_MESSAGE("Testing construction");
    BOOST_CHECK_NO_THROW(BitCalculator b);    
  }
  
  BOOST_AUTO_TEST_CASE( GetMethods ){
    BitCalculator b;
    constexpr int exampleBarcode = 155454554;
    constexpr int parent = exampleBarcode & 0xFFF; //first 12 bits
    constexpr int pidParent = (exampleBarcode & 0x3FF000) >>12; //next 10 bits
    constexpr int hs = (exampleBarcode & 0x400000) >>22; //next one bit
    constexpr int sim = (exampleBarcode & 0x3800000) >>23;//next three bits
    constexpr int bcid = (exampleBarcode & 0xFC000000) >>26;//next six bits
    BOOST_TEST(b.GetParentID(exampleBarcode) == parent);
    BOOST_TEST(b.GetPIDofParent(exampleBarcode) == pidParent);
    BOOST_TEST(b.GetHS(exampleBarcode) == hs);
    BOOST_TEST(b.GetSimulator(exampleBarcode) == sim);
    BOOST_TEST(b.GetBCID(exampleBarcode) == bcid);
  }
  
   BOOST_AUTO_TEST_CASE( SetMethods, * utf::expected_failures(1)){
    BitCalculator b;
    int exampleBarcode = 155454554;
   
    b.SetParentID(exampleBarcode,9);
    b.SetPIDofParent(exampleBarcode,100) ;
    b.SetHS(exampleBarcode, 0);
    b.SetSimulator(exampleBarcode, 7);
    b.SetBCID(exampleBarcode, 15);
    BOOST_TEST(b.GetParentID(exampleBarcode) == 9);
    BOOST_TEST(b.GetPIDofParent(exampleBarcode) == 100);
    BOOST_TEST(b.GetHS(exampleBarcode) == 0);
    BOOST_TEST(b.GetSimulator(exampleBarcode) == 7);
    BOOST_TEST(b.GetBCID(exampleBarcode) == 15);
    //the following fails, 48 is 0b11 0000, and the 'GetBCID' method returns zero
    b.SetBCID(exampleBarcode, 48);
    BOOST_TEST(b.GetBCID(exampleBarcode) == 48);
    //try setting all 5 bits, 0b1 1111:
    b.SetBCID(exampleBarcode, 31);
    BOOST_TEST(b.GetBCID(exampleBarcode) == 31);
    //this is likely why: its a 6 bit signed number, from -31 to 31
    b.SetBCID(exampleBarcode, -3);
    BOOST_TEST(b.GetBCID(exampleBarcode) == -3);
  }
BOOST_AUTO_TEST_SUITE_END()
 
