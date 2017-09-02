/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: crc64_test.cxx,v 1.2 2007-03-08 02:02:06 ssnyder Exp $
/**
 * @file  SGTools/crc64_test.cxx
 * @author scott snyder
 * @date Mar 2007
 * @brief Regression tests for CRC-64 calculation.
 */

#undef NDEBUG

#include "SGTools/crc64.h"
#include <iostream>
#include <cassert>


void testextend (const std::string& str, unsigned int x)
{
  uint64_t crc1 = SG::crc64 (str);
  crc1 = SG::crc64addint (crc1, x);

  std::string str2 = str;
  while (x > 0) {
    str2.push_back ((char)(x&0xff));
    x >>= 8;
  }
  assert (crc1 == SG::crc64 (str2));
}


int main()
{
  const char* testin1 = "alkdkjasldjaldjalkdjaldjoqiwj;lknnfaoiuuhewfuasuhfaiuuhffiuuhaffoiuahfefiuauheofiuhapkjjjhdhfpiauw3hpkjajshhdfpkkuawheoiuahwefpiuawhfeiauehfpiauwehfpawieufhpaw";
  const char* testout1 = "047BDD061CE68225";
  const char* testin2 = "alkdkjasldjaldjalkdjalejoqiwj;lknnfaoiuuhewfuasuhfaiuuhffiuuhaffoiuahfefiuauheofiuhapkjjjhdhfpiauw2hpkjajshhdfpkkuawheoiuahwefpiuawhfeiauehfpiauwehfpawieufhpaw"; // DIffers from 1st in two bits.
  const char* testout2 = "6051B184A58A80C6";

  assert (SG::crc64digest (testin1) == std::string (testout1));
  assert (SG::crc64digest (testin2) == std::string (testout2));

  testextend (testin1, 54356);
  testextend (testin2, 9812674);
  return 0;
}

